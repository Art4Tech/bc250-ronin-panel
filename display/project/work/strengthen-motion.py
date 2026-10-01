from pathlib import Path
import base64

root = Path('outputs/Resource-Panel')
source = (root / 'video/preview.html').read_text(encoding='utf-8')
source = source.replace('water*.0024', 'water*.010').replace('water*.0011', 'water*.004')
source = source.replace('cloud*.004', 'cloud*.018').replace('cloud*.001', 'cloud*.004')
source = source.replace('canopy*.0028', 'canopy*.013').replace('canopy*.0015', 'canopy*.006')
source = source.replace('water*.008', 'water*.025')
source = source.replace('for(int i=0;i<26;i++)', '''// Rising steam stays within the onsen, away from the figure and dashboard.
for(int s=0;s<4;s++){
 float f=float(s);float rise=fract(phase+f*.25);
 vec2 center=vec2(.44+f*.055+.018*sin(a+f),.83-rise*.19);
 float steam=bell(uv,center,vec2(.045+rise*.025,.022+rise*.04));
 color=mix(color,mix(vec3(.90,.90,.84),vec3(.48,.57,.67),night),steam*sin(rise*3.1415926)*.16);
}
for(int i=0;i<42;i++)''')
source = source.replace('(t%4)/4', '(t%6)/6')
source = source.replace('Full-screen moving-painting loop.', 'Stronger environmental motion preview: water, rising steam, clouds, branches and petals.')
(root / 'video/preview-motion-v2.html').write_text(source, encoding='utf-8')
for name in ('day', 'night'):
 data = base64.b64encode((root / f'art/ronin-sakura-{name}.png').read_bytes()).decode()
 source = source.replace(f'../art/ronin-sakura-{name}.png', 'data:image/png;base64,' + data)
(root / 'video/preview-motion-v2-standalone.html').write_text(source, encoding='utf-8')
print('Created stronger-motion preview; installed clips unchanged.')
