@echo off
REM Usage: copy_prebuilt.bat <source_dirs_semi_colon_separated> <build_output_dir>
REM Example: copy_prebuilt.bat "thirdparty\enet;prebuilt\libs" "bin\Debug"

if "%~1"=="" (
  echo No source dirs provided. Provide semicolon-separated directories to search for dll/lib.
  exit /b 1
)
if "%~2"=="" (
  echo No build output dir provided.
  exit /b 1
)

set SOURCES=%~1
set TARGET=%~2

REM Split SOURCES by ; and search for .dll/.lib
for %%D in (%SOURCES::= %) do (
  echo Searching in %%D ...
  if exist "%%D" (
    for /r "%%D" %%F in (*.dll *.lib *.dll.a *.a) do (
      echo Copying %%F to %TARGET%
      xcopy /Y "%%F" "%TARGET%\" >nul
    )
  )
)
echo Done.
