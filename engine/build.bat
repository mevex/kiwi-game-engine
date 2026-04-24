@echo off

SETLOCAL ENABLEDELAYEDEXPANSION

break>..\bin\engine_unity_build.cpp

for /R %%f in (*.cpp) do (
	set "IncludeFile=#include "%%f""
	echo !IncludeFile! >> ..\bin\engine_unity_build.cpp
)

pushd ..\bin

set Assembly=Engine
set IncludeFolders=/I../engine/src /I%VULKAN_SDK%/Include
set Defines=/DKIWI_SLOW /DKIWI_ENGINE_EXPORTS
:: set IgnoredWarnings=/wd4201 /wd4505 /wd4820
:: set AdditionalWarnings=/w14820
set WarningsOptions=/W4 /WX %AdditionalWarnings% %ignoredWarnings%
set Libraries=user32.lib winmm.lib %VULKAN_SDK%\Lib\vulkan-1.lib

set CompilerFlags=/nologo /MTd /fp:fast /GR- /Od /Oi /FC /Zi /LD /permissive- %IncludeFolders% %Defines% %WarningsOptions%
set LinkerFlags=/nologo /incremental:no /opt:ref /WX %Libraries%

cl %CompilerFlags% engine_unity_build.cpp /Fe%Assembly% /link %LinkerFlags%

popd