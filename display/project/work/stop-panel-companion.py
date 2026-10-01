import psutil
from pathlib import Path
expected=str(Path('outputs/Resource-Panel/resource_panel.py').resolve()).lower()
for pid in (9892,29664):
 try:
  p=psutil.Process(pid)
  if any(a.lower()==expected for a in p.cmdline()):
   p.terminate(); print('Stopped panel companion',pid)
 except psutil.NoSuchProcess:pass
