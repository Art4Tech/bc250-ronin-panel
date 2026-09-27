from pathlib import Path
p=Path('work/panel-test/peripheral/bsp_illuminate/bsp_illuminate.c')
s=p.read_text();a=s.index('#if CONFIG_DISPLAY_LVGL_FULL_REFRESH');b=s.index('        },',a)
s=s[:a]+'''            .full_refresh = true, // Redraw both complete buffers; avoid stale rectangles during transparent animation.
            .direct_mode = false,
'''+s[b:];p.write_text(s)
p=Path('work/panel-test/main/main.c');s=p.read_text()
s=s.replace('lv_obj_set_width(l,w);lv_label_set_text(l,text);','lv_obj_set_width(l,w);lv_label_set_text(l,text);lv_obj_set_style_text_color(l,lv_color_hex(0x182c25),0);')
s=s.replace('lv_label_set_text(l,text);lv_obj_center(l);return b;', 'lv_label_set_text(l,text);lv_obj_set_style_text_color(l,lv_color_hex(0xfff8e8),0);lv_obj_center(l);return b;')
s=s.replace('lv_color_hex(0x162923),0);lv_obj_set_style_bg_opa(panel,205','lv_color_hex(0xf4ead7),0);lv_obj_set_style_bg_opa(panel,245')
s=s.replace('set_lcd_blight(40)','set_lcd_blight(60)').replace('sleeping?8:40','sleeping?12:60')
s=s.replace('lv_obj_remove_flag(s,LV_OBJ_FLAG_SCROLLABLE);','lv_obj_remove_flag(s,LV_OBJ_FLAG_SCROLLABLE);lv_obj_set_style_bg_color(s,lv_color_hex(0x101c2b),0);')
s=s.replace('lv_obj_set_style_bg_opa(mist[i],(sleeping?8:14)+(int)(5*sinf(t*.4f+i)),0);','lv_obj_set_style_bg_opa(mist[i],sleeping?0:14+(int)(5*sinf(t*.4f+i)),0);')
p.write_text(s)
