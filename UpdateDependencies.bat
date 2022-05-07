rem Path presets
Set "ExternalDependencies=./0_ExternalDependencies"

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

rem stbimage for loading pngs/jpgs
rmdir /s /q "%ExternalDependencies%/stb/"
git clone https://github.com/nothings/stb.git "%ExternalDependencies%/stb/"

rem assimp for model loading, build and copy libraries to libraries directory
rmdir /s /q "%ExternalDependencies%/assimp"
git clone https://github.com/assimp/assimp.git "%ExternalDependencies%/assimp/"
cd "%ExternalDependencies%/assimp/"
call BUILDBINARIES_EXAMPLE.bat
cd "../../"
robocopy "%ExternalDependencies%/assimp/build/x64/lib/Debug" "%ExternalDependencies%/Libraries"
robocopy "%ExternalDependencies%/assimp/build/x64/lib/Release" "%ExternalDependencies%/Libraries"

pause