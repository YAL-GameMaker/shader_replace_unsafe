function LiveShaderFile(_prefix) constructor {
    static __inc_file_dir = undefined;
    if (__inc_file_dir == undefined) {
        __inc_file_dir = filename_dir(GM_project_filename) + "/datafiles";
        if (!directory_exists(__inc_file_dir)) __inc_file_dir = "";
    }
    static __get_mtime = function(_path) /*=>*/ {
        var _full = game_save_id + "/" + _path;
        if (file_exists(_full)) return [_full, shader_replace_unsafe_get_mtime(_full)];
        if (__inc_file_dir == "") return ["", 0];
        _full = __inc_file_dir + "/" + _path;
        return [_full, shader_replace_unsafe_get_mtime(_full)];
    }
    static __get_text = function(_path) /*=>*/ {
        var _buf = buffer_load(_path);
        var _text = buffer_read(_buf, buffer_string);
        buffer_delete(_buf);
        return _text;
    }
    
    vsh_path = _prefix + ".vsh";
    psh_path = _prefix + ".psh";
    vsh_time = 0;
    psh_time = 0;
    static update = function() /*=>*/ {
        var _vsh_time_p = __get_mtime(vsh_path);
        var _psh_time_p = __get_mtime(psh_path);
        if (_vsh_time_p[1] != vsh_time || _psh_time_p[1] != psh_time) {
            vsh_time = _vsh_time_p[1];
            psh_time = _psh_time_p[1];
            return [__get_text(_vsh_time_p[0]), __get_text(_psh_time_p[0])];
        }
        return undefined;
    }
}