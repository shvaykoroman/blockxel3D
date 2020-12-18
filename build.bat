#echo

mkdir ..\build
pushd ..\build

call  "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" x64

cl -Zi -Od d:\blockxel\code\win32_platform.cpp user32.lib gdi32.lib opengl32.lib				
REM PNG_READER
REM cl -Zi -Od d:\mario\code\test_png_loader.cpp user32.lib gdi32.lib				

popd		
