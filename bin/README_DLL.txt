WINDOWS DLL NOTE

Do not invent or download random DLL files.

The exact DLLs required depend on:
- your openFrameworks version,
- whether you use Visual Studio or MSYS2,
- the architecture/toolchain used to build the app,
- any addons you add later.

MSYS2:
After building the application, from the project folder run:

    make copy_dlls

This copies the runtime DLLs required by that build next to the executable.

Visual Studio:
Build the project generated for your installed openFrameworks version.
Use the DLLs/runtime files supplied by that exact OF/toolchain build.
Before submission, test the contents of bin/ by launching the built .exe
from the bin folder (and ideally on another Windows PC).

Do not copy DLLs from an unrelated openFrameworks release.
