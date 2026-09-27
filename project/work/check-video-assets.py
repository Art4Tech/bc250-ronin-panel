import struct,hashlib,importlib.util
from pathlib import Path
from PIL import Image
import io
spec=importlib.util.spec_from_file_location('upload','outputs/Resource-Panel/upload_video.py');m=importlib.util.module_from_spec(spec);spec.loader.exec_module(m)
for name in ('day','night'):
 p=Path(f'outputs/Resource-Panel/video/{name}.rvj');raw=m.validate(p);pos=16;hashes=set();sizes=[]
 for i in range(60):
  n=struct.unpack_from('<I',raw,pos)[0];pos+=4;b=raw[pos:pos+n];pos+=n;hashes.add(hashlib.sha256(b).digest());im=Image.open(io.BytesIO(b));assert im.size==(800,480);im.load();sizes.append(n)
 print(name,'60 valid JPEG frames;',len(hashes),'distinct; average bytes',sum(sizes)//60)
