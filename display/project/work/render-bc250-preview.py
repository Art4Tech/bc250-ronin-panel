"""Render the existing dashboard UI deterministically from native LVGL glyphs.
No artwork is repainted: the background JPEG is extracted intact from day3.rvj.
"""
from pathlib import Path
import re,struct,base64,json,html
from playwright.sync_api import sync_playwright
ROOT=Path(__file__).resolve().parent.parent
OUT=ROOT/'outputs/Resource-Panel/simulations';OUT.mkdir(exist_ok=True)
FONT=ROOT/'work/panel-test/managed_components/lvgl__lvgl/src/font'
class Font:
 def __init__(self,size):
  t=(FONT/f'lv_font_montserrat_{size}.c').read_text(encoding='utf-8')
  t=re.sub(r'/\*.*?\*/','',t,flags=re.S)
  def nums(name):
   a=re.search(r'\b'+name+r'\[\]\s*=\s*\{(.*?)\};',t,re.S).group(1)
   return [int(x,0) for x in re.findall(r'-?0x[0-9a-fA-F]+|-?\d+',a)]
  self.bits=nums('glyph_bitmap');self.left=nums('kern_left_class_mapping');self.right=nums('kern_right_class_mapping');self.kern=nums('kern_class_values')
  a=re.search(r'\bglyph_dsc\[\]\s*=\s*\{(.*?)\};',t,re.S).group(1)
  self.glyphs=[{k:int(v) for k,v in re.findall(r'\.(\w+)\s*=\s*(-?\d+)',g)} for g in re.findall(r'\{([^{}]*)\}',a)]
  self.extra={176+n:96+i for i,n in enumerate(nums('unicode_list_1'))}
  self.height=int(re.search(r'\.line_height\s*=\s*(\d+)',t).group(1));self.base=int(re.search(r'\.base_line\s*=\s*(\d+)',t).group(1));self.rc=int(re.search(r'\.right_class_cnt\s*=\s*(\d+)',t).group(1))
 def gid(self,c):return ord(c)-31 if 32<=ord(c)<=126 else self.extra.get(ord(c),0)
 def advance(self,g,nextg):
  k=0;l=self.left[g];r=self.right[nextg]
  if l and r:k=self.kern[(l-1)*self.rc+r-1]
  return (self.glyphs[g]['adv_w']+k+8)//16
 def width(self,text):
  ids=[self.gid(c) for c in text]+[0]
  return sum(self.advance(g,ids[i+1]) for i,g in enumerate(ids[:-1]))
 def draw(self,text,x,y,color):
  paths={a:[] for a in range(1,16)}
  for line in text.split('\n'):
   cx=x;ids=[self.gid(c) for c in line]+[0]
   for i,gid in enumerate(ids[:-1]):
    g=self.glyphs[gid];w=g['box_w'];h=g['box_h'];pos=g['bitmap_index']*2
    for yy in range(h):
     for xx in range(w):
      b=self.bits[(pos+yy*w+xx)//2];v=(b>>4) if (pos+yy*w+xx)%2==0 else (b&15)
      if v:paths[v].append(f'M{cx+g["ofs_x"]+xx},{y+self.height-self.base-h-g["ofs_y"]+yy}h1v1h-1z')
    cx+=self.advance(gid,ids[i+1])
   y+=self.height
  return ''.join(f'<path fill="{color}" fill-opacity="{a/15:.5f}" d="{"".join(p)}"/>' for a,p in paths.items() if p)
fonts={s:Font(s) for s in (14,24)}
media=ROOT/'outputs/Resource-Panel/video/movies-v3/day3.rvj'
with media.open('rb') as f:
 header=f.read(16);magic,w,h,fps,reserved,count=struct.unpack('<4sHHHHI',header);assert (magic,w,h)==(b'RV01',800,480)
 for i in range(21):
  n=struct.unpack('<I',f.read(4))[0];frame=f.read(n)
(OUT/'day-frame-2s.jpg').write_bytes(frame)
uri='data:image/jpeg;base64,'+base64.b64encode(frame).decode()
pieces=[f'<image width="800" height="480" href="{uri}"/>', '<rect x="476.5" y="16.5" width="307" height="447" rx="16" fill="#3a5048" fill-opacity="0.921569" stroke="#afa88d" stroke-opacity="0.352941"/>']
def txt(t,x,y,size=24,color='#fff3da'):pieces.append(fonts[size].draw(t,x,y,color))
def button(label,x,y,width):
 pieces.append(f'<rect x="{x}" y="{y}" width="{width}" height="38" rx="8" fill="#374943" fill-opacity="0.784314"/>')
 txt(label,x+(width-fonts[14].width(label))//2,y+11,14,'#fff8e8')
txt('MOUNTAIN REFUGE',491,31,24,'#ffc7d5')
txt('bc250-omarchy | Linux',491,63,14)
txt('CPU LOAD\n8.6 %',491,95)
txt('CPU TEMP\n47.0 C',636,95)
txt('RAM\n41.6 %\n3.2 / 7.7 GiB',491,179)
txt('SYSTEM DISK\n6.8 % used',491,276)
txt('USB connected - live readings',491,344,14)
button('SLEEP DISPLAY',491,377,133);button('MUTE / SOUND',634,377,135)
txt('Nature sound playing',491,424,14)
button('\uf013 SETTINGS',16,16,125)
content=''.join(pieces)
native=f'<svg xmlns="http://www.w3.org/2000/svg" width="800" height="480" viewBox="0 0 800 480">{content}</svg>'
(OUT/'bc250-day-native.svg').write_text(native)
# Exactly 800x480 display pixels inside an integer-aligned black bezel for PNG.
framed=f'<svg xmlns="http://www.w3.org/2000/svg" width="894" height="565" viewBox="0 0 894 565"><rect width="894" height="565" fill="#090909"/><g transform="translate(47 43)">{content}</g></svg>'
(OUT/'bc250-day-bezel.svg').write_text(framed)
# Physical-size vector wrapper keeps the requested outer dimensions exact.
physical=f'<svg xmlns="http://www.w3.org/2000/svg" width="120.7mm" height="76.3mm" viewBox="0 0 120.7 76.3"><rect width="120.7" height="76.3" fill="#090909"/><g transform="translate(6.35 5.75) scale(0.135)">{content}</g></svg>'
(OUT/'bc250-day-actual-size.svg').write_text(physical)
page='''<!doctype html><html><meta charset="utf-8"><title>BC250 day scene — actual size</title><style>body{background:#eee;color:#243d35;font:16px system-ui;margin:30px}.device{width:120.7mm;height:76.3mm;display:block}p{max-width:720px;line-height:1.5}@media print{@page{size:A4;margin:15mm}body{margin:0;background:white}.instructions{display:none}}</style><div class="instructions"><h1>BC250 day scene • simulated readings</h1><p>The display image is 800 × 480 pixels. The complete black front is sized to your requested 120.7 × 76.3 mm; the centered active area is modeled as 108 × 64.8 mm. This is a visual sizing mockup, not a verified fabrication drawing.</p><p>Print at <b>100% / Actual size</b>, with headers/footers off and no Fit to page. On-screen size depends on your monitor and browser zoom. Check the 50 mm ruler below.</p></div>'''+physical+'''<svg xmlns="http://www.w3.org/2000/svg" width="60mm" height="12mm" viewBox="0 0 60 12"><path d="M5 3V7M5 5H55M55 3V7" fill="none" stroke="black" stroke-width=".2"/><text x="30" y="11" text-anchor="middle" font-family="sans-serif" font-size="3">50 mm</text></svg><p class="instructions">Example only: 8.6% CPU, 47°C CPU, 3.2/7.7 GiB RAM (41.6%), 6.8% disk used. RAM assumes roughly 8 GiB allocated to the OS; firmware allocation, workload and cooling change the actual readings. The artwork is an unchanged frame from your current day video, and the labels use the firmware's Montserrat bitmap glyphs. No device configuration or firmware was changed.</p></html>'''
(OUT/'bc250-day-actual-size.html').write_text(page,encoding='utf-8')
with sync_playwright() as p:
 browser=p.chromium.launch(executable_path='C:/Program Files (x86)/Microsoft/Edge/Application/msedge.exe',headless=True)
 tab=browser.new_page(device_scale_factor=1)
 for name,width,height in [('bc250-day-native',800,480),('bc250-day-bezel',894,565)]:
  tab.set_viewport_size({'width':width,'height':height})
  tab.goto((OUT/(name+'.svg')).as_uri());tab.screenshot(path=str(OUT/(name+'.png')))
 browser.close()
metadata={'simulated':True,'panel_resolution':[800,480],'outer_mm':[120.7,76.3],'assumed_active_mm':[108,64.8],'values':{'cpu_percent':8.6,'cpu_c':47,'ram_percent':41.6,'ram_used_gib':3.2,'ram_total_gib':7.7,'disk_percent':6.8},'background':'day3.rvj frame 20 at 10fps','fonts':'LVGL Montserrat Medium native glyph bitmaps 14px/24px','geometry_note':'Outer size supplied by user; centered active area inferred from Elecrow rounded 108x65mm specification and 800:480 pixel aspect. Not a fabrication drawing.'}
(OUT/'preview-details.json').write_text(json.dumps(metadata,indent=2))
print('Rendered native 800x480 and bezel 894x565 PNGs, plus exact-size SVG/HTML.',flush=True)
