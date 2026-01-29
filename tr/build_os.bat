@echo off
setlocal

set SHELL=cmd.exe
set SHELLFLAGS=/C

pushd "%~dp0%"

echo $ Tangent2\Lapis\make.exe all
..\..\Lapis\make.exe all

echo $ Tangent2\Lapis\hex2bin.exe TANGENT.HEX
..\..\Lapis\hex2bin.exe Tangent.HEX

popd
endlocal
exit /b 0
