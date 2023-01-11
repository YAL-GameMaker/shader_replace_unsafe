#pragma once
#include "stdafx.h"
#include <map>
#include <string>
#include "gml_internals.h"

extern ID3D11Device* d3dDevice;
extern ID3D11DeviceContext* d3dContext;

#pragma warning(push)
#pragma warning(disable : 26812)
YYShaderVarType D3DtoGMLvarType(_D3D_SHADER_VARIABLE_TYPE t);
#pragma warning(pop)

YYShaderInputSemantic D3DtoGMLsemanticName(const char* name);