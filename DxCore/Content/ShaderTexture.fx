// ShaderTexture.fx : Implements the shader.
//
// By Geelix School of Serious Games and Edutainment.


Texture2D colorMap : register(t0);
SamplerState colorSampler : register(s0);


struct VS_Input
{
    float4 pos  : POSITION;
    float2 tex0 : TEXCOORD0;
};


struct PS_Input
{
    float4 pos  : SV_POSITION;
    float2 tex0 : TEXCOORD0;
};


PS_Input VS_Main(VS_Input vertex)
{
    PS_Input vsOut = (PS_Input)0;

    vsOut.pos = vertex.pos;
    vsOut.tex0 = vertex.tex0;

    return vsOut;
}


float4 PS_Main(PS_Input frag) : SV_TARGET
{
    float4 noColor = float4(0, 0, 0, 0.5f);
    float4 sideColor = float4(1, 0, 0, 1);
    
    float width = 0.05;
    float high = 1.0 - width;
    float low = width;
    
    if (frag.tex0.x > high || frag.tex0.x < low)
    {
        return sideColor;
    }
    
    if (frag.tex0.y > high || frag.tex0.y < low)
    {
        return sideColor;
    }
    
    return noColor;
    
    //return colorMap.Sample(colorSampler, frag.tex0);
}
