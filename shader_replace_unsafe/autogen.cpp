#include "gml_ext.h"
#include "gml_internals.h"
extern void shader_replace_unsafe_cleanup();
dllx double shader_replace_unsafe_cleanup_raw(void* _in_ptr, void* _in_ptr_size) {
	gml_istream _in(_in_ptr);
	shader_replace_unsafe_cleanup();
	return 1;
}

extern const char* shader_get_hlsl_model();
dllx const char* shader_get_hlsl_model_raw(void* _in_ptr, void* _in_ptr_size) {
	gml_istream _in(_in_ptr);
	return shader_get_hlsl_model();
}

extern bool shader_set_hlsl_model(const char* suffix);
dllx double shader_set_hlsl_model_raw(void* _in_ptr, void* _in_ptr_size, const char* _arg_suffix) {
	gml_istream _in(_in_ptr);
	return shader_set_hlsl_model(_arg_suffix);
}

extern const char* shader_get_last_error();
static const char* shader_get_last_error_raw_vec;
dllx double shader_get_last_error_raw(void* _in_ptr, void* _in_ptr_size) {
	gml_istream _in(_in_ptr);
	shader_get_last_error_raw_vec = shader_get_last_error();
	return (double)(1 + strlen(shader_get_last_error_raw_vec));
}
dllx double shader_get_last_error_raw_post(void* _out_ptr, double _out_ptr_size) {
	gml_ostream _out(_out_ptr);
	_out.write_string(shader_get_last_error_raw_vec);
	return 1;
}

extern const char* shader_get_path();
dllx const char* shader_get_path_raw(void* _in_ptr, void* _in_ptr_size) {
	gml_istream _in(_in_ptr);
	return shader_get_path();
}

extern bool shader_set_path(const char* path);
dllx double shader_set_path_raw(void* _in_ptr, void* _in_ptr_size, const char* _arg_path) {
	gml_istream _in(_in_ptr);
	return shader_set_path(_arg_path);
}

extern int shader_add(const char* vertex_code, const char* fragment_code, const char* name);
dllx double shader_add_raw(void* _inout_ptr, void* _inout_ptr_size, const char* _arg_vertex_code, const char* _arg_fragment_code) {
	gml_istream _in(_inout_ptr);
	const char* _arg_name;
	if (_in.read<bool>()) {
		_arg_name = _in.read_string();
	} else _arg_name = "shader!";
	int _ret = shader_add(_arg_vertex_code, _arg_fragment_code, _arg_name);
	gml_ostream _out(_inout_ptr);
	_out.write<int>(_ret);
	return 1;
}

