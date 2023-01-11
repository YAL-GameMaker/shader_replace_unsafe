#define shader_replace_unsafe_cleanup
/// shader_replace_unsafe_cleanup()
var _buf = shader_replace_unsafe_prepare_buffer(1);
shader_replace_unsafe_cleanup_raw(buffer_get_address(_buf), ptr(1));

#define shader_get_hlsl_model
/// shader_get_hlsl_model()->string
var _buf = shader_replace_unsafe_prepare_buffer(1);
return shader_get_hlsl_model_raw(buffer_get_address(_buf), ptr(1));

#define shader_set_hlsl_model
/// shader_set_hlsl_model(suffix:string)->bool
var _buf = shader_replace_unsafe_prepare_buffer(8);
return shader_set_hlsl_model_raw(buffer_get_address(_buf), ptr(8), argument0);

#define shader_get_last_error
/// shader_get_last_error()->string @dllg:defValue "DLL is not loaded."
var _buf = shader_replace_unsafe_prepare_buffer(8);
var __size__ = shader_get_last_error_raw(buffer_get_address(_buf), ptr(8));
if (__size__ == 0) return "DLL is not loaded.";
if (buffer_get_size(_buf) < __size__) buffer_resize(_buf, __size__);
shader_get_last_error_raw_post(buffer_get_address(_buf), __size__);
buffer_seek(_buf, buffer_seek_start, 0);

return buffer_read(_buf, buffer_string);

#define shader_get_path
/// shader_get_path()->string
var _buf = shader_replace_unsafe_prepare_buffer(1);
return shader_get_path_raw(buffer_get_address(_buf), ptr(1));

#define shader_set_path
/// shader_set_path(path:string)->bool
var _buf = shader_replace_unsafe_prepare_buffer(8);
return shader_set_path_raw(buffer_get_address(_buf), ptr(8), argument0);

#define shader_add
/// shader_add(vertex_code:string, fragment_code:string, name:string = "shader!")->int @dllg:defValue -1
var _buf = shader_replace_unsafe_prepare_buffer(25);
if (argument_count >= 3) {
	buffer_write(_buf, buffer_bool, true);
	buffer_write(_buf, buffer_string, argument[2]);
} else buffer_write(_buf, buffer_bool, false);
if (shader_add_raw(buffer_get_address(_buf), ptr(25), argument[0], argument[1])) {
	buffer_seek(_buf, buffer_seek_start, 0);
	return buffer_read(_buf, buffer_s32);
} else return -1;

