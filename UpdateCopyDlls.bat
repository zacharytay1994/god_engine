rem Path presets
Set "ExternalDependencies=./0_ExternalDependencies"
Set "TargetDirDebug=./x64/Debug"
Set "TargetDirRelease=./x64/Release"

rem Copy assimp dlls
robocopy "%ExternalDependencies%/assimp/build/x64/bin/Debug" "%TargetDirDebug%" "assimp-vc142-mtd.dll"
robocopy "%ExternalDependencies%/assimp/build/x64/bin/Release" "%TargetDirRelease%" "assimp-vc142-mt.dll"

pause