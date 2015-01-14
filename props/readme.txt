These .props files make it easier for Visual Studio to link to libraries,
so you don't have to go through the project's properties page and fiddle with the linker/includes paths.

YOU WILL NEED TO MODIFY THESE .props FILES TO SUIT YOUR ENVIRONMENT!!!!

In my set up I copy these .props files to where I've installed the libraries (in my case, F:\Cpp).
You will probably need to change VirtualMirror.vcxproj to point to your .props files.
