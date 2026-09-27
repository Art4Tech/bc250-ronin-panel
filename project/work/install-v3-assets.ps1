$ErrorActionPreference='Stop'
& outputs/Resource-Panel/Windows-Monitor.ps1 -Action Stop
try {
 $movieAssets=@('day3.rvj','nite3.rvj','sleep3.rvj','wake3.rvj','day3.pcm','nite3.pcm','sleep3.pcm','wake3.pcm') | ForEach-Object { Join-Path 'outputs/Resource-Panel/video/movies-v3' $_ }
 & work/idf-tools/python_env/idf5.5_py3.12_env/Scripts/python.exe outputs/Resource-Panel/upload_video.py --chunk-size 1024 @movieAssets
 if($LASTEXITCODE -ne 0){throw 'Transfer failed; previous video set preserved.'}
 & work/panel-tools/Scripts/python.exe -m esptool --chip esp32p4 --port COM10 --after hard-reset chip-id
 if($LASTEXITCODE -ne 0){throw 'Transferred assets but restart failed.'}
} finally { & outputs/Resource-Panel/Windows-Monitor.ps1 -Action Start }
