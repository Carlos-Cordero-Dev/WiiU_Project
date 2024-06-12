@echo off

.\tools\premake5.exe vs2019

setlocal

rem Set the path to be added
set INCLUDE_PATH=D:\DevkitPro\wut\include

rem Find all .vcxproj files in the solution directory
for /r %%i in (*.vcxproj) do (
    call :AddIncludePath "%%i"
)

echo Include path added to all project files.
pause
goto :eof

:AddIncludePath
set "projectFile=%~1"

rem Backup the original project file
copy "%projectFile%" "%projectFile%.bak"

rem Use PowerShell to add the include path to the project file
powershell -Command ^
    "(Get-Content -Path '%projectFile%') | ForEach-Object { $_ -replace '<AdditionalIncludeDirectories>(.*)</AdditionalIncludeDirectories>', '<AdditionalIncludeDirectories>$1;%INCLUDE_PATH%</AdditionalIncludeDirectories>' } | Set-Content -Path '%projectFile%'"

goto :eof
