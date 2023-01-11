#include "stdafx.h"
#include "shared.h"
#include "gml_d3d.h"
#include "gml_internals.h"

#include <d3d11.h>
#include <d3dcompiler.h>

extern int (*shader_get_uniform)(YYShader* shader, const char* name);
extern GMLFunc* f_shader_get_sampler_index;

void shader_sync_uniforms(YYShader* shader) {
	if (f_shader_get_sampler_index) {
		static RValue args[3];
		static bool ready = false;
		if (!ready) {
			args[0].kind = VALUE_REAL;
			args[0].flags = 0;

			args[1].kind = VALUE_STRING;
			args[1].flags = 0;
			auto str = new RefString();
			str->text = "gm_BaseTexture";
			str->refCount = 0x70000000;
			str->size = 0;
			args[1].str = str;

			args[2].kind = VALUE_REAL;
			args[2].flags = 0;
		}
		args[0].real = shader->id;
		f_shader_get_sampler_index(args[2], nullptr, nullptr, 2, args);
		shader->gm_BaseTexture = args[2].getInt32(0);
	} else shader->gm_BaseTexture = 0;
	shader->gm_Matrices = shader_get_uniform(shader, "gm_Matrices");
	shader->gm_Lights_Direction = shader_get_uniform(shader, "gm_Lights_Direction");
	shader->gm_Lights_PosRange = shader_get_uniform(shader, "gm_Lights_PosRange");
	shader->gm_Lights_Colour = shader_get_uniform(shader, "gm_Lights_Colour");
	shader->gm_AmbientColour = shader_get_uniform(shader, "gm_AmbientColour");
	shader->gm_LightingEnabled = shader_get_uniform(shader, "gm_LightingEnabled");
}
CustomShader* shader_add_impl(const char* vertex_code, const char* fragment_code, std::string name) {
	ID3D11VertexShader* vertex_shader = nullptr;
	ID3D11PixelShader* pixel_shader = nullptr;
	ID3DBlob* vertex_blob = nullptr;
	ID3DBlob* pixel_blob = nullptr;
	ID3D11ShaderReflection* vertex_reflection = nullptr;
	ID3D11ShaderReflection* pixel_reflection = nullptr;
	D3D_SHADER_MACRO macros[] = { { /* name: */nullptr, /* desc: */nullptr } };

	ID3DBlob* error_blob = nullptr; // might contain an error message
	HRESULT hr;

	#define m_try_break(_call, _text) {\
		hr = _call;\
		if (FAILED(hr)) {\
			shader_last_error = std::string(_text) + " (hresult:" + std::to_string(hr) + ")";\
			break;\
		}\
	}
	do { // once
		auto vs_model = "vs_" + shader_model;
		auto vs_path = shader_path + ".vsh";
		hr = D3DCompile(
			vertex_code, strlen(vertex_code), vs_path.c_str(),
			macros, NULL, "main", vs_model.c_str(),
			D3DCOMPILE_ENABLE_BACKWARDS_COMPATIBILITY, 0,
			&vertex_blob, &error_blob
		);
		m_try_break(D3DCompile(
			vertex_code, strlen(vertex_code), vs_path.c_str(),
			macros, NULL, "main", vs_model.c_str(),
			D3DCOMPILE_ENABLE_BACKWARDS_COMPATIBILITY, 0,
			&vertex_blob, &error_blob
		), "Vertex shader compile failed");

		auto ps_model = "ps_" + shader_model;
		auto ps_path = shader_path + ".fsh";
		m_try_break(D3DCompile(
			fragment_code, strlen(fragment_code), ps_path.c_str(),
			macros, NULL, "main", ps_model.c_str(),
			D3DCOMPILE_ENABLE_BACKWARDS_COMPATIBILITY, 0,
			&pixel_blob, &error_blob
		), "Pixel shader compile failed");

		m_try_break(d3dDevice->CreateVertexShader(
			vertex_blob->GetBufferPointer(),
			vertex_blob->GetBufferSize(),
			NULL, &vertex_shader
		), "Vertex shader create failed");

		m_try_break(d3dDevice->CreatePixelShader(
			pixel_blob->GetBufferPointer(),
			pixel_blob->GetBufferSize(),
			NULL, &pixel_shader
		), "Pixel shader create failed");

		m_try_break(D3DReflect(
			vertex_blob->GetBufferPointer(),
			vertex_blob->GetBufferSize(),
			IID_ID3D11ShaderReflection,
			(void**)&vertex_reflection
		), "Vertex shader reflection failed");

		m_try_break(D3DReflect(
			pixel_blob->GetBufferPointer(),
			pixel_blob->GetBufferSize(),
			IID_ID3D11ShaderReflection,
			(void**)&pixel_reflection
		), "Pixel shader reflection failed");

		std::vector<YYShaderConstBuf> cbufs{};
		std::vector<YYShaderInputParam> cinputs{};
		std::vector<YYShaderConstBufVar> cvars{};
		std::vector<YYShaderSampler> csamplers{};
		for (int pass = 0; pass < 2; pass++) {
			D3D11_SHADER_DESC rsd;
			D3D11_SHADER_BUFFER_DESC rbd;
			D3D11_SHADER_TYPE_DESC rtd;
			D3D11_SHADER_VARIABLE_DESC rvd;
			D3D11_SIGNATURE_PARAMETER_DESC rid;
			D3D11_SHADER_INPUT_BIND_DESC bdesc;
			auto passVert = pass == 0;
			ID3D11ShaderReflection* rs = passVert ? vertex_reflection : pixel_reflection;
			auto passType = passVert ? YYShaderKind::Vertex : YYShaderKind::Pixel;
			rs->GetDesc(&rsd);
			//
			if (passVert) for (auto i = 0u; i < rsd.InputParameters; i++) {
				rs->GetInputParameterDesc(i, &rid);
				cinputs.push_back({});
				auto& cinput = cinputs[cinputs.size() - 1];
				cinput.semantic = D3DtoGMLsemanticName(rid.SemanticName);
				cinput.semanticIndex = rid.SemanticIndex;
			}
			//
			for (auto i = 0u; i < rsd.BoundResources; i++) {
				auto ru = rs->GetResourceBindingDesc(i, &bdesc);
				if (bdesc.Name[0] == '$') continue; // $Globals..?
				csamplers.push_back({});
				auto& sampler = csamplers[csamplers.size()-1];
				auto nlen = strlen(bdesc.Name);
				auto name = new char[nlen + 1];
				memcpy_arr(name, bdesc.Name, nlen + 1);
				sampler.name = name;
				sampler.reg = bdesc.BindPoint;
				sampler.type = passType;
				//trace("name:%s", bdesc.Name);
			}
			//
			for (auto i = 0u; i < rsd.ConstantBuffers; i++) {
				auto rb = rs->GetConstantBufferByIndex(i);
				rb->GetDesc(&rbd);
				//
				auto cbufind = cbufs.size();
				cbufs.push_back({});
				auto& cbuf = cbufs[cbufind];
				cbuf.reg = i;
				cbuf.type = passType;
				cbuf.size = rbd.Size;
				cbuf.data = new uint8_t[rbd.Size];
				CD3D11_BUFFER_DESC bDesc(cbuf.size, D3D11_BIND_CONSTANT_BUFFER);
				d3dDevice->CreateBuffer(&bDesc, nullptr, &cbuf.buffer);
				ZeroMemory(cbuf.data, rbd.Size);
				cbuf.dirty = true;
				//
				for (auto k = 0u; k < rbd.Variables; k++) {
					auto rv = rb->GetVariableByIndex(k);
					rv->GetDesc(&rvd);
					auto rt = rv->GetType();
					rt->GetDesc(&rtd);
					//
					auto vname = (char*)rvd.Name;
					auto vexists = false;
					for (auto& v : cvars) {
						if (strcmp(v.name, vname) == 0) {
							vexists = true;
							break;
						}
					}
					if (vexists) continue;
					YYShaderConstBufVar v{};
					v.name = vname;
					v.bufferIndex = cbufind;
					v.offset = rvd.StartOffset; // Normalize to 16 byte alignment
					v.data = cbuf.data + v.offset; // ?
					v.type = D3DtoGMLvarType(rtd.Type);
					v.size = rvd.Size;
					v.rows = rtd.Rows;
					v.cols = rtd.Columns;
					v.elementCount = rtd.Elements;
					cvars.push_back(v);
				} // for const buf var
			} // for const buf
			//
			//for (auto i = 0u; i < rsd)
		} // for kind

		auto nsh = new YYNativeShader();
		yySetArrayWithCountToVector(nsh->constBuffers, nsh->constBufferCount, cbufs);
		yySetArrayWithCountToVector(nsh->constBufVars, nsh->constBufVarCount, cvars);
		yySetArrayWithCountToVector(nsh->inputs, nsh->inputCount, cinputs);
		yySetArrayWithCountToVector(nsh->samplers, nsh->samplerCount, csamplers);
		nsh->inputLayouts = 0; // (YYShaderInputLayout*)yyRunnerInterface.YYAlloc(sizeof(YYShaderInputLayout));
		nsh->inputLayoutCount = 0;
		nsh->lastUsedInputLayout = -1;

		nsh->vertexShader = vertex_shader;
		nsh->vertexHeader = new YYShaderDataHeader();
		nsh->vertexHeader->shaderData = vertex_blob->GetBufferPointer();
		nsh->vertexHeader->shaderSize = vertex_blob->GetBufferSize();

		nsh->pixelShader = pixel_shader;
		nsh->pixelHeader = new YYShaderDataHeader();
		nsh->pixelHeader->shaderData = pixel_blob->GetBufferPointer();
		nsh->pixelHeader->shaderSize = pixel_blob->GetBufferSize();

		auto wsh = new YYShader();
		wsh->HLSL11.vertexShader = nsh->vertexHeader->shaderData;
		wsh->HLSL11.fragmentShader = nsh->pixelHeader->shaderData;
		// If we don't set type to GLSLES, we have to do x-=1, y+=1, w=1 in vertex main()
		// (cheers to GoldenEpsilon for figuring this out)
		// This implies that there's some special handling depending on shader type, but how do
		// handwritten HLSL shaders _usually_ work in GM then?
		wsh->type = YYShaderType::GLSLES;

		auto csh = new CustomShader();
		csh->name = name;
		wsh->name = csh->name.c_str();
		csh->vertex.codeBlob = vertex_blob;
		csh->vertex.reflection = vertex_reflection;
		csh->pixel.codeBlob = pixel_blob;
		csh->pixel.reflection = pixel_reflection;
		csh->native = nsh;
		csh->wrapped = wsh;

		return csh;
	} while (false); // do-once
	#undef m_try_break

	// so that didn't work out
	if (error_blob) {
		shader_last_error += std::string(": ") + (const char*)error_blob->GetBufferPointer();
		error_blob->Release();
	}

	#define m_free_if_exists(_thing) if (_thing) _thing->Release();
	m_free_if_exists(vertex_shader);
	m_free_if_exists(vertex_blob);
	m_free_if_exists(vertex_reflection);

	m_free_if_exists(pixel_shader);
	m_free_if_exists(pixel_blob);
	m_free_if_exists(pixel_reflection);
	#undef m_free_if_exists

	return nullptr;
}