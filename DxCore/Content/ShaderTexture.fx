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
    float3 noColor = float3(0, 0, 0);
    float3 sideColor = float3(1, 0, 0);
    
    float width = 0.05;
    float high = 1.0 - width;
    float low = width;
    
    float3 final =  lerp(noColor, sideColor, step(high, frag.tex0.y));
    final +=  lerp(sideColor, noColor, step(low, frag.tex0.y));
    final +=  lerp(sideColor, noColor, step(low, frag.tex0.x));
    final +=  lerp(noColor, sideColor, step(high, frag.tex0.x));
    
    return float4(final, 1);
    //return colorMap.Sample(colorSampler, frag.tex0);
}
