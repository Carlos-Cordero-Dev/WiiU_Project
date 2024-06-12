@echo off
REM Set the path to the Cemu executable
set CEMU_PATH=D:\Cemu\cemu_1.26.2\Cemu.exe

REM Set the path to the .rpx file
set RPX_PATH=%~dp0..\build\WiiU_Project.rpx

REM Set the path to the log file
set LOG_PATH=D:\Cemu\cemu_1.26.2\log.txt

REM Tail the log file
start "" tail -f "%LOG_PATH%"

REM Start Cemu with the .rpx file
start "" "%CEMU_PATH%" -g "%RPX_PATH%"



