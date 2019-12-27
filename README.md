# GLib
 A static C++ library that encompasses all of the classes I have made.

<h1>What Changed</h1>
<p>Redone using a simple directory structure, <br>using Clang as the compiler, <br>and ninja as the build system along with some batch code to make my life easier.</p>

<h1>Clang</h1>
<p>Clang is a c++ compiler that can behave like the GCC compiler g++ or Microsoft compiler cl depending on what command you call. While it does not perform as well, it is more portable allowing me to create libraries for different operating systems easily.</p>
<p>Clang is also better on windows than the GCC ports as they support filesystem in the c++17 standard which MinGW suffered with for me.</p>
<p>You can get clang here: <a href="http://releases.llvm.org/download.html">http://releases.llvm.org/download.html</a></p>
<p>This project currently uses clang 9.0.0</p>

<h1>Ninja</h1>
<p>Ninja is a build system simlar to Make however, it is cross platform and relatively simple to setup and use</p>
<p>While it is recommended to use CMake with Ninja, for this project, it was not difficult to setup the .ninja files</p>
<p>Using Ninja allows you to build only what is neccessary when building an Executable or Library file while maintaining dependencies on other files.</p>
<p>While the setup for building the intermidiate object files is simple, I found that using a batch file to run ninja first and then build the .exe file was simpler than putting that in the .ninja file.</p>
<p>This will likely change later.</p>
<p>You can get Ninja here: <a href="https://ninja-build.org/">https://ninja-build.org/</a></p>

<h1>cppPS</h1>
<p>This is a personal project I created to help setup c++ projects.</p>
<p>It was used to setup this project, the batch files, and the .ninja files.</p>
<p>While editing was still needed on a few files, it works nicely.</p>
<p>This is mainly used when you add a new file to update the .ninja files for you.</p>
<p>You can get cppPS here: <a href="https://github.com/Davidah121/cppPS/">https://github.com/Davidah121/cppPS/</a></p>

