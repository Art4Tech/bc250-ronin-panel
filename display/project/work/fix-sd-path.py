from pathlib import Path
p=Path('work/panel-test/main/panel_video.c');s=p.read_text().replace('/sdcard/bc250-panel-v1','/sdcard/BC250').replace('day.rvid','day.rvj').replace('night.rvid','night.rvj').replace('snprintf(partial,sizeof(partial),"%s.part",destination);','snprintf(partial,sizeof(partial),ROOT"/%s",!strcmp(name,"day.rvj")?"day.tmp":"night.tmp");');p.write_text(s)
