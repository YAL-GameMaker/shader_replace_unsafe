function scr_fsh() {return /*hlsl*/@'
// Varyings
static float4 _v_vColour = {0, 0, 0, 0};
static float2 _v_vTexcoord = {0, 0};

static float4 gl_Color[4] =
{
    float4(0, 0, 0, 0),
    float4(0, 0, 0, 0),
    float4(0, 0, 0, 0),
    float4(0, 0, 0, 0)
};

cbuffer DriverConstants : register(b1)
{
};

uniform float _gm_AlphaRefValue : register(c0);
uniform bool _gm_AlphaTestEnabled : register(c1);
uniform SamplerState sampler__gm_BaseTexture : register(s0);
uniform Texture2D texture__gm_BaseTexture : register(t0);
uniform float4 _gm_FogColour : register(c2);
uniform bool _gm_PS_FogEnabled : register(c3);

float4 gl_texture2D(Texture2D t, SamplerState s, float2 uv)
{
    return t.Sample(s, uv);
}

#define GL_USES_MRT
#define GL_USES_FRAG_COLOR
;
;
;
;
;
void _DoAlphaTest(in float4 _SrcColour)
{
{
if(_gm_AlphaTestEnabled)
{
{
if((_SrcColour.w <= _gm_AlphaRefValue))
{
{
discard;
;
}
;
}
;
}
;
}
;
}
}
;
void _DoFog(inout float4 _SrcColour, in float _fogval)
{
{
if(_gm_PS_FogEnabled)
{
{
(_SrcColour = lerp(_SrcColour, _gm_FogColour, clamp(_fogval, 0.0, 1.0)));
}
;
}
;
}
}
;
;
;
void gl_main()
{
{
(gl_Color[0] = ((_v_vColour * gl_texture2D(texture__gm_BaseTexture, sampler__gm_BaseTexture, _v_vTexcoord))));
}
}
;
struct PS_INPUT
{
    float4 v0 : TEXCOORD0;
    float2 v1 : TEXCOORD1;
};

struct PS_OUTPUT
{
    float4 gl_Color0 : SV_Target0;
    float4 gl_Color1 : SV_Target1;
    float4 gl_Color2 : SV_Target2;
    float4 gl_Color3 : SV_Target3;
};

PS_OUTPUT main(PS_INPUT input)
{
    _v_vColour = input.v0;
    _v_vTexcoord = input.v1.xy;

    gl_main();

    PS_OUTPUT output;
    output.gl_Color0 = gl_Color[0];
    output.gl_Color1 = gl_Color[0];
    output.gl_Color2 = gl_Color[0];
    output.gl_Color3 = gl_Color[0];

    return output;
}
';}