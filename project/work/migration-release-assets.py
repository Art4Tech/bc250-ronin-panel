"""Create reusable public dependency archives; never include authentication state."""
from pathlib import Path
import zipfile,hashlib,json,os,concurrent.futures
ROOT=Path(__file__).resolve().parent.parent
DEST=ROOT/'migration/release-assets';DEST.mkdir(exist_ok=True)
groups={
 'esp-idf-5.5.4.zip':[(ROOT/'work/esp-idf','project/work/esp-idf')],
 'elecrow-vendor-source.zip':[(ROOT/'work/elecrow-official','project/work/elecrow-official')],
 'windows-python-environments.zip':[(ROOT/'work/python','project/work/python'),(ROOT/'work/panel-tools','project/work/panel-tools'),(ROOT/'work/idf-tools/python_env','project/work/idf-tools/python_env'),(Path('C:/Python314'),'dependencies/Python314')],
 'git-windows-2.52.0.zip':[(Path('C:/Program Files/Git'),'dependencies/Git')],
}
for p in (ROOT/'work/idf-tools/tools').iterdir():
    groups['idf-tool-'+p.name+'.zip']=[(p,'project/work/idf-tools/tools/'+p.name)]
groups['idf-tool-metadata.zip']=[(p,'project/work/idf-tools/'+p.name) for p in (ROOT/'work/idf-tools').iterdir() if p.is_file()]
def build(item):
    name,items=item;out=DEST/name
    with zipfile.ZipFile(out,'w',compression=zipfile.ZIP_DEFLATED,compresslevel=3,allowZip64=True) as z:
        for source,prefix in items:
            isfile=source.is_file()
            files=[source] if isfile else (Path(base)/name for base,dirs,names in os.walk(source) for name in names)
            for p in files:
                rel=Path() if isfile else p.relative_to(source)
                # Reflogs are local history, caches are redundant; retain official repo objects and source.
                if '__pycache__' in rel.parts or ('.git' in rel.parts and 'logs' in rel.parts):continue
                arc=prefix if isfile else prefix+'/'+rel.as_posix()
                if p.name=='idf-env.json':
                    z.writestr(arc,json.dumps({'idfInstalled':{}}));continue
                if p.name=='pyvenv.cfg':
                    lines=p.read_text().splitlines()
                    z.writestr(arc,'\n'.join(l for l in lines if not l.startswith(('home =','executable =','command =')))+'\nhome = RELOCATE_WITH_SETUP\nexecutable = RELOCATE_WITH_SETUP\n')
                else:z.write(p,arc)
    if out.stat().st_size>=2*1024**3:raise RuntimeError(f'Asset exceeds GitHub limit: {name}')
    with out.open('rb') as f:h=hashlib.file_digest(f,'sha256').hexdigest()
    print(f'{name}: {out.stat().st_size:,} bytes SHA256 {h}',flush=True)
    return {'name':name,'bytes':out.stat().st_size,'sha256':h}
with concurrent.futures.ThreadPoolExecutor(max_workers=3) as pool:result=list(pool.map(build,groups.items()))
(DEST/'dependency-assets.json').write_text(json.dumps(result,indent=2))
print('Release dependency archives complete.',flush=True)
