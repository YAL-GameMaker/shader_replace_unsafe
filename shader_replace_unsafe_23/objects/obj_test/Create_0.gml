show_message("");
var _init = shader_replace_unsafe_init(144);
if (_init <= 0) {
    trace("init failed, error: ", _init);
}
trace(shader_get_name(sh_first));
var vsh = scr_vsh();
var fsh = scr_fsh();
if (0) {
    var pair = scr_oldsh();
    vsh = pair[0];
    fsh = pair[1];
}
custom_shader = shader_add(vsh, fsh, "me");
trace("new shader:", custom_shader, shader_get_name(custom_shader));
trace("sampler-base", shader_get_sampler_index(sh_first, "gm_BaseTexture"))
trace("sampler-new", shader_get_sampler_index(custom_shader, "gm_BaseTexture"))