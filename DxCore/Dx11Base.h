#pragma once
// Dx11Base.h : Defines the CDx11Base class.
//
// By Geelix School of Serious Games and Edutainment.

#include <d3d11.h>
#include <d3dx11.h>
#include <d3dcompiler.h>
#include <dxerr.h>
#include <xnamath.h>
#include <chrono>

class CDx11Base
{
    // Constructors
    public:
    CDx11Base();
    virtual ~CDx11Base();

    // Methods
    public:
    bool Initialize(HWND hWnd, HINSTANCE hInst);
    void Terminate();
    bool CompileShader(LPCWSTR szFilePath, LPCSTR szFunc, LPCSTR szShaderModel, ID3DBlob** buffer);

    // Overrides
    public:
    virtual bool LoadContent() = 0;
    virtual void UnloadContent() = 0;

    virtual void Update();
    virtual void Render() = 0;

    // Attributes
    public:
    HWND m_hWnd;
    HINSTANCE m_hInst;
    ID3D11Device* m_pD3DDevice;
    ID3D11DeviceContext* m_pD3DContext;
    ID3D11RenderTargetView*	m_pD3DRenderTargetView;
    IDXGISwapChain* m_pSwapChain;

    std::chrono::time_point<std::chrono::steady_clock> startPoint;
    float elapsedTime = 0.f;
    float deltaTime = 0.f;
};
