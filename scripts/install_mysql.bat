@echo off
setlocal enabledelayedexpansion

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

echo Checking if MySQL service exists...

sc query MySQL842ChuanQi >nul 2>&1
if %errorlevel%==0 (
    echo MySQL service already exists. Skipping installation.
    goto :EOF
)

echo Installing MySQL...

for %%i in ("%~dp0.") do set "SCRIPT_DIR=%%~fi"

set "MYSQL_INSTALL_PATH=!SCRIPT_DIR!\mysql-8.4.2-winx64"

echo Creating MySQL configuration file...
(
    echo [mysqld]
    echo basedir=!MYSQL_INSTALL_PATH!
    echo datadir=!MYSQL_INSTALL_PATH!\data
    echo port=13306
    echo character-set-server=utf8mb4
    echo collation-server=utf8mb4_unicode_ci
    echo [client]
    echo port=13306
    echo default-character-set=utf8mb4
    echo [mysql]
    echo default-character-set=utf8mb4
) > "!MYSQL_INSTALL_PATH!\my.ini"

echo Initializing MySQL data directory...
mkdir "!MYSQL_INSTALL_PATH!\data"
"!MYSQL_INSTALL_PATH!\bin\mysqld" --initialize-insecure --user=mysql

echo Installing MySQL service...
"!MYSQL_INSTALL_PATH!\bin\mysqld" --install MySQL842ChuanQi --defaults-file="!MYSQL_INSTALL_PATH!\my.ini"

echo Starting MySQL service...
net start MySQL842ChuanQi

echo Waiting for MySQL service to start...
timeout /t 5 > nul

echo MySQL installation and configuration completed.

"!MYSQL_INSTALL_PATH!\bin\mysql" -h localhost -u root -e "CREATE DATABASE IF NOT EXISTS FreqStation;"

"!MYSQL_INSTALL_PATH!\bin\mysql" -h localhost -u root FreqStation < "!SCRIPT_DIR!\FreqStation.sql"
echo Deleting SQL file...
del /f /q "!SCRIPT_DIR!\FreqStation.sql"

echo Script execution finished.