from pathlib import Path
p=Path('work/panel-test/main/main.c');s=p.read_text().replace('0x182c25','0xfff3da').replace('0xf4ead7','0x3a5048').replace('bg_opa(panel,245','bg_opa(panel,235')
s=s.replace('text_at(panel,0,0,270,"MOUNTAIN REFUGE",true);','lv_obj_t *title=text_at(panel,0,0,270,"MOUNTAIN REFUGE",true);lv_obj_set_style_text_color(title,lv_color_hex(0xffc7d5),0);')
p.write_text(s)
