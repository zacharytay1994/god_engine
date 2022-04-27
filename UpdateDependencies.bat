rem Path presets
Set "ExternalDependencies=./0_ExternalDependencies"

rem Make dependencies directory if does not exist
if not exist "%ExternalDependencies%" mkdir "%ExternalDependencies%"

rem Sol2 for lua scripting
rmdir /s /q "%ExternalDependencies%/Sol2/"
git clone "https://github.com/ThePhD/sol2.git" "%ExternalDependencies%/Sol2/"


pause