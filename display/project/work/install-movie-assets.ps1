$ErrorActionPreference='Stop'
& outputs/Resource-Panel/Windows-Monitor.ps1 -Action Stop
try {
 & work/idf-tools/python_env/idf5.5_py3.12_env/Scripts/python.exe outputs/Resource-Panel/upload_video.py outputs/Resource-Panel/video/user-clips/day2.rvj outputs/Resource-Panel/video/user-clips/nite2.rvj outputs/Resource-Panel/video/user-clips/sleep2.rvj outputs/Resource-Panel/video/user-clips/wake2.rvj outputs/Resource-Panel/video/user-clips/nature2.pcm
 if($LASTEXITCODE -ne 0){throw 'Asset transfer failed; original clips remain intact.'}
 & work/panel-tools/Scripts/python.exe -m esptool --chip esp32p4 --port COM10 --after hard-reset chip-id
 if($LASTEXITCODE -ne 0){throw 'Files transferred but automatic restart failed.'}
} finally {
 & outputs/Resource-Panel/Windows-Monitor.ps1 -Action Start
}
