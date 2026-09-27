from PIL import Image
from pathlib import Path
im=Image.open('outputs/Resource-Panel/art/sakura-branch.png').convert('RGBA').resize((360,150),Image.Resampling.LANCZOS)
print('Alpha range',im.getchannel('A').getextrema())
Path('work/panel-test/main/branch.argb').write_bytes(im.tobytes('raw','BGRA'))
p=Path('work/panel-test/main/CMakeLists.txt');s=p.read_text();s+='\ntarget_add_binary_data(${COMPONENT_LIB} "${CMAKE_CURRENT_SOURCE_DIR}/branch.argb" BINARY)\n';p.write_text(s)
p=Path('work/panel-test/main/main.c');s=p.read_text().replace('*petals[18],*mist[3]','*petals[36],*mist[3],*ripples[10],*clouds[3],*branch')
s=s.replace('static int night_opacity;', '''static int night_opacity;
extern const uint8_t branch_data[] asm("_binary_branch_argb_start");
static const lv_image_dsc_t branch_art={.header={.magic=LV_IMAGE_HEADER_MAGIC,.cf=LV_COLOR_FORMAT_ARGB8888,.w=360,.h=150,.stride=1440},.data_size=216000,.data=branch_data};''')
s=s.replace('i<18','i<36').replace('i*31,530','i*19,530').replace('i*47+t*7','i*29+t*7')
s=s.replace('static void atmosphere(lv_timer_t *timer){','''static void atmosphere(lv_timer_t *timer){''')
s=s.replace('for(int i=0;i<36;i++){\n  float y=', '''lv_image_set_rotation(branch,(int)(8*sinf(t*.33f)));
 lv_image_set_pivot(branch,0,0);
 lv_obj_set_pos(branch,-8,-57+(int)(2*sinf(t*.27f)));
 lv_obj_set_style_image_recolor(branch,lv_color_hex(0x253c62),0);
 lv_obj_set_style_image_recolor_opa(branch,night_opacity*150/255,0);
 for(int i=0;i<10;i++){
  float phase=fmodf(t*.12f+i*.1f,1);
  lv_obj_set_pos(ripples[i],300+(i%4)*36+(int)(4*sinf(t*.4f+i)),337+(i/4)*12);
  lv_obj_set_width(ripples[i],14+(int)(phase*22));
  lv_obj_set_style_border_opa(ripples[i],(int)(sinf(phase*3.14159f)*(sleeping?18:50)),0);
 }
 for(int i=0;i<3;i++){
  lv_obj_set_pos(clouds[i],370+i*65+(int)(18*sinf(t*.04f+i)),105+i*48);
  lv_obj_set_style_bg_opa(clouds[i],sleeping?4:7,0);
 }
 for(int i=0;i<36;i++){
  float y=''')
needle=' for(int i=0;i<3;i++){\n  mist[i]=lv_obj_create(s);'
insert=''' branch=lv_image_create(s);lv_image_set_src(branch,&branch_art);lv_obj_remove_flag(branch,LV_OBJ_FLAG_CLICKABLE);
 for(int i=0;i<10;i++){
  ripples[i]=lv_obj_create(s);lv_obj_remove_style_all(ripples[i]);lv_obj_set_size(ripples[i],25,5);
  lv_obj_set_style_radius(ripples[i],LV_RADIUS_CIRCLE,0);lv_obj_set_style_border_width(ripples[i],1,0);
  lv_obj_set_style_border_color(ripples[i],lv_color_hex(0xbdd4d9),0);lv_obj_set_style_bg_opa(ripples[i],0,0);lv_obj_remove_flag(ripples[i],LV_OBJ_FLAG_CLICKABLE);
 }
 for(int i=0;i<3;i++){
  clouds[i]=lv_obj_create(s);lv_obj_remove_style_all(clouds[i]);lv_obj_set_size(clouds[i],170,22);
  lv_obj_set_style_radius(clouds[i],LV_RADIUS_CIRCLE,0);lv_obj_set_style_bg_color(clouds[i],lv_color_hex(0xb3bfce),0);lv_obj_remove_flag(clouds[i],LV_OBJ_FLAG_CLICKABLE);
 }
'''
s=s.replace(needle,insert+needle)
p.write_text(s)
