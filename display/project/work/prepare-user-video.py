import io, struct, subprocess, shutil, json
from pathlib import Path
import imageio_ffmpeg
from PIL import Image
root=Path('outputs/Resource-Panel/video/user-clips')
ff=imageio_ffmpeg.get_ffmpeg_exe()
for name in ('day','night'):
 shutil.copy2(Path('external-originals/Downloads')/(name+'.mp4'),root/(name+'-original.mp4'))
# Preserve the whole 864px composition within the 800x480 panel, rather than crop the ronin.
segments=[('day2','day',0,.75),('nite2','night',0,1.5),('sleep2','day',.75,5.125),('wake2','night',1.5,5.125)]
for dest,source,start,end in segments:
 cmd=[ff,'-v','error','-ss',str(start),'-i',str(root/(source+'-original.mp4')),'-t',str(end-start),'-an','-vf','fps=12,scale=800:444,pad=800:480:0:18:color=0x10201d','-f','rawvideo','-pix_fmt','rgb24','pipe:1']
 raw=subprocess.run(cmd,stdout=subprocess.PIPE,check=True).stdout
 count=len(raw)//(800*480*3)
 with (root/(dest+'.rvj')).open('wb') as f:
  f.write(struct.pack('<4sHHHHI',b'RV01',800,480,12,0,count))
  for i in range(count):
   im=Image.frombytes('RGB',(800,480),raw[i*1152000:(i+1)*1152000]); b=io.BytesIO();im.save(b,format='JPEG',quality=60,subsampling=2)
   data=b.getvalue();f.write(struct.pack('<I',len(data)));f.write(data)
 print(dest,count,(root/(dest+'.rvj')).stat().st_size,flush=True)
# Crossfade the audio seam, no music track is used; modest initial playback level.
from array import array
raw_audio=subprocess.run([ff,'-v','error','-i',str(root/'day-original.mp4'),'-vn','-ac','2','-ar','16000','-f','s16le','pipe:1'],stdout=subprocess.PIPE,check=True).stdout
samples=array('h');samples.frombytes(raw_audio[:320000]);n=4800*2
blend=array('h',(round(.35*((1-(i//2)/4800)*samples[-n+i]+((i//2)/4800)*samples[i])) for i in range(n)))
blend.extend(round(.35*x) for x in samples[n:-n]);(root/'nature2.pcm').write_bytes(blend.tobytes())
(root/'segments.json').write_text(json.dumps({'fps':12,'segments':segments,'audio':'day-original.mp4','audio_gain':.35,'audio_loop_seconds':4.7},indent=2))
