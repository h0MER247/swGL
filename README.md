# swGL
A (reasonably fast) multithreaded software rasterizer implementation of OpenGL 1.3(ish).

### Screenshots
<p align="center">
  <img src="https://user-images.githubusercontent.com/5739639/33936619-76436544-e000-11e7-83e8-381c4143cc81.jpg" width="24%" alt="Quake 3 - Q3CTF3" title="Quake 3 - Q3CTF3" /> 
  <img src="https://user-images.githubusercontent.com/5739639/33936620-765ddf14-e000-11e7-8f5c-576b427f487e.jpg" width="24%" alt="Quake 3 - NOR3CTF1" title="Quake 3 - NOR3CTF1" /> 
  <img src="https://user-images.githubusercontent.com/5739639/33936624-76b65004-e000-11e7-8a2f-cbe363ab9459.jpg" width="24%" alt="Star Trek - Elite Force II" title="Star Trek - Elite Force II" /> 
  <img src="https://user-images.githubusercontent.com/5739639/33936625-76d4b7b0-e000-11e7-9731-dc3eb1459c53.jpg" width="24%" alt="Star Trek - Elite Force II" title="Star Trek - Elite Force II" /> 
  <img src="https://user-images.githubusercontent.com/5739639/33936626-76f98310-e000-11e7-8293-598596de3470.jpg" width="24%" alt="Half Life 1" title="Half Life 1" />
  <img src="https://user-images.githubusercontent.com/5739639/33936627-7715c714-e000-11e7-919c-a0041f8d3550.jpg" width="24%" alt="Half Life 1" title="Half Life 1" /> 
  <img src="https://user-images.githubusercontent.com/5739639/33936628-773c8dea-e000-11e7-9617-eaeb0fc4c132.jpg" width="24%" alt="Unreal Tournament 99" title="Unreal Tournament 99" /> 
  <img src="https://user-images.githubusercontent.com/5739639/33936629-775d5b74-e000-11e7-820c-b8ff2c836a70.jpg" width="24%" alt="Unreal Tournament 99" title="Unreal Tournament 99" /> 
  <img src="https://user-images.githubusercontent.com/5739639/33936631-777b51e2-e000-11e7-9bc7-7952f3e8b22c.jpg" width="24%" alt="Heavy Metal: F.A.K.K.²" title="Heavy Metal: F.A.K.K.²" />
  <img src="https://user-images.githubusercontent.com/5739639/35512540-5be18ec2-0500-11e8-9d39-ad44d468f4e3.jpg" width="24%" alt="Serious Sam -  The First Encounter" title="Serious Sam -  The First Encounter" />
  <img src="https://user-images.githubusercontent.com/5739639/35512591-9499425a-0500-11e8-84f5-a23dbe748f9e.jpg" width="24%" alt="Serious Sam -  The First Encounter" title="Serious Sam -  The Second Encounter" />
  <img src="https://user-images.githubusercontent.com/5739639/35512933-aa06cd0a-0501-11e8-89c8-5134b5f22d8d.jpg" width="24%" alt="Doom 3" title="Doom 3" />
</p>

### How fast is it
Reasonably fast for a software rasterizer :). You'll get good framerates if you stick to 640x480 or 800x600 resolutions.

### How to get it running
For most games it's enough to put the resulting file `OpenGL32.dll` right into the game directory. Games like *Half Life 1*, *Star Trek - Elite Force I* or *Heavy Metal F.A.K.K.²* however need some config file modifications before they use the custom OpenGL driver. For this games you have to rename the `OpenGL32.dll` file you just compiled to something else - like `swGL.dll` for example - or they will just use the OpenGL driver provided from your graphics card vendor!

##### Half Life 1
Put `swGL.dll` into Half Life 1's `gldrv` directory and modify the file `drvmap.txt` accordingly. Then select the driver from the Options menu in Half Life 1.

##### Star Trek - Elite Force I / Heavy Metal F.A.K.K.²
Put `swGL.dll` into the game directory. Modify the config file inside the `BaseEF` / `fakk` directory to use the new custom OpenGL driver. To do this, add the command `r_gldriver "swGL.dll"`.

### Supported Games
As far as I can tell Quake 3, Star Trek Elite Force I & II, Half Life 1, Unreal, Unreal Tournament 99, Heavy Metal: F.A.K.K.², Serious Sam - The First & Second Encounter and Warcraft III work pretty well for the most part. Homeworld, Enclave and Doom 3 work okay. Blade Of Darkness isn't really playable as I haven't implemented display lists. I haven't tried other titles so far.

Have fun.
