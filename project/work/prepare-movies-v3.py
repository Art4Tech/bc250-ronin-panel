import subprocess,io,struct,shutil,json
from pathlib import Path
import imageio_ffmpeg
from PIL import Image,ImageDraw
root=Path('outputs/Resource-Panel/video/movies-v3');root.mkdir(exist_ok=True)
ff=imageio_ffmpeg.get_ffmpeg_exe();report=[]
for name,dest in [('dayidle','day3'),('nightidle','nite3'),('sleepytime','sleep3'),('wakeuptime','wake3')]:
 source=Path('external-originals/Downloads')/(name+'.mp4');shutil.copy2(source,root/source.name)
 limit=['-t','5.2'] if name in ('sleepytime','wakeuptime') else []
 process=subprocess.Popen([ff,'-v','error','-i',str(source),*limit,'-an','-vf','fps=10,scale=800:450,pad=800:480:0:15:color=0x10201d','-pix_fmt','rgb24','-f','rawvideo','pipe:1'],stdout=subprocess.PIPE)
 count=0;thumbs=[]
 with (root/(dest+'.rvj')).open('wb') as out:
  out.write(b'\0'*16)
  while True:
   raw=process.stdout.read(800*480*3)
   if not raw:break
   if len(raw)!=800*480*3:raise RuntimeError('Incomplete frame')
   im=Image.frombytes('RGB',(800,480),raw);jpeg=io.BytesIO();im.save(jpeg,format='JPEG',quality=28,subsampling=2)
   frame=jpeg.getvalue();out.write(struct.pack('<I',len(frame)));out.write(frame)
   if count%20==0:thumbs.append((count/10,im.resize((320,192))))
   count+=1
  if process.wait()!=0:raise RuntimeError('Video conversion failed')
  out.seek(0);out.write(struct.pack('<4sHHHHI',b'RV01',800,480,10,0,count))
 sheet=Image.new('RGB',(960,216*((len(thumbs)+2)//3)), '#10201d');draw=ImageDraw.Draw(sheet)
 for i,(t,im) in enumerate(thumbs):x=(i%3)*320;y=(i//3)*216;sheet.paste(im,(x,y));draw.text((x+8,y+195),f'{name}: {t:.1f}s',fill='white')
 sheet.save(root/(name+'-contact.jpg'))
 audio_filter='volume=0.35'
 if name in ('dayidle','nightidle'):audio_filter+=f',apad,atrim=duration={count/10}'
 subprocess.run([ff,'-v','error','-i',str(source),'-vn','-af',audio_filter,'-ac','2','-ar','16000','-f','s16le','-y',str(root/(dest+'.pcm'))],check=True)
 row={'source':source.name,'asset':dest+'.rvj','frames':count,'fps':10,'bytes':(root/(dest+'.rvj')).stat().st_size,'audio_bytes':(root/(dest+'.pcm')).stat().st_size};report.append(row);print(row,flush=True)
(root/'manifest.json').write_text(json.dumps(report,indent=2))
