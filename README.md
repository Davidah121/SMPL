# SMPL
 A static C++ library that encompasses many of the tools I have made to make development easier.

<h1>Current features</h1>
<ul>
    <li>Basic File input and output</li>
    <li>XML support</li>
    <li>Partial Gui Support</li>
    <li>Partial SVG support</li>
    <li>Audio Support</li>
    <li>Image Support</li>
    <li>Basic Window Support</li>
    <li>Basic Compression Support</li>
    <li>Basic OpenGL 4.6 Support</li>
    <li>Basic DirectX 11.0 Support</li>
</ul>

<h1>Planed features and improvments</h1>
<ul>
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

<h2>Additional Notes:</h2>
<p>There are libraries that are not fundamentally required to compile as you can ignore them using Preprocessor defines.</p>
<p>Without these though, you may find certain classes no longer work or performance may be lacking.</p>
<p>For instance, not using -mavx and setting the preprocessor OPTI to a value above 0 can result in the library not using intrinsics instructions.</p>
<p>OpenSSL is required to use SSL. This includes SecureSockets and HTTPS.</p>
<p>pthreads is required on linux.</p>

