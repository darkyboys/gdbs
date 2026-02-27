# Goblin's Demonic Build System
An open source build system for C/C++ projects with powerful dynamic threaded executer, Making it capable to build almost anything insanely fast. Unlike traditional build systems, Everything is powered by a simple configuration Language , written entirely in C++ with STL Only, Giving it enormus speed advantage and flexiblity.

<img src="art/gdbs vs make.png" alt="GDBS Vs Make">

## Installation
Installing `GDBS` is really straightforward, Just follow these commands.

### For First Time
```bash
git clone https://github.com/darkyboys/gdbs
cd gdbs
chmod +x build.sh
./build.sh
sudo mv gdbs /usr/local/bin
```

### For Those Who Already Have GDBS
```bash
git clone https://github.com/darkyboys/gdbs
cd gdbs
sudo gdbs . -make -install
```

## How To Use
`GDBS` Comes in 2 parts. `CLI` and `Core`.
The `CLI` is used by the programmer to access the `GDBS` without writing any C++ program manually. `Core` is used by the programmer to access the `GDBS` via programming.

### CLI
`CLI` stands for **Command Line Interface**, Something which you can use to talk to the `Core` without writing any C++. Understanding the `CLI` is crucial because a lot of the time you won't be talking to the `Core` directly.

Everything in `CLI` is a flag used to tell `GDBS` what to check. 
#### Flags
 - *--version / -v* : These flags will show you the `GDBS` current version you have.
 - *--thread / -t [number]* : These flags are used to tell the `GDBS` about how many cores of the cpu do you want to use for the compilation, By default `GDBS` will always use all the cores of your system for the best performance.
 - *--show-commands / -sc* : These flags tell the `GDBS` to not only log about what file is currently being built but also show the commands being used to compile that file.
 - *--clean / -c* : These flags tells the `GDBS` to rebuild the entire cache and rebuild your entire project from exact zero.
 - *--noincrement / -ni* : These flags tells the `GDBS` to skip the incremental build and build your entire project no matter what while keeping all the previous configs safe.
 - *--cache-dir / -cd [number]* : These flags tells the `GDBS` to use a different cache directory. This is super important if you are using dual stage builds where at first gdbs builds objects and then you use a different build file to build binaries. By default gdbs uses `.gdbs-cache` directory to store it's cache. But for dual stage builds make sure to use different directories to prevent overwriting each other's build cache. Corrupted cache can result in unnecessary rebuilds.
 
**Usage**
```bash
 > gdbs some flags
 > gdbs --clean
```

**Other Flags**
Aside the standard flags `GDBS` also have some other flags, These are used to provide `GDBS` the path to the `build.gdbs` or are specified inside the `build.gdbs`.

Anything starting without a `-` symbol will be taken as the path to `build.gdbs` so be careful while typing flags.

---

#### Build File
`GDBS` can only know what to do if you pass a `build.gdbs` file to it with the rules specified for what to build. Those rules will then be used to determine what to do.

Before you learn about any rules you need to know the basics of the (https://github.com/darkyboys/hell6.99mo)[H699] Basics. If you don't know then you can always visit here (https://github.com/darkyboys/hell6.99mo)[H699]. Although this guide will cover very basics of H699 Cofig Format (The format you will be writing the `build.gdbs` in) it's still worth checking the official guide.

#### Basics of H699
`H699` is a fast and Open Source configuration format designed for C++ projects, The reason it was chosen as the DSL of `GDBS` was because not only it is super fast to execute but also it have a really forgiving nature for the configs, Essentially making the programmer happy all the day about getting less possible errors, Also it's super easy to read and write by humans and can be also modified by programs. So with that's all let's dive deep into the syntax.


##### Scopes
`Scopes` in `H699` are used to define the entry for a configuration, They can hold any amount of properties also other scopes. 

*Example of a scope*
```config
scope:
    property
    property2
    property3
    anotherScope:
        anotherProperty
```

Now scope holds all the properties alongside with the `anotherScope` and `anotherScope` holds it's own properties while being inside the `scope`. They can be accessed via `scope.property` in programming but you don't have to worry about all of that because that part is handled by the `Core` not you.

##### Properties
`Properties` are essentially just the key/value pairs for holding values for the `GDBS Core` to understand and execute, For `GDBS` You only need to know 2 datatypes of the properties. `String` and `Arrays`.

**Strings**: They are written unside `""`(Double Quotes) and are used to hold some text information.
**Arrays**: They are the collection of strings seperated by a `,` inside `[]`.

*Example of properties*:
```config
property = "Some string value"
anotherProperty = ["some", "array", "value"]
```

> **Note** : This guide only covers what is needed for `GDBS`, It's still worth checking the [official documentation](https://github.com/darkyboys/hell6.99mo) for the project `H699`.

--- 

#### Important Part About GDBS
Everything we need to do is done inside the `build.gdbs` file with some scopes and properties. There are some standard scopes and some standard properties and rest except them are just used to describe the source files.


This part was removed from the gdbs 1.6

```txt
Also GDBS Build system builds in `cycles` means that if the build was interrupted during the build then the entire `cycle` will rebuild from the start to ensure the correctness in the build and eliminate all the random builds errors. 

A `Cycle` means the total files who were supposed to be built.
Let's suppose that you have 4 files.
a.c
b.c
c.c
d.c

Now if you are building for the first time then all the files from `a.c` to `d.c` must build, So the current `cycle` will contain all the files from `a.c` to `d.c` and build them. However if the build was interrupted then the cycle will remain incomplete so the next time you build the project again the build system will restart that entire cycle again to rebuild from `a.c` to `d.c`. This is to ensure that all the builds were successfully performed with all the dependencies linked them them so no random error of starting the builds from where it was left. 

Once the Build `Cycle` is completed then nothing will be rebuilt and only the changed files / dependencies will be rebuilt with a new `cycle`.

For example:-
We had `a.c` to `d.c` now let's suppose that `c.c` depends on `b.c` and `b.c` changed.
So the moment you run the GDBS Again it will only build the new `cycle` and new `cycle` will store both `c.c` and `b.c` to be rebuilt because `c.c` relies on `b.c` so any change to `b.c` will also affect the `c.c` and that's why both will be built. But again interrupting a `cycle` means restarting the full rebuild for that particular `cycle`, Means if interrupted then `b.c` and `c.c` will rebuild again no matter what because GDBS Saves the timestamps only after the `cycle` completes.

 > Note: This is not a bug so please do not report this in issues. This is an intended behaviour because GDBS Ensures correctness over UX. 
```

The `cycles` were removed because they were replaced by updative builds. In updative builds the `GDBS` (1.6+) only builds what's actually needed so in case if your previous compilation had some errors then unlike `cycles` where the build system would build everything in that `cycle`, Updative Builds will only build the remaining files which had errors. But again remeber that time stamp update is only performed after everything is finished means if you have interrupted the build then the timestamps won't be updated means it will still build what you were building again (Not the whole build).

So let's say that we have 4 files:
```bash
1.cc
2.cc
3.cc
4.cc
```
You ran `GDBS` but `2.cc` had an error so it was not built but `1.cc` , `4.cc`, `3.cc` were built in that case the `GDBS` Will warn you about the `2.cc` . But the next time you run `gdbs` it will only build `2.cc`. How ever if you interrupted the `GDBS` while it was building then it will still build all the 4 files the next time you run it because the time stamps weren't updated.

So if we had `1.cc` already built but errors for the opther files and the next time we ran it started building `2.cc`, `3.cc`, `4.cc` and you interrupted it in mid build then it will build all of those 3 files again no matter what because timestamps for `1.cc` were registered but not for them.

So is this a bug ? And the answer is. No. This is not a bug this is very intentional because if the time stamps were actually updated after the compilation commands for a file ended then first it would trigger so many file system calls like 100 times for a 100 file long project which is a massive overhead and can slow down the compilation and also updating time stamps after builds can confuse the build system the next time you try to rebuild so it might become buggy in edge cases so in order to handle this GDBS only updates all it's time stamps at once inside the memory and after they are updated it writes them directly to the cache at once so we get extremely low overhead and closed the door for timestamp related compilation bugs.

 > Note: Again saying. GDBS is a build system for correctness and speed in threading not the build system for interrupt level time stamp managment.

Also remember that GDBS is a cache sharing build system means it will share it's cache relative to the directory.
Let's suppose we have
```bash
/dir1
 - build.gdbs
build.gdbs
```
Now if we execute `gdbs .` from the root directory then it will create it's cache in the root directory inside the `.gdbs-cache` directory, If we run it from `dir1` then it will build the cache inside it, How ever if you run `gdbs dir1` from the root directory then the gdbs will use the root directory's cache for `dir1` means it will look at the `build.gdbs` of `dir1` from the `root` fs path context. So be careful while writing build files in different directories because the context matters.
---

#### Writing build.gdbs
While writing `build.gdbs` we have to tell the `GDBS` about what file to compile and for that we opens a scope with the path to that file and fills it with some properties so that the `GDBS` can know what to do exactly.

*Example*
```bash
myfile.cpp:
    out = "Hello world.bin"
```

*Here we described that there is a file named `main.cpp` which we want to compile as `Hello world.bin"*

There are some standard scopes which we can not describe as a file and we will soon talk about them but as of now let's talk about the standard properties we can use to describe something.

##### Standard Properties
These are the implemented `Standard Properties` inside the `GDBS Core` which tells exactly what to do with a C++ source file.

##### Types of Known properties
These are in 2 types:
- Static
- Dynamic

**Static** propertys are those who only accepts a single type eg string.
**Dynamic** propertys are those who supports more than one types. eg a property can store both string and array.

*Let's start learning these propertys and write our first recipe*

##### out - Static string property
`out` is a property which tells the GDBS the name of your executable, This is optional as if you don't pass it the GDBS is intellegent enough to fetch the name from your source file's name without `.any extension`.

##### bin - Static string property
`bin` is a property which tells the GDBS the name of the directory where all the compiled binaries will be stored , Default is `bin`.

##### compiler - Static string property
`compiler` is a property which tells the GDBS the compiler you want to use to compile all your source files , Default is `g++`.

**Dynamic Array Type** These types supports _rem and _add as suffix to remove anything or add anything to them respectively.

##### compiler_arguments - Dynamic string property
`compiler_arguments` is a property which tells the GDBS the arguments you want to give to your compiler , Default is empty.

##### compiler_parguments - Dynamic string property
`compiler_parguments` stands for the `compiler_primary_arguments` which is a property giving arguments to your compiler before the file name so if you want to make let say object files then say `compiler_parguments = "-c"`.

##### system - Static string property
`system` is a property which tells the GDBS the command you want to execute when a target is built , This won't run if the target is skipped in incremental builds , Default is empty.

##### psystem - Static string property
`psystem` is a property which tells the GDBS the command you want to execute when a target is built , This won't run if the target is skipped in incremental builds , The only difference in between this and system is that this runs before the compiler and system runs after the compiler which is why it stands for primary system, Default is empty.

##### pkg_in - Dynamic String / Array Property
`pkg_in` is a property which tells the GDBS to look for pkg-config packages in your system if the packages aren't found the GDBS will throw error for package not found and hault the compilation. If you want a single package use string othervise for multiple packages feel free to use array.

##### include - Dynamic String / Array Property
`include` is a property which tells the GDBS to mark include directories for the compiler via `-I` flag. This also allows single string and arrays of strings.

##### lib - Dynamic String / Array Property
`lib` is a property which tells the GDBS to mark libraries for the compiler via `-l` flag. This also allows single string and arrays of strings.

##### combine - Dynamic String / Array Property
`combine` is a property which tells the GDBS to combine multiple C++ source files together to form a single binary , Remember incremental build will only apply to main binary not on combines ones , This also allows single string and arrays of strings.

##### compare - Dynamic String / Array Property
`compare` is used to tell the GDBS that it needs to register a file inside it's cached data set made for the project , Means if that file changes then it will trigger all those files to be rebuilt which relies on that file. This is specially usefull for the `Single Header Libraries` like `stbi_image.h`, `cpp-httplib` or even the `H699`.

---

##### Standard Scopes
These scopes are used to do something really important with the build process and they are very limited because of their functionality.

##### global
The `global` scope can take any property of a C++ source file except for the `out` and make those property globally accessible means all the source file scopes will automatically inherit everything `global` have except for those which the programmer have overwritten for example you might want to keep using `gtk3` package for all your source files so you can specify that in the `pkg_in` property inside `global` scope and all the C++ source files will automatically inherit means you no longer have to write `pkg_in` for every C++ source file's scope, But if you don't want that package for some `cli` files then you can simply overwrite the `pkg_in` for that file.

**Example**:
```bash
global:
    pkg_in = "gtk+-3.0"

window.cpp: out = "window.bin"
editor.cpp: out = "editor.bin"
cli.cpp:
    out = "cli.bin"
    pkg_in = "" # Overwritten
```

##### call
`call` is a really important scope because it can only take 2 properties `command` and `commands` the `command` is the static string and `commands` is the static array of string. Those properties are used to add some system commands which will execute before any compilation happens.

*Example*
```bash
call:
    command = "echo \"starting the build...\""
    commands = [
        "ls",
        "echo \"Listed all the directories!\""
    ]
```

*Here all those commands will execute before any compilation happens. Also you don't have to write both of them , You can just write one of them and you will be fine.

##### callback
`callback` is a really important scope because it works exactly like `call` even takes same arguments but only executes once the compilation is finished.

*Example*
```bash
call:
    command = "echo \"starting the build...\""
    commands = [
        "ls",
        "echo \"Listed all the directories!\""
    ]

callback:
`   command = "echo \"compilation finished.\""
```

##### onchange
`onchange` is another important scope because it also takes same properties as `call` and `callback` but will only execute those commands if anything is changed and after the compilation is finished.


*Example*
```bash
onchange:
    command = "echo \"Something did compile.\""
```

##### ontriggerchange
`ontriggerchange` is a subtle one but it works same as `onchange` but executes the commands before the compilation starts.

*Example*
```bash
ontriggerchange:
    command = "echo \"Something is about to be compiled.\""
```
 
##### need property with onchange and ontriggerchange
`need` property with `onchange` and `ontriggerchange` scopes is used to execute the commands even if a file or a directory don't exists. For example you want to run something only if something is changed but also if a file or directory don't exists. This is ideal for installation scripts and even building.

Example:
```bash
ontriggerchange:
    command = "echo \"This will always run until the abc file exists and nothing changes\""
    need = "abc"
```


##### afterchange
`afterchange` is similar to `onchange`, It also takes the `command` and `commands` properties but it is used to run a command after the entire build cycle is completed. Unlike `onchange` and `ontriggerchange` which executes before the timestamps are updated. This was mainly introduced for highly complex builds which might need to call the build system multiple times because of cache sharing.



##### cli
This is one of the most important additions to the `GDBS`, Because this directly allows the programers to take the input fromt the `CLI` and execute some commands for those inputs. This one can take any property as the argument and a dynamic `string/array` value as the commands to execute.

*Example*
```bash
cli:
    -install = "cp myapp /usr/local/bin"
    -make = [
        "echo \"Now the directory looks like\"",
        "ls"
    ]
```

*Here if the `GDBS CLI` gets any of those arguments `-install` or `-make` it will trigger the commands respectively. In fact multiple arguments can be given at once but remember that arguments from the `CLI` will put the `GDBS` into the `CLI Mode` means it will only execute those arguments (If given) and will not build anything. If only standard arguments were passed then it will build the project normally.*

---

##### Runtime Manipulators
Runtime Manipulators are those features of gdbs which directly manipulates the runtime of the build system while building something. This manipulation can include modification of the build file inside the memory without touching the physical disk so your main file remains untouched & intact. All the changed happens into the memory.

##### expanded_compare & expanded_combine
Let's suppose that you have so many files in a directory that you want to compare or combine to build the binary of your project. If the files were like 10 or 20 then you could literally write them in combine inside `global` and let the project inherit them. This works fine for small to mid sized projects how ever for bigger projects where a directory can contain 100s of files to be compared or combined manually writing them becomes very impractical. So here comes the `expanded_compare` and `expanded_combine`. They can take a string value or an array value of strings which will contain the path to the directory where your files are that you want to compare / combine.

Example:-
Directory structure
```bash
dir/
 - a.cpp
 - b.cpp
 - c.cpp
```

Now you want to combine them automatically without manually writing their names in `combine` so you can just say.

```bash
myfile.cc:
    expanded_combine = "dir"
```

This will automatically create a combine which will hold the names of all the files in the `dir` means you didn't had to manually write them.

Compare is same just with different naming.

```bash
myfile.cc:
    expanded_compare = "dir"
```

Similarly with multiple directories:
```bash
dir/
 - dir2/
 - - anotherfile.cpp
 - - anotherfile2.cpp
 - a.cpp
 - b.cpp
 - c.cpp
```

You can just say:
```bash
myfile.cc:
    expanded_compare = [
        "dir",
        "dir/dir2"
    ]
```

It will automatically do the work of creating compare for you for each file inside the directories. Combine is same just different naming.

If you don't wanna include a particular file during the expansion then you can use the `expanded_compare_ignore` and `expanded_combine_ignore` respectively. They can take both a string for a single file and an array of strings for multiple files.

So let's suppose you wanna expand to link the object files but the object files also contains the duplicate files. Now you want to ignore them to get rid of linking errors then you can use the `_ignore` suffix respectively.

 > Note: They do creates new values but your physical file remains untouched! Everything happens in the file stored in the memory.

##### Expanding source files
Expanding source files means listing the source files in a directory without actually writing their names in the build config.

Let's suppose that you are working on a big project with have thousands of files in the `src` directory and you want to compile them, Well that will be devastating if you try to write their names manually. Sure `global` can help in config inheritance but writing each file's name is very impractical for bigger projects. So here comes the `GDBS's` **Expanding source files** feature. Which let's you automatically write the file names located in a directory with the `out` property in the config without touching your physical file. You just write the path and end it with `/*` and the build system will automatically expand it.

Example:-
Suppose you have a directory `src` containing a,b,c, files to compile so instead of writing their names manually you can just say `src/*` and make it a *scope* by ending it with `:` symbol then the build system will replace the `src/*` with `src/a` , `src/b`, `src/c`. Cool isn't it ? You have to write very less now. 

So you can literally write
```bash
src/*:
```
in your config and the build system will expand it automatically and the build system will also correctly parse the output names. If the files have a file extension then that file extension will be replaced by `.bin` and if the files don't have a file extension then by default the `.bin` extension will be added in the last of the out property.

So if the files were `a.cc` , `b.cc`, `c.cc` then they will become `a.bin` , `b.bin`, `c.bin`

But you can change this with the `ext` property of file expanding. `ext` takes just a string value and it directly tells the build system to what file extension to use.

Means if you write:
```bash
global: compiler_parguments = "-c"
src/*: ext = "o"
```

And your project structure was:
```bash
src/
 - a.cc
 - b.cc
 - c.cc
```

So that will become this in the memory
```bash
global: compiler_parguments = "-c"
src/a.cc: out = "a.o"
src/b.cc: out = "b.o"
src/c.cc: out = "c.o"
```

Again repeating! All this happens inside the file stored in the memory not the actual build file you wrote yourself.

Now while expanding files you might now want some files to be compiled like `.png` or `.o` as they can cause errors so you have to tell the `GDBS` that while expansion it should not add those files. 

This can be done in 2 ways.

**With `only`**: `only` is a property like `ext` for expansion which can take both `strings` and `arrays` to actually store the file extensions. `GDBS` Will only add the files with extensions defined in this properties.

Example:-
I only want `.c` and `.cpp` files to be expanded from the `src` directory so i can write.
```bash
src/*:
    only = [
        "c",
        "cpp"
    ]
```

This is a way if you want to remove a bulk of files from being added inside the memory during the expansion. But if you just want to remove a few specific files then you can use `ignore`.

**With `ignore`**: `ignore` is a property like `only` as it can take both `string` and `array` values but it's used to tell the `GDBS` Exactly what files you do not want to compile during expansion.

Example:
Let's say that i don't want to include a file `main.o` during expansion so i can just say
```bash
src/*:
    ignore = "main.o"
```

Always remember that what ever path you are going to give to ignore must be relative to the directory you are expanding in our case it was `src` so instead of writing `src/main.o` we just wrote `main.o`.
--- 

### Real Life Example Of A build.gdbs
One of the best real life example of a `build.gdbs` can be 
```bash
main.cpp:
```
*Yes that's it. It will compile the `main.cpp` file with the `g++` compiled and already include the `include` directory by default.*

Another really good example can be the `build.gdbs` of the project `GDBS` itself. Yes the build system can build itself. And the official build file for the `GDBS` isn't a single file. It's splitted accross directories for crystal clear maintainance.

`build.gdbs`
```bash
# This is the official build file for the gdbs build system's core
# This must be run from the root of the repo

cli:
    -clean = "rm -rf bin objects .gdbs-cache "
    -install = "sudo mv bin/gdbs /usr/local/bin"

onchange:
    command = "gdbs link"
    need = "bin/gdbs"

global:
    compiler_parguments = "-c"
    bin = "objects"
    include = [
        "include",
        "src"
    ]

# Build the libraries first
src/implementations/core.cc: 
    out = "core.o"
    expanded_compare = "src/components"

src/implementations/executer.cc: out = "exec.o"
src/implementations/fs-utils.cc: out = "fsut.o"
include/ConsolePrint/ConsolePrint.cc: out = "conp.o"

# Make the main source files objects
src/gdbs.cc: out = "gdbs.o"
    
```

`link/build.gdbs`
```bash
# This file must only run automatically by the gdbs build system from the root of the project
objects/gdbs.o:
    combine = [
        "objects/conp.o",
        "objects/core.o",
        "objects/exec.o",
        "objects/fsut.o"
    ]
    out = "gdbs"
```

*Try to figure out why are those written like they are and you will also find the reason why you might also prefer `GDBS` for your C/C++ projects. 😃*

---

## Contributing ?
Checkout the [CONTRIBUTING.md](CONTRIBUTING.md).

## Found Issues ?
Checkout the [ISSUES.md](ISSUES.md)

## Again! Thanks for reading. Hope you have the nice day (TIME 8:52PM IST, 11/12/25)
