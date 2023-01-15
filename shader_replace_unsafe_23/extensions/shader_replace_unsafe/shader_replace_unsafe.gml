#define shader_replace_unsafe_init
/// (?func_offset)->bool
var _func_offset = argument_count > 0 ? argument[0] : -1;

var _fself = { arr: [] };
var _fptr; with (_fself) {
    _fptr = function(_func) /*=>*/ {
        var _fn = method(undefined, _func);
        array_push(self.arr, _fn);
        return ptr(_fn);
    }
}
if (_func_offset < 0) {
    _func_offset = shader_replace_unsafe_init_offset(
        _fptr(shader_get_name), _fptr(shader_get_name)
    );
    if (_func_offset < 0) {
        show_debug_message("Could not auto-detect function implementation offset in CScriptRef* - please provide as an argument to shader_replace_unsafe_init.");
        return shader_replace_unsafe_init_error_wrong_func_offset;
    }
}

var _info = os_get_info();
if (!shader_replace_unsafe_init_1(
    _info[?"video_d3d11_device"],
    _info[?"video_d3d11_context"],
    _func_offset
)) {
    show_debug_message("shader_replace_unsafe DLL failed to load.");
    return shader_replace_unsafe_init_error_no_dll;
}

var _result;
_result = shader_replace_unsafe_init_2(_fptr(event_perform), _fptr(string_replace));
if (_result <= 0) return _result;

_result = shader_replace_unsafe_init_3(_fptr(shader_get_name));
if (_result <= 0) return _result;

_result = shader_replace_unsafe_init_4(_fptr(shader_get_uniform));
if (_result <= 0) return _result;

return shader_replace_unsafe_init_5(_fptr(shader_get_sampler_index));

#define shader_replace_unsafe_init_error_get_name
/// (shader_replace_unsafe_init_error)->string
switch (argument0) {
    case shader_replace_unsafe_init_error_success: return "success";
	case shader_replace_unsafe_init_error_no_dll: return "no_dll";
	case shader_replace_unsafe_init_error_wrong_func_offset: return "wrong_func_offset";
	case shader_replace_unsafe_init_error_no_shader_array: return "no_shader_array";
	case shader_replace_unsafe_init_error_no_native_shader_array: return "no_native_shader_array";
	case shader_replace_unsafe_init_error_instr_init_failed: return "instr_init_failed";
	default: return "error " + string(argument0);
}

#define shader_replace_unsafe_prepare_buffer
/// (size:int)->buffer~
var _size = argument0;
gml_pragma("global", "global.__shader_replace_unsafe_buffer = undefined");
var _buf = global.__shader_replace_unsafe_buffer;
if (_buf == undefined) {
    _buf = buffer_create(_size, buffer_grow, 1);
    global.__shader_replace_unsafe_buffer = _buf;
} else if (buffer_get_size(_buf) < _size) {
    buffer_resize(_buf, _size);
}
buffer_seek(_buf, buffer_seek_start, 0);
return _buf;