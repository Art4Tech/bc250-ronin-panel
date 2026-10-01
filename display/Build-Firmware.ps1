$ErrorActionPreference='Stop'
$work=Join-Path $PSScriptRoot 'project\work'
$env:IDF_PATH=Join-Path $work 'esp-idf'
$env:IDF_TOOLS_PATH=Join-Path $work 'idf-tools'
$env:IDF_PYTHON_ENV_PATH=Join-Path $env:IDF_TOOLS_PATH 'python_env\idf5.5_py3.12_env'
$env:PYTHONUTF8='1'
$env:IDF_CCACHE_ENABLE='0'
$env:PATH="$env:IDF_PYTHON_ENV_PATH\Scripts;$(Join-Path $PSScriptRoot 'dependencies\Git\cmd');$(Join-Path $PSScriptRoot 'dependencies\Git\usr\bin');$(Join-Path $PSScriptRoot 'dependencies\Git\mingw64\bin');"+$env:PATH
$python=Join-Path $env:IDF_PYTHON_ENV_PATH 'Scripts\python.exe'
$toolEnv=& $python "$env:IDF_PATH\tools\idf_tools.py" export --format key-value
if($LASTEXITCODE -ne 0){throw 'IDF tool environment failed. Run Setup-Windows.ps1 first.'}
foreach($line in $toolEnv){
    if($line -match '^([A-Z_][A-Z0-9_]*)=(.*)$'){
        $value=$Matches[2].Replace('%PATH%',$env:PATH).Replace('$PATH',$env:PATH)
        [Environment]::SetEnvironmentVariable($Matches[1],$value,'Process')
    }
}
$compilerBin=Join-Path $env:IDF_TOOLS_PATH 'tools\riscv32-esp-elf\esp-14.2.0_20260121\riscv32-esp-elf\bin'
$env:PATH=$compilerBin+';'+$env:PATH
& $python "$env:IDF_PATH\tools\idf.py" -C "$work\panel-test" -B "$work\panel-test\build-migrated" -D "CMAKE_MAKE_PROGRAM=$env:IDF_TOOLS_PATH/tools/ninja/1.12.1/ninja.exe" -D CCACHE_ENABLE=0 -D "CMAKE_PROGRAM_PATH=$compilerBin" build
if($LASTEXITCODE -ne 0){throw 'Firmware build failed.'}
Write-Host "Built: $work\panel-test\build-migrated\5inch_Lesson09.bin"
