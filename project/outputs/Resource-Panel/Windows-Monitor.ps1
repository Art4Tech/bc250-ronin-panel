param([ValidateSet('Start','Stop')][string]$Action='Start',[string]$Port='COM10')
$ErrorActionPreference='Stop'
if($Port -notmatch '^COM\d+$'){throw 'Specify a Windows port such as COM10.'}
$runtime=Join-Path $PSScriptRoot '.venv\Scripts\python.exe'
if(!(Test-Path -LiteralPath $runtime)){$runtime=Join-Path $PSScriptRoot '..\..\work\idf-tools\python_env\idf5.5_py3.12_env\Scripts\python.exe'}
if(!(Test-Path -LiteralPath $runtime)){throw 'Create the Python environment using README.md first.'}
& $runtime (Join-Path $PSScriptRoot 'monitor_control.py') $Action --port $Port
if($LASTEXITCODE -ne 0){throw 'Panel monitoring command failed; see the message above.'}
