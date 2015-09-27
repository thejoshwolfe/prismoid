# Prismoid

2d physics engine with fixed-orientation rigid bodies intended for side-scrolling platformers.

The primary goal of this physics engine is correctness: never let objects pass through walls or other objects.
For example, firing two bullets directly toward each other should cause the bullets to collide rather than pass through each other,
and a stack of boxes should never occupy less height than the sum of the heights of the boxes.
This physics engine approaches the issue by plotting the intersection of moving 2d objects as 3d objects in spacetime.
With this model collisions are intersections of prismoid polyhedra.

## Status

Not cool yet.

## How Do I Run It?

### Ubuntu

add ppa for rucksack:

```
sudo apt-add-repository ppa:andrewrk/rucksack
sudo apt-get update
```

install dependencies:

```
sudo apt-get install librucksack-dev rucksack libsdl2-dev libsdl2-ttf-dev libpng12-dev
```

build:

```
make
```

run:

```
./build/native/prismoid
```

run with gdb from the command line:

```
gdb -iex "add-auto-load-safe-path ./" -ex run -ex quit ./build/native/prismoid
```

## Eclipse Environment Setup on Linux

Josh develops this project in Eclipse on Linux, but of course it's optional.
These instructions were last updated with Eclipse Luna.

Search Google for "eclipse luna" to get to right Eclipse downloads page (there are lots of wrong download pages.).
Avoid the download for Java; get the one for C/C++ (if you don't see one for Java, you're probably on the wrong page.).
The description should confirm that it's for Linux.
You should get a binary archive on the order of 200MB with an executable at the top level called `eclipse`.

```
sudo apt-get install default-jdk
```

In eclipse, File -> New -> Project... -> C/C++ -> Makefile Project with Existing Code.
Browse to this directory.
**Click the "Linux GCC" toolchain.**

Now you should be able to open `main.cpp` and see no error or warning annotations.
You should be able to select a system include (`#include <...>`), and F3 to see its source.

### Optional Tweaks

Project -> Properties -> C/C++ Build -> Behavior Tab.
Uncheck "Stop on first build error".
Check "Enable parallel build".
Check "Build on resource save (Auto build)".
Blank out the textboxes under "Make build target" that say "all".

Run -> Debug Configurations... -> C/C++ Application -> New launch configuration.
Main Tab: Project -> Browse... -> select the one.
C/C++ Application: `build/native/prismoid`
Debugger Tab: Uncheck "Stop on startup at".
GDB command file: `${workspace_loc:prismoid}/.gdbinit`

Project -> Properties -> C/C++ General -> Formatter -> Edit...
Indentation Tab: Indentation policy: Spaces only.
Check "Statements within a 'switch' body".
Line Wrapping Tab: Maximum line width: 9999.
Default indentation for wrapped lines: 1.

Project -> Properties -> C/C++ General -> Code Analysis.
Use project settings.
Uncheck Potential Programming Problems/No Return.
