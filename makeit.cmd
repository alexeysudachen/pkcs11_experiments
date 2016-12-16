@echo off

set MAKEIT_DIR=%~dp0
set OUTDIR_ROOT=%~dp0

cmd /c %XTERNAL%\makeit_proc.cmd %*


