# swGL
A multithreaded software rasterizer implementation of OpenGL 1.3(ish).

### Screenshots
<p align="center">
  <img src="https://user-images.githubusercontent.com/5739639/33936618-76241022-e000-11e7-8c9c-00dce49acf83.jpg" width="24%" alt="Quake 3 - Q3CTF3" title="Quake 3 - Q3CTF3" /> 
  <img src="https://user-images.githubusercontent.com/5739639/33936619-76436544-e000-11e7-83e8-381c4143cc81.jpg" width="24%" alt="Quake 3 - Q3CTF3" title="Quake 3 - Q3CTF3" /> 
  <img src="https://user-images.githubusercontent.com/5739639/33936620-765ddf14-e000-11e7-8f5c-576b427f487e.jpg" width="24%" alt="Quake 3 - NOR3CTF1" title="Quake 3 - NOR3CTF1" /> 
  <img src="https://user-images.githubusercontent.com/5739639/33936621-76822d9c-e000-11e7-85a5-f366d1122377.jpg" width="24%" alt="Quake 3 - NOR3CTF1" title="Quake 3 - NOR3CTF1" />  
  <img src="https://user-images.githubusercontent.com/5739639/33936623-769f1358-e000-11e7-9e58-ea7e9f841e21.jpg" width="24%" alt="Quake 3 - PUKKA3TOURNEY3" title="Quake 3 - PUKKA3TOURNEY3" /> 
  <img src="https://user-images.githubusercontent.com/5739639/33936624-76b65004-e000-11e7-8a2f-cbe363ab9459.jpg" width="24%" alt="Star Trek - Elite Force II" title="Star Trek - Elite Force II" /> 
  <img src="https://user-images.githubusercontent.com/5739639/33936625-76d4b7b0-e000-11e7-9731-dc3eb1459c53.jpg" width="24%" alt="Star Trek - Elite Force II" title="Star Trek - Elite Force II" /> 
  <img src="https://user-images.githubusercontent.com/5739639/33936626-76f98310-e000-11e7-8293-598596de3470.jpg" width="24%" alt="Half Life 1" title="Half Life 1" />
  <img src="https://user-images.githubusercontent.com/5739639/33936627-7715c714-e000-11e7-919c-a0041f8d3550.jpg" width="24%" alt="Half Life 1" title="Half Life 1" /> 
  <img src="https://user-images.githubusercontent.com/5739639/33936628-773c8dea-e000-11e7-9617-eaeb0fc4c132.jpg" width="24%" alt="Unreal Tournament 99" title="Unreal Tournament 99" /> 
  <img src="https://user-images.githubusercontent.com/5739639/33936629-775d5b74-e000-11e7-820c-b8ff2c836a70.jpg" width="24%" alt="Unreal Tournament 99" title="Unreal Tournament 99" /> 
  <img src="https://user-images.githubusercontent.com/5739639/33936631-777b51e2-e000-11e7-9bc7-7952f3e8b22c.jpg" width="24%" alt="Heavy Metal: F.A.K.K.²" title="Heavy Metal: F.A.K.K.²" />
</p>

### How fast is it
It's reasonably fast. But don't expect rock-solid 60 fps in Full HD resolutions - it ain't gonna happen :).

### How to compile
The project compiles with Visual Studio 2017 just fine - no external dependencies needed. It should also compile with GCC 7.2, but I currently have no Makefile for that.

### How to get it running
For most games it suffices if you put the resulting file `OpenGL32.dll` right into the game directory before starting the game. Games like *Half Life 1* or *Star Trek - Elite Force I* however need some config file modifications before they start using the custom OpenGL driver. For this games you have to rename the `OpenGL32.dll` file you just compiled to something else (like `swGL.dll` for example) - or they will just use the OpenGL driver provided from your graphics card vendor!

##### Half Life 1
Put `swGL.dll` into Half Life 1's `gldrv` directory and modify the file `drvmap.txt` accordingly. Then select the driver from the Options menu in Half Life 1.

##### Star Trek - Elite Force I
Put `swGL.dll` into Star Trek - Elite Force I's game directory. Modify the config file inside the `BaseEF` directory to use the new custom OpenGL driver. To do this, add the command `r_gldriver "swGL.dll"`.

### Supported Games
As far as I can tell Quake 3, Star Trek Elite Force I & II, Half Life 1, Unreal Tournament 99 and Heavy Metal: F.A.K.K.² work pretty well.

### What doesn't work
I can't give you any guarantees that software which isn't listed under "Supported Games" will run without glitches or crashes. Most of the OpenGL 1.3 features are still unimplemented or only partially implemented.

Have fun.
