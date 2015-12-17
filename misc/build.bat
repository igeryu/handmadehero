

mkdir ..\build
pushd ..\build
cl -Zi ..\handmadehero\win32_handmade.cpp user32.lib
popd