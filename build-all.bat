@echo off

if not defined VCINSTALLDIR (
	echo [91;4mMSVC not initialized![0m
	echo You must execute vcvarsall.bat before executing this batch file.
	echo It is contained inside your VS install folder in a path similar to
	echo C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat
	exit /B
)

if not exist ..\bin mkdir ..\bin

pushd engine
call build.bat
popd
if %errorlevel% neq 0 (echo Error:%errorlevel% && exit)

pushd testbed
call build.bat
popd
if %errorlevel% neq 0 (echo Error:%errorlevel% && exit)

echo "Build finished successfully! YAY"