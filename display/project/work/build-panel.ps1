$ErrorActionPreference = 'Stop'
$env:IDF_PATH = Join-Path $PSScriptRoot 'esp-idf'
$env:IDF_TOOLS_PATH = Join-Path $PSScriptRoot 'idf-tools'
$env:IDF_PYTHON_ENV_PATH = Join-Path $env:IDF_TOOLS_PATH 'python_env\idf5.5_py3.12_env'
$env:PYTHONUTF8 = '1'
$env:PIP_CACHE_DIR = Join-Path $PSScriptRoot 'pip-cache'
$env:PATH = "$env:IDF_PYTHON_ENV_PATH\Scripts;C:\Program Files\Git\usr\bin;C:\Program Files\Git\mingw64\bin;" + $env:PATH
$env:GIT_CONFIG_COUNT = '1'
$env:GIT_CONFIG_KEY_0 = 'http.sslBackend'
$env:GIT_CONFIG_VALUE_0 = 'openssl'
$toolEnv = & "$env:IDF_PYTHON_ENV_PATH\Scripts\python.exe" "$env:IDF_PATH\tools\idf_tools.py" export --format key-value
if ($LASTEXITCODE -ne 0) { throw 'Tool environment setup failed' }
foreach ($line in $toolEnv) {
 if ($line -match '^([A-Z_][A-Z0-9_]*)=(.*)$') {
  $value = $Matches[2].Replace('%PATH%', $env:PATH).Replace('$PATH', $env:PATH)
  [Environment]::SetEnvironmentVariable($Matches[1], $value, 'Process')
 }
}
$compilerBin = Join-Path $env:IDF_TOOLS_PATH 'tools\riscv32-esp-elf\esp-14.2.0_20260121\riscv32-esp-elf\bin'
$env:PATH = $compilerBin + ';' + $env:PATH
$env:IDF_CCACHE_ENABLE = '0'
& "$env:IDF_PYTHON_ENV_PATH\Scripts\python.exe" "$env:IDF_PATH\tools\idf.py" -C "$PSScriptRoot\panel-test" -D "CMAKE_MAKE_PROGRAM=$env:IDF_TOOLS_PATH/tools/ninja/1.12.1/ninja.exe" -D CCACHE_ENABLE=0 -D "CMAKE_PROGRAM_PATH=$compilerBin" build
exit $LASTEXITCODE



