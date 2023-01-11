#include "stdafx.h"
#include "gml_d3d.h"
#include <map>
#include <string>

ID3D11Device* d3dDevice = nullptr;
ID3D11DeviceContext* d3dContext = nullptr;

#pragma warning(push)
#pragma warning(disable : 26812)
YYShaderVarType D3DtoGMLvarType(_D3D_SHADER_VARIABLE_TYPE t) {
	switch (t) {
		case D3D_SVT_VOID:
		case D3D_SVT_BOOL:
		case D3D_SVT_INT:
			return (YYShaderVarType)t;

		case D3D_SVT_FLOAT:
			return YYShaderVarType::Float;

		case D3D_SVT_UINT:
			return YYShaderVarType::Uint;

		case D3D_SVT_DOUBLE:
			return YYShaderVarType::Double;

			// Is it min8int? if so, then it's won't be avaliable in GM
		case D3D_SVT_UINT8:
			return YYShaderVarType::Byte;

		default:
			return YYShaderVarType::Void;
	}
}
#pragma warning(pop)

static std::map<std::string, YYShaderInputSemantic> D3DtoGMLsemanticName_init() {
	std::map<std::string, YYShaderInputSemantic> result;
	result["POSITION"] = YYShaderInputSemantic::Position;
	result["COLOR"] = YYShaderInputSemantic::Color;
	result["NORMAL"] = YYShaderInputSemantic::Normal;
	result["TEXCOORD"] = YYShaderInputSemantic::TexCoord;
	result["BLENDWEIGHT"] = YYShaderInputSemantic::BlendWeight;
	result["BLENDINDEX"] = YYShaderInputSemantic::BlendIndex;
	result["PSIZE"] = YYShaderInputSemantic::PSize;
	result["TANGENT"] = YYShaderInputSemantic::Tangent;
	result["BINORMAL"] = YYShaderInputSemantic::Binormal;
	result["UNKNOWN1"] = YYShaderInputSemantic::Unknown1;
	result["UNKNOWN2"] = YYShaderInputSemantic::Unknown2;
	result["FOG"] = YYShaderInputSemantic::Fog;
	result["DEPTH"] = YYShaderInputSemantic::Depth;
	result["SAMPLE"] = YYShaderInputSemantic::Sample;
	return result;
}
static std::map<std::string, YYShaderInputSemantic> D3DtoGMLsemanticName_map = D3DtoGMLsemanticName_init();

YYShaderInputSemantic D3DtoGMLsemanticName(const char* name) {
	auto pair = D3DtoGMLsemanticName_map.find(name);
	if (pair != D3DtoGMLsemanticName_map.end()) {
		return pair->second;
	} else {
		// this is what GM does, you tell me
		return YYShaderInputSemantic::TexCoord;
	}
}