from pathlib import Path
import base64
root=Path('outputs/Resource-Panel')
s=(root/'video/preview.html').read_text()
for name in ('day','night'):
 encoded=base64.b64encode((root/f'art/ronin-sakura-{name}.png').read_bytes()).decode()
 s=s.replace(f'../art/ronin-sakura-{name}.png','data:image/png;base64,'+encoded)
s=s.replace('Full-screen moving-painting loop.','Browser preview; device playback speed is measured separately. Full-screen moving-painting loop.')
(root/'video/preview-standalone.html').write_text(s)
