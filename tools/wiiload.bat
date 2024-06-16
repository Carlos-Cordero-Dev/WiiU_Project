@echo off
cd /d D:\Devkitpro\wut\WiiU_Project\tools
@echo on

REM Clear the contents of GeckoLog.txt
echo. > GeckoLog.txt

REM Open UdpDebugReader.exe in a new command window
start cmd /k "UdpDebugReader.exe"

REM Set the path to the log file
set WIILOAD=tcp:192.168.1.31
wiiload.exe ..\build\WiiU_Project.rpx
