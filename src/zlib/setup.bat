@echo off
pushd %~dp0
if not exist ..\zlib_config mkdir ..\zlib_config
if not exist ..\zlib_config\zlib_config.h call :copy_config

popd
goto :eof

:copy_config
echo copy zlib_config.h.in to ..\zlib_config\zlib_config.h
copy zlib_config.h.in ..\zlib_config\zlib_config.h>nul
goto :eof