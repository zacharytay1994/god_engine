rem Path presets
Set "ExternalDependencies=./0_ExternalDependencies"
Set "InternalDependencies=./0_InternalDependencies"
Set "TargetDirDebug=./x64/Debug"
Set "TargetDirRelease=./x64/Release"

rem Built library paths
rmdir /s /q "%ExternalDependencies%/Libraries/"
mkdir "%ExternalDependencies%/Libraries/"

rem Make dependencies directory if does not exist
if not exist "%ExternalDependencies%" mkdir "%ExternalDependencies%"

rem glm for math functionality
rmdir /s /q "%ExternalDependencies%/glm/"
git clone "https://github.com/g-truc/glm.git" "%ExternalDependencies%/glm/"

rem Sol2 for lua scripting
rmdir /s /q "%ExternalDependencies%/Sol2/"
git clone "https://github.com/ThePhD/sol2.git" "%ExternalDependencies%/Sol2/"

rem Rapidjson
rmdir /s /q "%ExternalDependencies%/RapidJSON/"
git clone "https://github.com/Tencent/rapidjson.git" "%ExternalDependencies%/RapidJSON/"

rem PhysX
rmdir /s /q "%ExternalDependencies%/PhysX/"
git clone "https://github.com/NVIDIAGameWorks/PhysX.git" "%ExternalDependencies%/PhysX/"

rem assimp for model loading, build and copy libraries to libraries directory
rmdir /s /q "%ExternalDependencies%/assimp"
git clone https://github.com/assimp/assimp.git "%ExternalDependencies%/assimp/"
cd "%ExternalDependencies%/assimp/"
call BUILDBINARIES_EXAMPLE.bat
cd "../../"

rem copy assimp libs
robocopy "%ExternalDependencies%/assimp/build/x64/lib/Debug" "%ExternalDependencies%/Libraries"
robocopy "%ExternalDependencies%/assimp/build/x64/lib/Release" "%ExternalDependencies%/Libraries"

rem Copy assimp dlls
robocopy "%ExternalDependencies%/assimp/build/x64/bin/Debug" "%TargetDirDebug%" "assimp-vc142-mtd.dll"
robocopy "%ExternalDependencies%/assimp/build/x64/bin/Release" "%TargetDirRelease%" "assimp-vc142-mt.dll"

rem copy lua dlls
robocopy "%InternalDependencies%/lua" "%TargetDirDebug%" "lua.dll"
robocopy "%InternalDependencies%/lua" "%TargetDirRelease%" "lua.dll"

pause