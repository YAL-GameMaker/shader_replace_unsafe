var pair = test_file.update();
if (pair != undefined) {
    var _t = current_time;
    //trace(pair);
    if (0) {
        var _sh = shader_add(pair[0], pair[1], "me");
        if (_sh != -1) {
            custom_shader = _sh;
        } else {
            trace("Shader creation failed: " + shader_get_last_error());
        }
    } else {
        if (shader_replace(custom_shader, pair[0], pair[1])) {
            trace("Replaced shader, took " + string(current_time - _t) + " ms.");
        } else {
            trace("Replace failed: " + shader_get_last_error());
        }
    }
}
