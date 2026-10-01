from pathlib import Path
from playwright.sync_api import sync_playwright
with sync_playwright() as p:
 b=p.chromium.launch(executable_path='C:/Program Files (x86)/Microsoft/Edge/Application/msedge.exe',headless=True,args=['--use-angle=swiftshader','--enable-unsafe-swiftshader'])
 page=b.new_page();page.add_init_script('window.__exportMode=true;');page.goto(Path('outputs/Resource-Panel/video/preview-standalone.html').resolve().as_uri());page.evaluate('window.ready');data=page.evaluate('window.renderFrame(1,false)');assert data.startswith('data:image/jpeg;base64,');print('Standalone preview renders successfully from a local file.');b.close()
