Phong-Texture-Bump Readme
=========================

- Ben Houston [ plash / AZuRe ]
- Friday, March 14, 1997
- e-mail: 'ben@exocortex.org'

Here is the results of my first attempt at phong texture
bump mapping.  I'm moving from BC4/TASM to Watcom and my next
engine won't be so limited as this thing.  Although this engine
is fairly limited it is fairly fast because of the fixed point
math, fast polygon tracer and dirty rectangle animation.  You
will find alot of weird things in this source because I started
to run out of space in the data segment and I had to reuse
portions of it.  Let me tell you that phong-texture-bump is not
ment to be done under 540 kb using a small memory model in 16 bit
real mode.

To learn about some of the techniques used in this source it
would be best to read other people's work that is more suited to
learning from.  The texture mappers used are all based on a doc
called FATMAP.TXT written by MRI.  The fixed point rotation is
based on Rex Deathstar's EMPHONG4 archive.  Rex also has an
example of how to enviroment map and it the technique on which my
source is based.  The bump mapping method is my own as well as
the phong map calculations. The phong palette calculations are
not totally correct but fairly close. The technique used to
bumpmap causes bumps to appear larger then perpendicular to the
viewer than when the bump is facing the viewer
head on.  The distortion has to do with the non uniform phong
map. Most of the code was written when I had way too much coffee
so please ingore that the libraries are called "nothing comes
close" (NCC) ;).

Enjoy the code, if you do use it give credit or your
plagerizing (taking credit for work that's not your own).  I'm
always happy to get mail discussing techniques or whatever.  By
the way I am looking for one talented musician to help with a
major big demo.

PS. I've added reflective metal shading and plastic phong
shading jsut for the fun of it.  The inner loops of almost all
the triangle drawers are totally shit!  I'm moving to Watcom so
there is no point optimizing realmode code.  Real mode sucks shit
and I'm done with it!!!

PPS. I Want Greets Too!

Release Contents
========================

```
+-- readme.txt          this file
+-- bin                 archive containing working demo
|   +-- ptb_demo.exe   the DOS16 x86 demo executable (small eh?)
|   +-- azure.raw      background (320x200)
|   +-- bumpmap.raw    star heightmap (256x256)
|   +-- texture.raw    star texture (256x256 32clr)
|   +-- texture.pal    texture palette (32 clrs used)
|   +-- refmap.raw     metal reflection map (grayscale)
|   +-- mask.vtx       vertices for object
|   +-- mask.nrm       vertex normals for object
|   +-- mask.fct       facet indices for object
|   +-- mask.txr       vertex texture coord for object
+-- src                 archive containing all source code :)
    +-- ptb_demo.cpp   main c++ source for demo
    +-- ncctypes.h     types header for ncc library
    +-- nccgraph.*     graphics functions
    +-- ncctri.*       alot of triangle drawers
    +-- ncccolor.*     palette quantization
    +-- nccmisc.*      just simple crap
    +-- azr_ptb.ide    project file for Borland C++ 4.0
```

Greetz:
=======

- Mundane - in no particular order.
- Asch - that girl has driven me insane
- Olivier - am i bothering you?
- xyz - try to use some sentence structure :).
- TimJ/Vertigo - a guy who know's his phong.
- Phantasm - how goes the engine?
- Cyclone - object orientation overload.
