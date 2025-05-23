cd build
rm -r -d bin
mkdir bin
cd ../
Copy-Item ./build/Desktop_Qt_6_9_0_MinGW_64_bit-Release/TeamSpeak-Overlay.exe ./build/bin/TeamSpeak-Overlay.exe
echo "GENERATING QT DLL'S"
E:\Qt\6.9.0\mingw_64\bin\windeployqt6.exe ./build/bin/TeamSpeak-Overlay.exe
echo "GENERATING WINDOWS INSTALLER"
./tools/nsis/makensis.exe './TeamSpeak Overlay.nsi'