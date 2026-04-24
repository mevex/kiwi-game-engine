@echo off

SETLOCAL ENABLEDELAYEDEXPANSION

break>..\bin\testbed_unity_build.cpp

for /R %%f in (*.cpp) do (
	set "IncludeFile=#include "%%f""
	echo !IncludeFile! >> ..\bin\testbed_unity_build.cpp
)

pushd ..\bin

set Assembly=Testbed
set IncludeFolders=/I../testbed/src /I../engine/src/
set Defines=/DKIWI_SLOW
:: set IgnoredWarnings=/wd4201 /wd4505
:: set AdditionalWarnings=/w44820
set WarningsOptions=/W4 /WX %AdditionalWarnings% %ignoredWarnings%

set CompilerFlags=/nologo /MTd /fp:fast /GR- /Od /Oi /FC /Zi /permissive- %IncludeFolders% %Defines% %WarningsOptions%
set LinkerFlags=/nologo /incremental:no /opt:ref engine.lib


cl %CompilerFlags% testbed_unity_build.cpp /Fe%Assembly% /link %LinkerFlags%

popd