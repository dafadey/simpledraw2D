# simple draw 2D

This library allows you to draw your 2D data as easy as possible. You need one call to create a graphical window that needs no further maintenance from your side afterwards. Each time you want to plot your 2D data you make one call where you pass the pointer to your data, size of the array along x and y and tile index. You can draw any number of tiles you want. You do not need to track the geometry of data that you draw, since you always pass dimensions the tile will adapt to new values. While your main code is running you can explore the data: zoom, pan, investigate slices along X and Y.

You can stop your code with the keyboard (key S). Simpledraw draw calls are typically non blocking but if you put your graphical window in stop mode the draw call will block your app. While your main app is stopped you can investigate data in the graphical window and even change it (key D). Since you have passed a pointer to the data all your changes actually go to the main application so you can continue with the updated arrays (key C).

Library is distributed as source code. Below are the instructions for build procedures for different OS-es and build tools.

## Linux Build:
For Linux there is a build script that creates a distrubution with the following contents:

- `libsimpledraw2D.so` - shared libray for C/C++/Fortran builds
- `simpledraw2D.so` - shared library with Python bindings, just place it nearby your Python script

Prior to build install GLFW3, GLEW (Open GL Wrangler Library) and Python (optionally), those should be in your distro repository. Then go to project root folder and run `./build_lib`
### Examples
Go to `ccpp/fortran` folder and run `./build`, run generated `*.out` binaries. **NOTE**: Build script sets `-Wl,-rpath=../` so you do not have to set `LD_LIBRARY_PATH`.

Go to python folder and create symbolic link to python library (or just copy `simpledraw2D.so` to python folder)

`ln -s ../simpledraw2D.so simpledraw2D.so`

Run the example

`python3 maxwellTE_test.py`

### Fast python example
To build library the does FDTD calculations run

`./build_fdtd`

and then run

`python3 maxwellTE_test_FAST.py`

**checkout the difference** in speed and also take a look at the code in `fdtd_plasma.py` *vs* `fdtd_plasma_CPython.py + fdtd_plasma.cpp`.

### DYI Final Notes
- Do not need Python? Edit `build_lib` to disable python library `simpledraw2D.so`. Python dependency will disappear.

- Do not need Fortran and C? go ahead and disable embedding `libstdc++` into C/C++/Fortran library `libsimpledraw2D.so`

- Need a static library? Again, do not hesitate to edit `build_lib` script.


## MSVC Windows build:
We will do it manually without any CMake or Ninja ...to become ninjas ourselves.
### Dependensies
Get [GLFW](https://www.glfw.org/) and [GLEW](https://glew.sourceforge.net/) binaries or build it from source. Actually you need just one header from glew so you can proceed with glew source package without building it, but glfw3 is strongly needed.

### Actual Build
#### C/C++
1. After you get the sources, create "Empty C++" MSVC solution inside the sources directory (it is quite a convenient placement). Let it be MSVC<VER> in my case it is MSVC2015.

2. In your solution create a project libsimpledraw2D.
**NOTE**: You do both 1 and 2 steps in the New Project creation tab. Just read all fields carefully otherwise you will have to rename your project inside the solution.

3. Set the architecture. I prefer x64 and Release build type. Actually your arch should match `glfw3` build.

4. Now in the "Solution Explorer" go to `lisimpledraw2D` context menu and choose Properties. Set `Configuration Properties->General->Configuration type = Static library(.lib)`

5. In the same window, set `Configuration Properties->C/C++->General->Additional Include Directories` to `<yourpath>\glfw3\include` and `<yourpath>\glew\include`.

6. In the same window, set `Configuration Properties->Librarian->Additional Library Directories` to `<yourpath>\glfw3\lib`

7. In the same window, set `Configuration properties->Librarian->Additional Dependencies` add `opengl32.lib` and `glfw3.lib`

8. In `Solution Explorer->simpledraw2D->Sources` do `Add->Existing Item` and choose `simpledraw2d_glfw3.cpp`
<br>... we are almost done (no!)

Now your path diverges to 'a' or 'b'. In the case of path 'a' you are building a library with resources but to use it you have to add resources to all your projects every time. It is not very convenient so you can use path 'b'. In which you set `xxd` project to be built before simpledraw2D and before building the latter you set *pre-build* step which uses `xxd.exe` to generate include file `font.inl` which contains font data in a form of simple C character array.

- **a**<br> In `Solution Explorer->libsimpledraw2D->Header Files` do `Add->Existing Item` and choose `simpledraw2D_res.h`
    <br><br>In `Solution Explorer->libsimpledraw2D->Resource Files` do `Add->Existing Item` and choose `simpledraw2D.rc`

- **b**<br> Add `HAVE_FONT_DAT` to `Solution Explorer->libsimpledraw2D->Properties->Configuration properties->C/C++->Preprocessor->Preprocessor Definitions`
    <br><br>Add new project to your solution, name it `xxd`
    <br><br>Navigate to `Solution Explorer->libsimpledraw2D->Properties->Configuration properties->Build Dependencies->Project Dependencies`, where add `xxd` dependency for `libsimpledraw2D`
    <br><br>In `Solution Explorer->simpledraw2D->Properties->Configuration properties->Build Events->Pre-Build Event`
    set the `Command Line` to `"$(OutDir)xxd.exe $(SolutionDir)..\font.dat > $(SolutionDir)..\font.inl"`

In case of 'b' all the font stuff is embed into `libsimpledraw2D.lib`, so I would advice path 'b' as a best practice for Windows build.

### Python
Now we are going to add a Python library. It will be pure *simpledraw2D.pyd* that you should place right near your *py* script, i.e. no `__init__.py`, `setup.py`, pip-s, modules, and MSVC Python extensions. You need just libsimpledraw2D.lib and virgin MSVC capable of building C++ projects. To be sure, say you have [Python](https://www.python.org/downloads/) installed in C:\Python311 and you also got ***numpy*** with pip or so. So let's go. Assuming you have your source MSVC2015 project opened do:

- Add a new *Empty C++* project to the solution with the name ***pysimpledraw2D***. Set `Configuration Properties->General->Configuration type = Dynamic library(.dll)`.

- In the same window, set `Configuration Properties->C/C++->General->Additional Include Directories` to `C:\Python311\include;C:\Python311\Lib\site-packages\numpy\_core\include`. Check the last path!

- In the same window, set `Configuration Properties->Linker->General->Output File` to `simpledraw2D.pyd`

- In the same window, set `Configuration Properties->Linker->General->Additional Library Directories` to `<your path>\MSVC2015\x64\Release;C:\Python311\libs`. First one should point to *libsimpleraw2D.lib*

- In the same window, set `Configuration Properties->Linker->Input->Additional Dependencies = libsimpleraw2D.lib`

- In `Soltion Explorer->pysimpledraw2D(context menu)->Build Dependencies` set *pysimpledraw2D* to depend on *libsimpledraw2D*.

Build *pysimpledraw2D* project. You should have your *.pyd* file in `MSVC2015/x64/Release`.

## Examples:
As we did before with the library I suggest to create project `MSVC2015` in the `ccpp` folder in the sources directory.

This is not required and not tied with the library folder structure just to keep everything in one place and yet not to pollute the sources (we already did it once with font.inl).

So you create a project, set the architecture to any matching `simpledraw2D` and do the following:

- `Solution Explorer-><your project>->Sources->Add->Existing Item` choose... say `testcmd.cpp`

- `Solution Explorer-><your project>->Properties->Configuration properties->C/C++->Additional Include Directories` here you point to somewhat place where you have `simpledraw2D.h`, might be in library sources root

- `Solution Explorer-><your project>->Properties->Configuration properties->Linker->General->Additional Library Directories` here you point to somewhat place where you have libsimpledraw2D.lib (somewhere in `x64/Release` actually)

- `Solution Explorer-><your project>->Properties->Configuration properties->Linker->Input->Additional Dependencies` here you simply add `libsimpledraw2D.lib`

That is it! Build and run.

Also you can build examples by adding `simpledraw2d_glfw3.cpp` as source and also add resource header and resource script rc.

Also you can build with the library built with resources ('a'), in that case you also have to add resource header and resource script to your project.

**NOTE**: If you fail with resources there will be no text in graphical window.

## Using the Library
### C/C++ Examples
1. `test0.cpp`/`test.c` creates a bump in 2D array and waits for input.
2. `test.cpp` creates one bump in first array waits for input and creates second array with two bumps and draws it.
3. `testcmd.cpp` is a command mode application that allows user to add 'bumps' to 2D array by setting *x*, *y*, *width*, *amplitude* separated by spaces in the prompt. So just try `.3 .3 .1 1.` and see what happens in the graphical window.

### Fortran Examples
All three examples solve thermal condutivity problem.
1. hw0.f90 expects user input to continue and has an intentional bug in solution that can be seen on media boundaries.
2. hw.f90 also expects user input to continue but whith fixed bug
3. hw_live.f90 does not expect any user input but calls `fadey_halt()` before main Cauchy for-loop. To continue press `C` (see [Using graphical window](#using-graphical-window) section)

### Python Examples
In CMD go to python folder in simpledraw2D project, set `PATH` to `C:\Python311;C:\Python311\Scripts;%PATH%` also set `PYTHON_PATH` to `C:\Python311` (or setup your Python environment the way I prefer best). Copy *simpledraw2D.pyd* here. Run `python maxwellTE_test.py`.

### API
- `fadey_init(dimx, dimy, tile_count)` opens graphical window, `dimx`, `dimy` does not actually matter, `tile_count` is the number of tiles that plan to show (**NOTE** this value can not be changed)
- `fadey_draw(float/double * array, dimx, dimy, tile_id)` draws your 2D array, `dimx` and `dimy` can be updated for each draw command, tile will update its geometry
- `fadey_close()` closes the graphical window
- `fadey_halt()` switches on *stop* mode

Look at trivial test sources to learn more about API.

### Using Graphical Window
The header shows status, which might be `live`/`stop`/`paint`. Then it shows data under the cursor in the form of `f[tileId](x,y)=value`. Then it shows the number of draw commands it does without stopping, which shows up only in *stop* mode.

In the main area it shows your 2D arrays.

#### Explore your Colormaps with Mouse:

- Hold LMB and drag to pan your colormap.
- Use wheel to zoom
- RMB switches between `normal view`/`X slice view`/`Y slice view`

#### Control the Flow with Keyboard:

- **S** in normal (`live`) mode switches on `stop` mode, in `stop` mode allows for *n* drawing calls before next stop. Number of steps are set with digit keys, backspace sets *n* back to 1. This number is shown in the header as last entry.
- **C** switches on `live` mode
- **D**(*press and hold*) switches on paint mode.

#### Paint Mode

- LMB draws data
- RMB picks the value
- Wheel change brush size (brush is shown with bright green circle)

#### Key Press Glitch
In some cases pressing D in *stop* mode actually changes the mode (you see it in the header, red background also signals about draw mode) but you cannot actually paint. Most probably this happens because you Windows OS prohibits mouse click event from propagating to a subscriber (i.e. your app) while the keyboard key is pressed.
This is done intentionally to protect your typing. To disable this feature go to RegEdit and proceed to the following simple action:

Navigate to `HKEY_CURRENT_USER > Software > Microsoft > Windows > CurrentVersion > PrecisionTouchPad`

Set `AAPThreshold` to `0`

## Vintage Windows Build Systems (yeah! 30+ years of backward compatibility)
In this section let us try to use Visual Studio 6 ('96) and Microsoft Develeper Studio (Fortran Powerstation 4.0) ('94). First things first, simpledraw2D code cannot be compiled with those and will be nevewer rewritten to support ancient compilers. Instead we are going to use libraries that we built with modern MSVC. There seems to be no chance to use modern lib with anicinet compilers so we are going to use modern dll with import library.
## DLL
Not necessay but to me it is easier to create new directory in your `simpledraw2D` root, say `MSVC2015x86`. In this directory you create the project simpledraw2D with Properties->Configuration Properties->General->Configuration type=Dynamic library (.dll). Thurther build is the same as one described in previous section for [static library](#actual-build), with just one difference -- you need x86 versions of GLFW3 and GLEW.
## C/C++
To build C/C++ examples you use Microsoft Visual Studio 6.0.
After you got the library in your Release folder there also will be simpledraw2D.lib. This is an import library that (just as a matter of luck) can be used to link C/C++ sources. So for ccpp/vintage/testcmd.c you just add the full path to import library simpledraw2D.lib to Project->Settings->Link and do the build.
## Fortran
Here we are going to do some really nasty things. The problem is that import library generated by MSVC2015 or even import library generated by Microsoft Visual Studio 6.0 is not supported by MS Dev '94 linker. You can use Microsoft Visual Studio 6.0 liker for your fortran project but is requires significant changes in your acient iconic project, so we are going to do something really nasty to override all incompatibilities.
### Link
First you need to link your project. To do so you need import library with funtions expected by Fortran Powerstation 4.0 linker. Say in the test source you do:

`call fadey_init_(33,33,3)`

The linker then expects to have library that exports _FADEY_INIT_@12 function. This is __std_call convention, plus upper case, plus leading underscore. That is actually no propblem since you can create  simpldraw2D.def in the MSVC2015x86/Release folder with the following contents:

`NAME simpledraw2D.dll`
<br>`EXPORTS`
<br>`FADEY_INIT_@12`

After that you take your cmd wherein you initialize the MS Dev '94 environment with `C:\MSDEV\BIN\FPSVARS.BAT`, and do

`lib /def:simpledraw2D.def /out:fsimpledraw2D.def /verbose /machine:ix86`

Now you got import library *fsimpledraw2D.lib* that is compatible with MS Dev '94 linker. You can set full path to that library in Build->Settings->Link (just as it was done for Microsft Visual Studio 6.0). Doing so for all of exported functions from simpledraw2D you will get... NON-WORKING testcmd.exe binary in your MS Dev '94!
### Link Running Example
I have no idea why the hell it is not working with straiht DEF fie, but at runtime on some initialization step it says that there is no FADEY_INIT_@12 symbol found or something. So please note that ***for linking*** it needs _FADEY_INIT_@12 which you wrote in def ***without*** underscore but got import library ***with*** underscore which is ***required*** to MS Dev linker. Now if you open fsimpledraw2D.lib with somewhat like [HxD](https://mh-nexus.de/en/hxd/), you will see that in the beginning all functions are mentioned ***with leading underscode*** while in the end of that file there is a symbols like FADEY_INIT_@12 i.e. ***wihtout*** underscore. Possibly I am dong something wrong here but the only way I could bring it to work was the following trick:
In the def file you do the following change:

`NAME simpledraw2D.dll`
<br>`EXPORTS`
<br>`FFADEY_INIT_@12`

**NOTE** preceding F in the function name (say it is for Fortran, huh). In Fortran test source all functions are changed to preceding *f* dupliates, like so:

`call ffadey_init_(33,33,3)`

Linker tries to check if import libray fsimpledraw2D.lib provides _FFADEY_INIT_@12, and it does, but at runtime it tries to to search for FFADEY_INIT_@12 which does not exist, but what exist is _FADEY_INIT_@12 so in fsimpledraw2D.lib all symbols in the end of the file should be modified by changing F to _. This does the trick! Now at link time first symbols with leading **_F** are used while at runtime the duplicate capitalized function with only undescore are successfully retreived from DLL.