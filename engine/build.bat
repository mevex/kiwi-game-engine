@echo off

SETLOCAL ENABLEDELAYEDEXPANSION

set FileNames=
for /R %%f in (*.cpp) do (
	set FileNames=!FileNames! %%f
)

pushd ..\bin

set Assembly=Engine
set IncludeFolders=/I../engine/src /I%VULKAN_SDK%/Include
set Defines=-DKIWI_SLOW -DKIWI_ENGINE_EXPORTS
rem set IgnoredWarnings=/wd4201 /wd4505
set Libraries=user32.lib winmm.lib %VULKAN_SDK%\Lib\vulkan-1.lib

set CompilerFlags=/nologo /MDd /fp:fast /GR- /EHa- /Od /Oi /WX /W4 /FC /Zi /LD /permissive- %IncludeFolders% %Defines% %IgnoredWarnings%
set LinkerFlags=/nologo /incremental:no /opt:ref %Libraries%

cl %CompilerFlags% %FileNames% /Fe%Assembly% /link %LinkerFlags%

popd