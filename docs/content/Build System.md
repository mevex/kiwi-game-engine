---
description: Step-by-step instructions to build the project and an in-depth look at the build system’s structure.
lang: en
draft: false
aliases:
  - Build
---
The project uses a minimal build system. It avoids IDE-specific tooling so the codebase can be built from any environment independently.

# Build Instructions
The following steps describe how to build the entire project after a fresh repository clone. A basic familiarity with the Windows command prompt is assumed.

## Configuring the Command Prompt
To run MSVC (`cl`) from a command prompt, several environment variables must be set. Microsoft provides batch files to simplify this for the current command prompt session. 

These batch files can be located in the Visual Studio's installation folder, under `VC\Auxiliary\Build` (e.g. `C:\Program Files\Microsoft Visual Studio\20**\VC\Auxiliary\Build`). Run `vcvars64.bat` to configure the environment for building an x64 version of the project.

## Running the Build Batch
After configuring the environment, use the same command prompt to navigate to the project folder and run `build-all.bat`. This batch must be executed from the project root folder to work correctly.

## Running the Demo
If the build succeeds, a `bin` folder will be created. It will contain debug files, `Engine.lib` (and `.dll`) and `Testbed.exe`. Run this executable to start the demo.

# Inside the Build System
This section is a deep dive into the build system, explaining how it works and the reasoning behind its design.

## Batch Structure
Running `build-all.bat` triggers a chain reaction that builds the entire project. It builds `Engine.lib` (and `.dll`) first, then `Testbed.exe`, which dynamically links against the engine.

## Unity Build
Each binary is built using a unity build. A generated `.cpp` file in the `bin` folder includes every `.cpp` file from the `src` directory, creating a single large translation unit. This significantly reduces linking time, which is the main bottleneck when building large C++ projects.

This approach keeps build times low without requiring a more sophisticated system that recompiles and incrementally links only modified translation units.

## Compiler Flags & Macros
This section lists the compiler and linker flags and C++ macros used to configure the generation of the binaries.

### Flags
These are all the compiler flags used to build the engine and testbed binaries.

- **`/FC`:** Causes the compiler to display the full path of source code files in diagnostics (errors or warnings).
- **`/fp:fast`:** Uses "fast" floating-point model. Lets the compiler aggressively optimize floating-point code by reordering and simplifying operations, even if it changes rounding behavior or deviates from strict IEEE-754 rules. Suitable only when exact precision, ordering, and special-value handling (like NaN) aren’t critical.
- **`/GR-`:** Disables (`-`) run-time type information. This adds code to check object types at run time when using the `dynamic_cast` or `typeid` operators.
- **`/LD`:** Used only for the engine binary. Builds a DLL along with its corresponding `.lib` file for the testbed binary to link against.
- **`/MTd`:** Statically links the CRT inside the binary. This is done to allow the binary to be standalone and not depend on any redistributable. Specifying `d` makes the compiler use the debug version of the runtime library.
- **`/nologo`:** Suppresses the display of MSVC copyright banner.
- **`/Od`:** Disables all compiler optimizations to make debugging easier.
- **`/Oi`:** Replaces some function calls (usually CRT) with intrinsic or otherwise special forms of the function that help the application run faster.
- **`/permissive-`:** Disables (`-`) MSVC permissive mode enforcing standards conformance of the code making it more portable.
- **`/Zi`:** Produces a separate PDB file that contains all the symbolic debugging information for use with the debugger.

The following is a list of all the linker flags.

- **`/incremental:no`:** Disables incremental linking since it is not needed in a unity build system. This results in a smaller binary.
- **`/nologo`:** Suppresses the display of the copyright banner. Same as compiler.
- **`/opt:ref`:** Eliminates functions and data that are never referenced, resulting in a smaller binary and increased runtime performance.

While `/opt:ref` implicitly disables incremental linking (`/incremental:no`), it is still explicitly specified to guard against potential changes in this behavior in future updates.

### Warnings
The two main flags used here are `/W4`, which sets the warning level to 4 (roughly equivalent to GCC’s `-Wall`) and `/WX`, applied to both the compiler and linker, that treats all warnings as errors.

Additionally, specific warnings may be suppressed or explicitly enabled, either permanently or for debugging/testing purposes. The flag `/wd[xxxx]` is used to suppress a warning, while `/w[l][xxxx]` is used to enable one, where `[xxxx]` is the warning code and `[l]` specifies the level at which it should be reported. These may change between commits, so they are not listed here.

### Macros
Two macros are currently defined using the `/D` compiler flag.

- **`KIWI_ENGINE_EXPORTS`:** Used to resolve the `KIWI_API` macro to `__declspec(dllexport)` or `__declspec(dllimport)`. `KIWI_API` is applied to classes and functions that are part of the game-facing engine API. This macro is defined only when building the engine.
- **`KIWI_SLOW`:** Enables code that may reduce performance, such as assertions, bounds checking, and memory allocation tracking.

Additional macros may be introduced in the future to provide more fine-grained control over the type of binaries being produced.

### Include Folders
The include directories for the binaries are defined using the `/I` compiler flag.

The engine uses `engine/src` and `%VULKAN_SDK%/Include`, which is the standard include directory for the Vulkan API. The testbed uses `testbed/src` and `engine/src`.

### Linking Dependencies
The engine binary is linked against `user32.lib` and `winmm.lib` for the Windows APIs, and `%VULKAN_SDK%\Lib\vulkan-1.lib` for the Vulkan API.

The testbed, on the other hand, links only against the engine (`Engine.lib`).