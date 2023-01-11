/// @author YellowAfterlife

#include "stdafx.h"
#include "shared.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")
#pragma comment(lib, "dxguid.lib")
#include <d3dcompiler.h>

std::vector<CustomShader*> customShaders{};

std::string shader_model = "5_0";
dllg const char* shader_get_hlsl_model() {
	return shader_model.c_str();
}
dllg bool shader_set_hlsl_model(const char* suffix) {
	shader_model = suffix;
	return true;
}

std::string shader_last_error = "";
/// @dllg:defValue "DLL is not loaded."
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

void shader_sync_uniforms(YYShader* shader);
CustomShader* shader_add_impl(const char* vertex_code, const char* fragment_code, std::string name);
/// @dllg:defValue -1
dllg int shader_add(const char* vertex_code, const char* fragment_code, const char* name = "shader!") {
	trouble_check(-1);
	auto csh = shader_add_impl(vertex_code, fragment_code, name);
	if (csh == nullptr) return -1;

	auto natID = gmlShaders.native.add(csh->native);
	auto id = gmlShaders.wrappers.add(csh->wrapped);
	csh->wrapped->shaderHandle = natID;
	csh->wrapped->id = id;
	shader_sync_uniforms(csh->wrapped);
	setCustomShader(id, csh);

	return id;
}

dllg bool shader_replace(int id, const char* vertex_code, const char* fragment_code) {
	trouble_check(false);
	if (id < 0 || id >= gmlShaders.wrappers.size()) {
		shader_last_error = std::string("Shader index out of range (")
			+ std::to_string(gmlShaders.wrappers.size()) + " total, requested "
			+ std::to_string(id) + ")";
		return false;
	}

	auto orig = gmlShaders.wrappers.get(id);

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

// turns out that you don't REALLY have to use YYAlloc -
// it's not like the runtime's going to touch shader arrays... until it shuts down
/*__declspec(dllexport) void YYExtensionInitialise(const struct YYRunnerInterface* _struct, size_t _size) {
	memcpy(&yyRunnerInterface, _struct, sizeof(YYRunnerInterface));
	//trace("YYExtensionInitialise");
}*/
