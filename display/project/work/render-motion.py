import base64,struct,threading,http.server,functools
from pathlib import Path
from playwright.sync_api import sync_playwright
root=Path.cwd()
class Quiet(http.server.SimpleHTTPRequestHandler):
 def log_message(self,*args):pass
server=http.server.ThreadingHTTPServer(('127.0.0.1',0),functools.partial(Quiet,directory=str(root)))
threading.Thread(target=server.serve_forever,daemon=True).start()
try:
 with sync_playwright() as p:
  browser=p.chromium.launch(executable_path='C:/Program Files (x86)/Microsoft/Edge/Application/msedge.exe',headless=True,args=['--use-angle=swiftshader','--enable-unsafe-swiftshader'])
  page=browser.new_page(viewport={'width':1000,'height':700});page.add_init_script('window.__exportMode=true;')
  page.goto(f'http://127.0.0.1:{server.server_port}/outputs/Resource-Panel/video/preview.html');page.evaluate('window.ready')
  for night,name in ((False,'day'),(True,'night')):
   output=root/f'outputs/Resource-Panel/video/{name}.rvj'
   with output.open('wb') as f:
    f.write(struct.pack('<4sHHHHI',b'RV01',800,480,15,0,60))
    for i in range(60):
     data=page.evaluate('([t,n])=>window.renderFrame(t,n)',[i/15,night]);jpeg=base64.b64decode(data.split(',')[1]);f.write(struct.pack('<I',len(jpeg)));f.write(jpeg)
     if i==0:(output.parent/f'{name}-preview.jpg').write_bytes(jpeg)
   print(name,output.stat().st_size,'bytes',flush=True)
  browser.close()
finally:server.shutdown()
