@echo off

setlocal
set uac=~uac_permission_tmp_%random%
md "%SystemRoot%\system32\%uac%" 2>nul
if %errorlevel%==0 ( rd "%SystemRoot%\system32\%uac%" >nul 2>nul ) else (
    echo set uac = CreateObject^("Shell.Application"^)>"%temp%\%uac%.vbs"
    echo uac.ShellExecute "%~s0","","","runas",1 >>"%temp%\%uac%.vbs"
    echo WScript.Quit >>"%temp%\%uac%.vbs"
    "%temp%\%uac%.vbs" /f
    del /f /q "%temp%\%uac%.vbs" & exit )
endlocal

echo UnInstalling MySQL...

echo Stopping MySQL service...
net stop MySQL842ChuanQi

echo Waiting for MySQL service to stop...
timeout /t 5 > nul

echo Deleting MySQL service...
sc delete MySQL842ChuanQi

echo Removing MySQL data directory and configuration file...

set "MYSQL_INSTALL_PATH=%~dp0mysql-8.4.2-winx64"
rd /s /q "%MYSQL_INSTALL_PATH%\data"
del /f /q "%MYSQL_INSTALL_PATH%\my.ini"

echo Uninstall finished.