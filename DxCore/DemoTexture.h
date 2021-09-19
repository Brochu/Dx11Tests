#pragma once
// DemoTexture.h : Defines the CDemoTexture class.
//
// By Geelix School of Serious Games and Edutainment.
//

#include "Dx11Base.h"

class CDemoTexture : public CDx11Base
{
    // Constructors
    public:
    CDemoTexture();
    virtual ~CDemoTexture();

    // Overrides
    public:
    virtual bool LoadContent();
    virtual void UnloadContent();

    virtual void Update();
    virtual void Render();

    // Members
    protected:
    ID3D11VertexShader* m_pVS;
    ID3D11PixelShader* m_pPS;
    
    ID3D11InputLayout* m_pInputLayout;
    ID3D11Buffer* m_pVertexBuffer;
    
    ID3D11ShaderResourceView* m_pColorMap;
    ID3D11SamplerState* m_pColorMapSampler;

    ID3D11BlendState* m_pBlendState;

    ID3D11Buffer* m_pTimeValues;
    
    uint8_t m_dim = 6;
    float m_gridXSize = 0.2;
    float m_gridYSize = 0.2;
    XMFLOAT2 m_startPos = XMFLOAT2(-0.6f, -0.6f);
};
