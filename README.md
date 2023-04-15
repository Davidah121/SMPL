# GLib
 A static C++ library that encompasses many of the tools I have made to make development easier.

<h1>Current features</h1>
<ul>
    <li>Basic File input and output</li>
    <li>Basic XML support</li>
    <li>Partial Gui Support</li>
    <li>Partial SVG support</li>
    <li>Audio Support</li>
    <li>Image Support</li>
    <li>Basic Window Support</li>
    <li>Basic Compression Support</li>
    <li>Mathematical functionality</li>
</ul>

<h1>Planed features and improvments</h1>
<ul>
    <li>Basic OpenGL 4.6 Support</li>
    <li>Basic DirectX 11.0 Support</li>
    <li>Full Gui Support</li>
    <li>Full Linux Support</li>
    <li>Improvements to documentation</li>
    <li>Improvements to Nerual Network setup and training</li>
    <li>Finished SVG support</li>
    <li>Loading more file formats</li>
</ul>

<h1>Requirements</h1>
<p>The library was created using the tools listed below. While the library can be compiled without them, it would require setting up a make file manually.</p>

<h2>Clang</h2>
<p>You can get clang here: <a href="http://releases.llvm.org/download.html">http://releases.llvm.org/download.html</a></p>
<p>This project currently uses clang 12.0.0 with posix thread model</p>

<h2>Ninja</h2>
<p>You can get Ninja here: <a href="https://ninja-build.org/">https://ninja-build.org/</a></p>

<h2>cppPS</h2>
<p>This is a personal project I created to help setup c++ projects.</p>
<p>It was used to setup this project, the batch files, and the .ninja files.</p>
<p>While editing was still needed on a few files, it works nicely.</p>
<p>This is mainly used when you add a new file to update the .ninja files for you.</p>
<p>You can get cppPS here: <a href="https://github.com/Davidah121/cppPS/">https://github.com/Davidah121/cppPS/</a></p>

<h2>Windows SDK</h2>
<p>The windows SDK is needed and can be installed in different ways.</p>
<p>The build files reference the include files and library files through environment variables.</p>
<p>cppPS can setup the environment variables for you however if compiled using Visual Studios, it is not required.</p>
<p>Note that with Visual Studios, modification will be needed to compile and use the files in the 'ext' folders.</p>

