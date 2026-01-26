@echo off
setlocal

set SHELL=cmd.exe
set SHELLFLAGS=/C

pushd "%~dp0%"

echo $ Tangent2\Lapis\make.exe all
..\..\Lapis\make.exe all
if errorlevel 1 (
  echo === make.exe FAILED (exit code %ERRORLEVEL%) ===
  echo === Build Failed ===
  popd
  endlocal
  exit /b %ERRORLEVEL%
)

echo $ Tangent2\Lapis\hex2bin.exe Tangent.HEX
..\..\Lapis\hex2bin.exe Tangent.HEX
if errorlevel 1 (
  echo === hex2bin FAILED (exit code %ERRORLEVEL%) ===
  echo === Build Failed ===
  popd
  endlocal
  exit /b %ERRORLEVEL%
)

echo === Build Successful ===
popd
endlocal
exit /b 0
