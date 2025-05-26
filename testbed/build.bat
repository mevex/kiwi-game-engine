@echo off

SETLOCAL ENABLEDELAYEDEXPANSION

set FileNames=
for /R %%f in (*.cpp) do (
	call set FileNames=!FileNames! %%f
)

pushd ..\bin

set Assembly=Testbed
set IncludeFolders=/I../testbed/src /I../engine/src/
set Defines=-DKIWI_SLOW -DKIWI_ENGINE_EXPORTS
rem set IgnoredWarnings=/wd4201 /wd4505

set CompilerFlags=/nologo /MTd /fp:fast /GR- /EHa- /Od /Oi /WX /W4 /FC /Zi %IncludeFolders% %Defines% %IgnoredWarnings%
set LinkerFlags=/nologo /incremental:no /opt:ref engine.lib


cl %CompilerFlags% %FileNames% /Fe%Assembly% /link %LinkerFlags%

popd