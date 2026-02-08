# Kiwi Engine
Kiwi is yet another single-developer, from-scratch, open-source game engine. It is not my first attempt at it either, but hopefully that's the good one.

The project is not built with any particular goal in mind other than to follow curiosity wherever it leads, to tinker, to build, and to learn by doing. Sometimes, the process is the point.

# Feature List
Here I will add every feature with a brief explanation. Since we are just starting out for now the following are subsystems, not really features.

- Logger
- Events Subsystem
- Mouse & Keyboard Input
- Arena Based Memory Sybsystem
- Basic Vulkan Renderer (WIP)
	- Initialization & Screen Clearing
- Custom Math Library (WIP)
- Custom Data Structures
	- Dynamic Array
	- Linked List

# Project Structure
The engine itself will be a simple .dll that will be loaded by any application that will need it. The only executable that the build process produces for now is Testbed.exe.

# Build Instructions
The project is meant to run only on 64-bit Windows, and it will probably remain this way. In order to build it you must have access to MSVC (Visual Studio C/C++ Compiler).

Build process:
1. Open any type of CLI on Windows and navigate to your Visual Studio install directory. Usually inside your `Program Files` directory.
2. Navigate to `Microsoft Visual Studio\xxxx\Community\VC\Auxiliary\Build\`.
3. Run `vcvarsall.bat x64` or `vcvars64.bat`.
4. Return to the directory in which you cloned/downloaded the source code and run `build-all.bat`