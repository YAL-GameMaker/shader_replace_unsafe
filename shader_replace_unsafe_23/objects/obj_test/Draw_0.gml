draw_set_font(fnt_test);
draw_set_color(c_white);

var _custom = true; // mouse_check_button(mb_left);
shader_set(_custom ? custom_shader : sh_first);
draw_text(mouse_x, mouse_y, "wow");
shader_reset();