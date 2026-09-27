from pathlib import Path
from PIL import Image
import array,sys
for name in ('day','night'):
 p=Path(f'outputs/Resource-Panel/art/ronin-sakura-{name}.png')
 if not p.exists():continue
 im=Image.open(p).convert('RGB').resize((800,480),Image.Resampling.LANCZOS)
 data=array.array('H',(((r>>3)<<11)|((g>>2)<<5)|(b>>3) for r,g,b in im.getdata()))
 if sys.byteorder!='little':data.byteswap()
 Path(f'work/panel-test/main/{name}.rgb565').write_bytes(data.tobytes())
 print(name,len(data)*2)
