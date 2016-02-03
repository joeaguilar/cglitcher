# cglitcher
======

> Incoporating my C++ image editor into Node.js

A Node C++ addon gives you the power to leverage C++ to supercharge tasks. Put your Node in fast mode!
This addon has a dependency on OpenCV so do this first: `brew install opencv`
you can also follow this guide [here](https://jjyap.wordpress.com/2014/05/24/installing-opencv-2-4-9-on-mac-osx-with-python-support/).


Several files are included under server

1. directoryglitcher.js
2. directoryglitcherrandom.js
3. oneimageglitcher.js
4. socketioglitcher.js
5. utils.js -> Dont use this


+ directoryglitcher takes the arguments: `node /path/to/directoryglitcher.js /path/to/directory/ [effectargs]`
+ directoryglitcherrandom takes the arguments: `node /path/to/directoryglitcherrandom.js /path/to/directory/`
* oneimageglitcher takes the arguments: `node /path/to/oneimageglitcher.js /path/to/image.jpg [effectargs]`
- socketioglitcher takes the arguments: `node /path/to/socketioglitcher` and go to localhost:1337


The effects syntax work like this

1. open square bracket   "["
  1.  number for order   "0"
  2.  colon              ":"
  3.  letter for effect  "A"
   1. comma             ","
   2. Additional arguments following the previous example
2. close square brack    "]"

Examples:

    [1:s,2:a,0:p,4:l]
    [2:s,1:W]
    [0:p]

You can think of the order as holding two cups of water.
When you run an effect, it places a filter on the first cup
and pours water into the second cup. You choose which cup is
the first cup and which is the second. Keep in mind only the
information in the first cup is made into an image.


**Order Map**:

Num | Order 
--- | --- 
0   | 1 to 2
1   | 2 to 1
2   | 1 to 1
3   | 2 to 2


**Effect Map** _nAmEs aRe cAsE SenSiTivE_:

Key | Name 
--- | --- 
a   | zoomth 
b   | zoom_b 
c   | zoom_r 
d   | crunch 
e   | pixelate 
f   | outtashape 
g   | outtacircle
h   | outtasquare 
i   | outtatriangle 
j   | rgb_2_bgr
k   | rgb_2_gbr
l   | rgb_2_grb
m   | rgb_2_brg
n   | wet_reverb
o   | deplace
p   | pixelsort
s   | smosht
t   | posht
u   | fosht
v   | losht
w   | gradient_blend_add_sample
x   | gradient_blend_add_source
y   | gradient_blend_subtract_sample
z   | gradient_blend_subtract_source
w   | gradient_blend_add_sample
x   | gradient_blend_add_source
y   | gradient_blend_subtract_sample
z   | gradient_blend_subtract_source
A   | gradient_blend_add_sample
B   | gradient_blend_add_source
C   | gradient_blend_subtract_sample
D   | gradient_blend_subtract_source
W   | gradient_blend_add_sample
X   | gradient_blend_add_source
Y   | gradient_blend_subtract_sample
Z   | gradient_blend_subtract_source

