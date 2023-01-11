#define shader_replace_unsafe_init
/// (func_offset)->bool
var _func_offset = argument_count > 0 ? argument[0] : 144;

var _info = os_get_info();
if (!shader_replace_unsafe_init_1(
    _info[?"video_d3d11_device"],
    _info[?"video_d3d11_context"],
    _func_offset
)) {
    show_debug_message("shader_replace_unsafe DLL failed to load.");
    return shader_replace_unsafe_init_error_no_dll;
}

var _fself = { arr: [] };
var _fptr; with (_fself) {
    _fptr = function(_func) /*=>*/ {
        var _fn = method(undefined, _func);
        array_push(self.arr, _fn);
        return ptr(_fn);
    }
}

var _result;
_result = shader_replace_unsafe_init_2(_fptr(event_perform), _fptr(string_replace));
if (_result <= 0) return _result;

_result = shader_replace_unsafe_init_3(_fptr(shader_get_name));
if (_result <= 0) return _result;

_result = shader_replace_unsafe_init_4(_fptr(shader_get_uniform));
if (_result <= 0) return _result;

return shader_replace_unsafe_init_5(_fptr(shader_get_sampler_index));

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