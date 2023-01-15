/// @author YellowAfterlife

#include "stdafx.h"
#include "shared.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")
#pragma comment(lib, "dxguid.lib")
#include <d3dcompiler.h>
#include <queue>

std::vector<CustomShader*> customShaders{};

std::string shader_model = "5_0";
dllg const char* shader_get_hlsl_model() {
	return shader_model.c_str();
}
dllg bool shader_set_hlsl_model(const char* suffix) {
	shader_model = suffix;
	return true;
}

std::string shader_last_error = "DLL did not correctly initialize.";
// @dllg:defValue "DLL is not loaded."
dllg const char* shader_get_last_error() {
	return shader_last_error.c_str();
}

std::string shader_path = "shader";
dllg const char* shader_get_path() {
	return shader_path.c_str();
}
dllg bool shader_set_path(const char* path) {
	shader_path = path;
	return true;
}

// Leaving a hole in shader array is Extremely Dangerous so we won't
// destroy shaders until we have a new shader to replace them with
std::queue<int> reuseShaderIDs{};

void shader_sync_uniforms(YYShader* shader);
CustomShader* shader_add_impl(const char* vertex_code, const char* fragment_code, std::string name);

// @dllg:defValue -1
dllg shader_id shader_add(const char* vertex_code, const char* fragment_code, const char* name = "shader!") {
	trouble_check(-1);
	auto csh = shader_add_impl(vertex_code, fragment_code, name);
	if (csh == nullptr) return -1;

	int natID, id;
	if (reuseShaderIDs.empty()) {
		natID = gmlShaders.native.add(csh->native);
		id = gmlShaders.wrappers.add(csh->wrapped);
	} else {
		id = reuseShaderIDs.front();
		reuseShaderIDs.pop();
		auto old = customShaders[id];
		natID = old->wrapped->shaderHandle;
		gmlShaders.wrappers.set(id, csh->wrapped);
		gmlShaders.native.set(natID, csh->native);
		old->release();
		delete old;
	}

	csh->wrapped->shaderHandle = natID;
	csh->wrapped->id = id;
	shader_sync_uniforms(csh->wrapped);
	setCustomShader(id, csh);

	return id;
}

template<typename T> inline T shader_index_error(shader_id id, T result) {
	shader_last_error = std::string("Shader index out of range (")
		+ std::to_string(gmlShaders.wrappers.size()) + " total, attempted ID "
		+ std::to_string(id) + ")";
	return result;
}

dllg bool shader_replace(shader_id id, const char* vertex_code, const char* fragment_code) {
	trouble_check(false);
	auto orig = gmlShaders.wrappers.get(id);
	if (orig == nullptr) return shader_index_error(id, false);

	auto csh = shader_add_impl(vertex_code, fragment_code, orig->name);
	if (csh == nullptr) return false;
	auto natID = orig->shaderHandle;

	auto oldCustomShader = getCustomShader(id);
	if (oldCustomShader) {
		oldCustomShader->release();
		delete oldCustomShader;
	}

	gmlShaders.wrappers.set(id, csh->wrapped);
	gmlShaders.native.set(natID, csh->native);
	csh->wrapped->shaderHandle = natID;
	csh->wrapped->id = id;
	shader_sync_uniforms(csh->wrapped);
	setCustomShader(id, csh);

	return true;
}

dllg bool shader_destroy(shader_id id) {
	trouble_check(false);
	if (id < gmlOriginal.wrappers.count) {
		shader_last_error = std::string("You cannot destroy bundled shaders (indexes up to ")
			+ std::to_string(gmlShaders.wrappers.size()) + " non-inclusive, attempted ID "
			+ std::to_string(id) + ")";
		return false;
	}
	auto csh = getCustomShader(id);
	if (csh == nullptr) return shader_index_error(id, false);
	if (csh->isDestroyed) {
		shader_last_error = std::string("This shader is already destroyed (attempted ID ")
			+ std::to_string(id) + ")";
		return false;
	}
	csh->isDestroyed = true;
	reuseShaderIDs.push(id);
	return true;
}

// @dllg:defValue -1
dllg shader_uniform_id shader_get_uniform_at(shader_id id, int uniform_type, int uniform_index) {
	trouble_check(-1);
	auto wsh = gmlShaders.wrappers.get(id);
	if (wsh == nullptr) return shader_index_error(id, -1);
	if (uniform_index < 0) return -1;
	auto nsh = gmlShaders.native.get(wsh->shaderHandle);
	auto varCount = nsh->constBufVarCount;
	auto varArray = nsh->constBufVars;
	auto bufCount = nsh->constBufferCount;
	auto bufArray = nsh->constBuffers;
	auto left = uniform_index;
	for (auto i = 0; i < varCount; i++) {
		auto& var = varArray[i];

		auto bufIndex = var.bufferIndex;
		if (bufIndex < 0 || bufIndex > bufCount) continue;

		auto buf = bufArray[bufIndex];
		if (buf.type != (YYShaderKind)uniform_type) continue;

		if (--left < 0) return i;
	}
	return -1;
}

dllg const char* shader_get_uniform_name(shader_id id, shader_uniform_id uniform_index) {
	trouble_check("");
	auto wsh = gmlShaders.wrappers.get(id);
	if (wsh == nullptr) return shader_index_error(id, "");
	auto nsh = gmlShaders.native.get(wsh->shaderHandle);
	if (uniform_index < 0 || uniform_index >= nsh->constBufVarCount) {
		shader_last_error = std::string("Shader uniform index out of range (")
			+ std::to_string(nsh->constBufVarCount) + " total, attempted ID "
			+ std::to_string(uniform_index) + ")";
		return "";
	}
	return nsh->constBufVars[uniform_index].name;
}

dllg bool shader_set_name(shader_id id, const char* newname) {
	trouble_check(false);
	auto csh = getCustomShader(id);
	if (csh == nullptr) {
		if (gmlShaders.wrappers.get(id) == nullptr) return shader_index_error(id, false);
		shader_last_error = std::string("Can only change names of custom shaders (attempted")
			+ std::to_string(id) + ")";
		return false;
	}
	csh->name = newname;
	csh->wrapped->name = csh->name.c_str();
	return true;
}

dllg int shader_get_kind(shader_id id) {
	trouble_check(0);
	auto wsh = gmlShaders.wrappers.get(id);
	if (wsh == nullptr) return shader_index_error(id, 0);
	return (int)wsh->kind;
}

dllg bool shader_set_kind(shader_id id, int kind) {
	trouble_check(false);
	auto wsh = gmlShaders.wrappers.get(id);
	if (wsh == nullptr) return shader_index_error(id, false);
	wsh->kind = (shader_kind)kind;
	return true;
}


// turns out that you don't REALLY have to use YYAlloc -
// it's not like the runtime's going to touch shader arrays... until it shuts down
/*__declspec(dllexport) void YYExtensionInitialise(const struct YYRunnerInterface* _struct, size_t _size) {
	memcpy(&yyRunnerInterface, _struct, sizeof(YYRunnerInterface));
	//trace("YYExtensionInitialise");
}*/
