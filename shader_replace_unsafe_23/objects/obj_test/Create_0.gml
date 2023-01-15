//show_message("");
var _init = shader_replace_unsafe_init();
if (_init <= 0) {
    trace("init failed, error: " + shader_replace_unsafe_init_error_get_name(_init));
}
scr_dump_shader_uniforms(sh_first);
var vsh = scr_vsh();
var fsh = scr_fsh();
custom_shader = shader_add(vsh, fsh, "me");
if (custom_shader < 0) {
    trace("Shader creation failed: " + shader_get_last_error());
} else {
    scr_dump_shader_uniforms(custom_shader);
    trace("new shader:", custom_shader, shader_get_name(custom_shader));
}
test_file = new LiveShaderFile("test");