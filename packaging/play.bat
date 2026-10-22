@echo off
cd /d "%~dp0"
if errorlevel 1 exit /b 1
WorldBreaker.exe %*
exit /b %errorlevel%
