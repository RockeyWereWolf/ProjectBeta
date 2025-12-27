cbuffer Constants {
    float4x4 g_ViewProj; // Camera Matrix
};

struct VSInput {
    float3 Pos : ATTRIB0;
    float2 UV  : ATTRIB1;
    float3 Col : ATTRIB2; // Pre-baked shading
};

struct PSInput {
    float4 Pos : SV_POSITION;
    float3 Col : COLOR0;
    float2 UV  : TEX_COORD;
};

void main(in  VSInput VSIn,
          out PSInput PSIn) 
{
    // Transform local vertex to screen space
    PSIn.Pos = mul(float4(VSIn.Pos, 1.0), g_ViewProj);
    PSIn.Col = VSIn.Col;
    PSIn.UV  = VSIn.UV;
}
