param(
    [Parameter(Mandatory = $true)]
    [string]$TestFile
)

$ErrorActionPreference = 'Stop'
$workspaceRoot = Split-Path -Parent $PSScriptRoot
$testPath = [System.IO.Path]::GetFullPath($TestFile)
$testsRoot = [System.IO.Path]::GetFullPath((Join-Path $workspaceRoot 'tests')).TrimEnd('\') + '\'

if (-not $testPath.StartsWith($testsRoot, [System.StringComparison]::OrdinalIgnoreCase)) {
    Write-Error 'Open a .cpp file inside the tests folder before running this task.'
    exit 1
}

if ([System.IO.Path]::GetExtension($testPath) -ne '.cpp') {
    Write-Error 'The active test file must have a .cpp extension.'
    exit 1
}

$compiler = Get-Command g++ -ErrorAction SilentlyContinue
if (-not $compiler) {
    Write-Error 'g++ was not found. Install MinGW-w64 and add its bin folder to PATH.'
    exit 1
}

$sourceRoot = Join-Path $workspaceRoot 'src'
$mainSource = [System.IO.Path]::GetFullPath((Join-Path $sourceRoot 'main.cpp'))
$projectSources = @(
    Get-ChildItem -Path $sourceRoot -Recurse -File -Filter '*.cpp' |
        Where-Object { $_.FullName -ne $mainSource } |
        ForEach-Object { $_.FullName }
)

$buildDirectory = Join-Path $workspaceRoot 'build'
New-Item -ItemType Directory -Force -Path $buildDirectory | Out-Null
$outputPath = Join-Path $buildDirectory (([System.IO.Path]::GetFileNameWithoutExtension($testPath)) + '.exe')
$compilerArguments = @(
    '-std=c++17',
    '-Wall',
    '-Wextra',
    '-Wpedantic',
    '-finput-charset=UTF-8',
    '-fexec-charset=UTF-8',
    ('-I' + $sourceRoot),
    $testPath
) + $projectSources + @('-o', $outputPath)

& $compiler.Source @compilerArguments
if ($LASTEXITCODE -ne 0) {
    exit $LASTEXITCODE
}

& $outputPath
exit $LASTEXITCODE
