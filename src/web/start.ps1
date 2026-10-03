$ErrorActionPreference = 'Stop'

$root = (Resolve-Path (Join-Path $PSScriptRoot '..\..')).Path
$buildDirectory = Join-Path $root 'build'
$bridgePath = Join-Path $buildDirectory 'library_management.exe'
$sourceDirectory = Join-Path $root 'src'
$compiler = Get-Command g++.exe -ErrorAction SilentlyContinue
if (-not $compiler) {
    Write-Error "g++ was not found in PATH. Install MinGW-w64 to build the DSA Core."
    exit 1
}
New-Item -ItemType Directory -Force -Path $buildDirectory | Out-Null
$sources = Get-ChildItem $sourceDirectory -Recurse -Filter '*.cpp' | ForEach-Object { $_.FullName }
$compilerArguments = @('-std=c++17', '-Wall', '-Wextra', '-Wpedantic', '-finput-charset=UTF-8', '-fexec-charset=UTF-8', "-I$sourceDirectory")
$compilerArguments += $sources
$compilerArguments += @('-o', $bridgePath)
$compilerPath = $compiler.Source
& $compilerPath @compilerArguments
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

$env:LIBRARY_BRIDGE_PATH = $bridgePath

$nodeCommand = Get-Command node.exe -ErrorAction SilentlyContinue
if ($nodeCommand) {
    $nodePath = $nodeCommand.Source
} else {
    $nodePath = Join-Path $env:USERPROFILE '.cache\codex-runtimes\codex-primary-runtime\dependencies\node\bin\node.exe'
}

if (-not (Test-Path -LiteralPath $nodePath)) {
    Write-Error "Node.js was not found. Install Node.js or use the bundled Codex runtime."
    exit 1
}

$serverScript = Join-Path $PSScriptRoot 'server.js'
& $nodePath $serverScript
exit $LASTEXITCODE
