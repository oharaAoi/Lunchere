param(
    [ValidateSet("Debug", "Release")]
    [string]$Configuration = "Debug"
)

$ErrorActionPreference = "Stop"

$vswhere = Join-Path ([Environment]::GetFolderPath("ProgramFilesX86")) "Microsoft Visual Studio\Installer\vswhere.exe"
if (!(Test-Path $vswhere)) {
    throw "vswhere.exe was not found."
}

$msbuild = & $vswhere -latest -products * -requires Microsoft.Component.MSBuild -find "MSBuild\**\Bin\MSBuild.exe" | Select-Object -First 1
if (!$msbuild) {
    throw "MSBuild.exe was not found."
}

$solution = Resolve-Path (Join-Path $PSScriptRoot "..\Project\AOENGINE.sln")
& $msbuild $solution /m /nr:false "/p:Configuration=$Configuration" /p:Platform=x64
exit $LASTEXITCODE
