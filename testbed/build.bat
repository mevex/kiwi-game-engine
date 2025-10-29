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
set Defines=-DKIWI_SLOW -DKIWI_ENGINE_EXPORTS
:: set IgnoredWarnings=/wd4201 /wd4505

set CompilerFlags=/nologo /MTd /fp:fast /GR- /EHa- /Od /Oi /WX /W4 /FC /Zi /permissive- %IncludeFolders% %Defines% %IgnoredWarnings%
set LinkerFlags=/nologo /incremental:no /opt:ref engine.lib


cl %CompilerFlags% testbed_unity_build.cpp /Fe%Assembly% /link %LinkerFlags%

popd