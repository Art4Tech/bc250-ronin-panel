$ErrorActionPreference='Stop'
$root=$PSScriptRoot
$env:PYTHONUTF8='1'
$pairs=@(
    @('project\work\idf-tools\python_env\idf5.5_py3.12_env','project\work\python\cpython-3.12.13-windows-x86_64-none'),
    @('project\work\panel-tools','dependencies\Python314')
)
foreach($pair in $pairs){
    $venv=Join-Path $root $pair[0]
    $base=Join-Path $root $pair[1]
    $cfg=Join-Path $venv 'pyvenv.cfg'
    $content=Get-Content -LiteralPath $cfg
    $content=$content | ForEach-Object {
        if($_ -match '^home ='){'home = '+$base}
        elseif($_ -match '^executable ='){'executable = '+(Join-Path $base 'python.exe')}
        elseif($_ -match '^command ='){'command = migrated environment; use python.exe -m module'}
        else{$_}
    }
    [IO.File]::WriteAllLines($cfg,[string[]]$content,[Text.UTF8Encoding]::new($false))
    & (Join-Path $venv 'Scripts\python.exe') -c 'import sys; print(sys.version); print(sys.prefix)'
    if($LASTEXITCODE -ne 0){throw 'Relocated Python failed.'}
}
$env:PATH=(Join-Path $root 'dependencies\Git\cmd')+';'+$env:PATH
$idfPath=Join-Path $root 'project\work\esp-idf'
$installed=@{}
$installed[$idfPath+'-v5.5']=@{version='5.5';path=$idfPath;features=@('core');targets=@('esp32p4')}
$idfConfig=@{idfInstalled=$installed} | ConvertTo-Json -Depth 5
[IO.File]::WriteAllText((Join-Path $root 'project\work\idf-tools\idf-env.json'),$idfConfig,[Text.UTF8Encoding]::new($false))
$python=Join-Path $root 'project\work\idf-tools\python_env\idf5.5_py3.12_env\Scripts\python.exe'
& $python -c 'import psutil, serial, PIL, imageio_ffmpeg, idf_component_manager; print("Companion, media and IDF dependencies import successfully")'
if($LASTEXITCODE -ne 0){throw 'Dependency check failed.'}
& (Join-Path $root 'project\work\panel-tools\Scripts\python.exe') -m esptool version
if($LASTEXITCODE -ne 0){throw 'Flashing environment failed.'}
Write-Host 'Setup complete. See HANDOVER.md. No device was flashed or service installed.'
