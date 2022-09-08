rem Path presets
Set "InternalDependencies=./0_InternalDependencies"
Set "TargetDirDebug=./x64/Debug"
Set "TargetDirRelease=./x64/Release"

rem Copy physx dlls
robocopy "%InternalDependencies%/PhysX/lib/physxdebug" "%TargetDirDebug%" "PhysX_64.dll"
robocopy "%InternalDependencies%/PhysX/lib/physxdebug" "%TargetDirDebug%" "PhysXCommon_64.dll"
robocopy "%InternalDependencies%/PhysX/lib/physxdebug" "%TargetDirDebug%" "PhysXCooking_64.dll"
robocopy "%InternalDependencies%/PhysX/lib/physxdebug" "%TargetDirDebug%" "PhysXFoundation_64.dll"

robocopy "%InternalDependencies%/PhysX/lib/physxrelease" "%TargetDirRelease%" "PhysX_64.dll"
robocopy "%InternalDependencies%/PhysX/lib/physxrelease" "%TargetDirRelease%" "PhysXCommon_64.dll"
robocopy "%InternalDependencies%/PhysX/lib/physxrelease" "%TargetDirRelease%" "PhysXCooking_64.dll"
robocopy "%InternalDependencies%/PhysX/lib/physxrelease" "%TargetDirRelease%" "PhysXFoundation_64.dll"

pause