var pair = test_file.update();
if (pair != undefined) {
    var _t = current_time;
    //trace(pair);
    if (1) {
        var _sh = shader_add(pair[0], pair[1], "me");
        if (_sh != -1) {
            shader_destroy(custom_shader);
            trace(sfmt("Destroyed shader %, created shader % (%ms)",custom_shader,_sh, current_time - _t))
            custom_shader = _sh;
        } else {
            trace("Shader creation failed: " + shader_get_last_error());
        }
    } else {
        if (shader_replace(custom_shader, pair[0], pair[1])) {
            trace(sfmt("Replaced shader % (%ms)",custom_shader, current_time - _t));
        } else {
            trace("Replace failed: " + shader_get_last_error());
        }
    }
}
