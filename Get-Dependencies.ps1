param([string]$Repository='Art4Tech/bc250-ronin-panel',[string]$Tag='migration-2026-09-26')
$ErrorActionPreference='Stop'
if($Repository -eq 'REPOSITORY_PENDING'){throw 'Pass -Repository owner/repository until publishing is complete.'}
if($Repository -notmatch '^[A-Za-z0-9_.-]+/[A-Za-z0-9_.-]+$'){throw 'Expected owner/repository.'}
$assets=Get-Content -LiteralPath (Join-Path $PSScriptRoot 'dependency-assets.json') -Raw | ConvertFrom-Json
$downloads=Join-Path $PSScriptRoot '.downloads'
New-Item -ItemType Directory -Force -Path $downloads | Out-Null
foreach($asset in $assets){
    $path=Join-Path $downloads $asset.name
    if(!(Test-Path -LiteralPath $path)){
        Write-Host "Downloading $($asset.name)"
        Invoke-WebRequest -Uri "https://github.com/$Repository/releases/download/$Tag/$($asset.name)" -OutFile $path
    }
    $hash=(Get-FileHash -LiteralPath $path -Algorithm SHA256).Hash
    if($hash -ne $asset.sha256){throw "Checksum mismatch: $($asset.name). Delete only that download and retry."}
    Write-Host "Verified; extracting $($asset.name)"
    Expand-Archive -LiteralPath $path -DestinationPath $PSScriptRoot -Force
}
& (Join-Path $PSScriptRoot 'Setup-Windows.ps1')
if($LASTEXITCODE -ne 0){throw 'Setup failed.'}
