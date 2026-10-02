$ErrorActionPreference = 'Stop'

$nodeCommand = Get-Command node.exe -ErrorAction SilentlyContinue
if ($nodeCommand) {
    $nodePath = $nodeCommand.Source
} else {
    $nodePath = Join-Path $env:USERPROFILE '.cache\codex-runtimes\codex-primary-runtime\dependencies\node\bin\node.exe'
}

if (-not (Test-Path -LiteralPath $nodePath)) {
    Write-Error 'Không tìm thấy Node.js. Hãy cài Node.js hoặc mở task này trong môi trường Codex.'
    exit 1
}

& $nodePath (Join-Path $PSScriptRoot 'server.js')
exit $LASTEXITCODE
