﻿#pragma once
// DemoTexture.cpp : Implements the CDemoTexture class.
//
// By Geelix School of Serious Games and Edutainment.
//

#include "DemoTexture.h"

// Vertex struct
struct Vertex
{
    XMFLOAT3 pos;
    XMFLOAT2 tex0;

    static void AddNew(Vertex* v, unsigned int& idx , int x, unsigned int y, float offset, XMFLOAT2 start)
    {
        v[idx++] = { XMFLOAT3( start.x + (x * offset), start.y + (y * offset), 1.0f ), XMFLOAT2( 0.0f, 0.0f ) };
        v[idx++] = { XMFLOAT3( start.x + (x * offset), start.y + (y * offset) + offset, 1.0f ), XMFLOAT2( 0.0f, 1.0f ) };
        v[idx++] = { XMFLOAT3( start.x + (x * offset) + offset, start.y + (y * offset) + offset, 1.0f ), XMFLOAT2( 1.0f, 1.0f ) };
        
        v[idx++] = { XMFLOAT3( start.x + (x * offset) + offset, start.y + (y * offset) + offset, 1.0f ), XMFLOAT2( 1.0f, 1.0f ) };
        v[idx++] = { XMFLOAT3( start.x + (x * offset) + offset, start.y + (y * offset), 1.0f ), XMFLOAT2( 1.0f, 0.0f ) };
        v[idx++] = { XMFLOAT3( start.x + (x * offset), start.y + (y * offset), 1.0f ), XMFLOAT2( 0.0f, 0.0f ) };
    }
};

// Time values struct
struct TimeValues
{
    XMFLOAT4 Time; // Elapsed, Delta, Elasped/2, Delta/2
};

//////////////////////////////////////////////////////////////////////
// Constructors

CDemoTexture::CDemoTexture()
{
    m_pVS = NULL;
    m_pPS = NULL;
    
    m_pInputLayout = NULL;
    m_pVertexBuffer = NULL;
    
    m_pColorMap = NULL;
    m_pColorMapSampler = NULL;

    m_pTimeValues = NULL;
}

CDemoTexture::~CDemoTexture()
{
}

//////////////////////////////////////////////////////////////////////
// Overrides

bool CDemoTexture::LoadContent()
{
    // Compile vertex shader
    ID3DBlob* pVSBuffer = NULL;
    bool res = CompileShader(L"Content/ShaderTexture.fx", "VS_Main", "vs_4_0", &pVSBuffer);
    if (res == false) {
        ::MessageBox(m_hWnd, L"Unable to load vertex shader", L"ERROR", MB_OK);
        return false;
    }

    // Create vertex shader
    HRESULT hr;
    hr = m_pD3DDevice->CreateVertexShader(
        pVSBuffer->GetBufferPointer(),
        pVSBuffer->GetBufferSize(),
        0, &m_pVS);
    if (FAILED(hr)) {
        if (pVSBuffer)
            pVSBuffer->Release();
        return false;
    }

    // Define input layout
    D3D11_INPUT_ELEMENT_DESC shaderInputLayout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };
    UINT numLayoutElements = ARRAYSIZE(shaderInputLayout);

    // Create input layout
    hr = m_pD3DDevice->CreateInputLayout(
        shaderInputLayout, numLayoutElements,
        pVSBuffer->GetBufferPointer(), 
        pVSBuffer->GetBufferSize(), 
        &m_pInputLayout);
    if (FAILED(hr)) {
        return false;
    }

    // Release VS buffer
    pVSBuffer->Release();
    pVSBuffer = NULL;

    // Compile pixel shader
    ID3DBlob* pPSBuffer = NULL;
    res = CompileShader(L"Content/ShaderTexture.fx", "PS_Main", "ps_4_0", &pPSBuffer);
    if (res == false) {
        ::MessageBox(m_hWnd, L"Unable to load pixel shader", L"ERROR", MB_OK);
        return false;
    }

    // Create pixel shader
    hr = m_pD3DDevice->CreatePixelShader(
        pPSBuffer->GetBufferPointer(),
        pPSBuffer->GetBufferSize(), 
        0, &m_pPS);
    if (FAILED(hr)) {
        return false;
    }

    // Cleanup PS buffer
    pPSBuffer->Release();
    pPSBuffer = NULL;

    const uint8_t dim = 3;
    const float offset = 0.1; // This does not build a correct grid yet
    //TODO: debug this
    // Should be building a grid mesh w/ repeating UVs
    const XMFLOAT2 start = XMFLOAT2(-0.5f, -0.5f);

    unsigned int vertIndex = 0;

    // Define triangle
    Vertex vertices[dim*dim*6];
    for(unsigned int y = 0; y < dim; ++y)
    {
        for(unsigned int x = 0; x < dim; ++x)
        {
            Vertex::AddNew(vertices, vertIndex, x, y, offset, start);
        }
    }

    // Vertex description
    D3D11_BUFFER_DESC vertexDesc;
    ::ZeroMemory(&vertexDesc, sizeof(vertexDesc));
    vertexDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexDesc.ByteWidth = sizeof(Vertex) * 6;

    // Resource data
    D3D11_SUBRESOURCE_DATA resourceData;
    ZeroMemory(&resourceData, sizeof(resourceData));
    resourceData.pSysMem = vertices;

    // Create vertex buffer
    hr = m_pD3DDevice->CreateBuffer(&vertexDesc, &resourceData, &m_pVertexBuffer);
    if (FAILED(hr)) {
        return false;
    }

    // Create time values buffer
    D3D11_BUFFER_DESC TimeValuesDesc;
    ZeroMemory(&TimeValuesDesc, sizeof(vertexDesc));
    TimeValuesDesc.Usage = D3D11_USAGE_DEFAULT;
    TimeValuesDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    TimeValuesDesc.ByteWidth = sizeof(TimeValues);
    hr = m_pD3DDevice->CreateBuffer(&TimeValuesDesc, nullptr, &m_pTimeValues);

    // Load texture
    hr = ::D3DX11CreateShaderResourceViewFromFile(
        m_pD3DDevice, L"Content/borg.dds", 0, 0, &m_pColorMap, 0);
    if (FAILED(hr)) {
        ::MessageBox(m_hWnd, L"Unable to load texture", L"ERROR", MB_OK);
        return false;
    }

    // Texture sampler
    D3D11_SAMPLER_DESC textureDesc;
    ::ZeroMemory(&textureDesc, sizeof(textureDesc));
    textureDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    textureDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    textureDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    textureDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    textureDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    textureDesc.MaxLOD = D3D11_FLOAT32_MAX;
    hr = m_pD3DDevice->CreateSamplerState(&textureDesc, &m_pColorMapSampler);
    if (FAILED(hr)) {
        ::MessageBox(m_hWnd, L"Unable to create texture sampler state", L"ERROR", MB_OK);
        return false;
    }

    return true;
}

void CDemoTexture::UnloadContent()
{
    // Cleanup
    if (m_pColorMap)
        m_pColorMap->Release();
    m_pColorMap = NULL;
    
    if (m_pColorMapSampler)
        m_pColorMapSampler->Release();
    m_pColorMapSampler = NULL;
    
    if (m_pVS)
        m_pVS->Release();
    m_pVS = NULL;
    
    if (m_pPS)
        m_pPS->Release();
    m_pPS = NULL;
    
    if (m_pInputLayout)
        m_pInputLayout->Release();
    m_pInputLayout = NULL;
    
    if (m_pVertexBuffer)
        m_pVertexBuffer->Release();
    m_pVertexBuffer = NULL;

    if (m_pTimeValues)
        m_pTimeValues->Release();
    m_pTimeValues = NULL;
}

void CDemoTexture::Update()
{
    CDx11Base::Update();

    //TODO: Update time constant buffer
}

void CDemoTexture::Render()
{
    // Check if D3D is ready
    if (m_pD3DContext == NULL)
        return;

    // Clear back buffer
    float color[4] = { 0.0f, 0.0f, 0.5f, 1.0f };
    m_pD3DContext->ClearRenderTargetView(m_pD3DRenderTargetView, color);

    // Stride and offset
    UINT stride = sizeof(Vertex);
    UINT offset = 0;

    // Set vertex buffer
    m_pD3DContext->IASetInputLayout(m_pInputLayout);
    m_pD3DContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
    m_pD3DContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // Set shaders
    //TODO: Add constant buffer for time values to pixel shader
    m_pD3DContext->VSSetShader(m_pVS, 0, 0);
    m_pD3DContext->PSSetShader(m_pPS, 0, 0);
    m_pD3DContext->PSSetShaderResources(0, 1, &m_pColorMap);
    m_pD3DContext->PSSetSamplers(0, 1, &m_pColorMapSampler);

    // Draw triangles
    m_pD3DContext->Draw(54, 0);

    // Present back buffer to display
    m_pSwapChain->Present(0, 0);
}
