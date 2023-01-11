function scr_dump_shader_uniforms(_sh) {
    static _kind_names = ["vertex", "pixel"];
    trace(sfmt("Uniform info for shader #% (%):",_sh, shader_get_name(_sh)))
    for (var k = 0; k < 2; k++) {
        for (var i = 0; i < 100; i++) {
            var u = shader_get_uniform_at(_sh, k, i);
            if (u < 0) break;
            var _name = shader_get_uniform_name(_sh, u);
            trace(sfmt("%[%]: % (id=%)", _kind_names[k],i,_name,u));
        }
    }
}