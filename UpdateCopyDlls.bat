rem Path presets
Set "ExternalDependencies=./0_ExternalDependencies"
Set "InternalDependencies=./0_InternalDependencies"
Set "TargetDirDebug=./x64/Debug"
Set "TargetDirRelease=./x64/Release"

rem copy assimp libs
robocopy "%ExternalDependencies%/assimp/build/x64/lib/Debug" "%ExternalDependencies%/Libraries"
robocopy "%ExternalDependencies%/assimp/build/x64/lib/Release" "%ExternalDependencies%/Libraries"

rem Copy assimp dlls
robocopy "%ExternalDependencies%/assimp/build/x64/bin/Debug" "%TargetDirDebug%" "assimp-vc142-mtd.dll"
robocopy "%ExternalDependencies%/assimp/build/x64/bin/Release" "%TargetDirRelease%" "assimp-vc142-mt.dll"

rem copy lua dlls
robocopy "%InternalDependencies%/lua" "%TargetDirDebug%" "lua.dll"
robocopy "%InternalDependencies%/lua" "%TargetDirRelease%" "lua.dll"

rem copy physX dlls
robocopy "%InternalDependencies%/PhysX" "%TargetDirDebug%" "PhysX_64.dll"
robocopy "%InternalDependencies%/PhysX" "%TargetDirDebug%" "PhysXCommon_64.dll"
robocopy "%InternalDependencies%/PhysX" "%TargetDirDebug%" "PhysXCooking_64.dll"
robocopy "%InternalDependencies%/PhysX" "%TargetDirDebug%" "PhysXFoundation_64.dll"

robocopy "%InternalDependencies%/PhysX" "%TargetDirRelease%" "PhysX_64.dll"
robocopy "%InternalDependencies%/PhysX" "%TargetDirRelease%" "PhysXCommon_64.dll"
robocopy "%InternalDependencies%/PhysX" "%TargetDirRelease%" "PhysXCooking_64.dll"
robocopy "%InternalDependencies%/PhysX" "%TargetDirRelease%" "PhysXFoundation_64.dll"

rem copy fmod dlls
robocopy "%InternalDependencies%/fmod" "%TargetDirDebug%" "fmod.dll"
robocopy "%InternalDependencies%/fmod" "%TargetDirRelease%" "fmod.dll"

pause