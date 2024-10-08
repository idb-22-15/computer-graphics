#include "colors.inc"    

camera {
  angle 80
  location <3, 2, -3>
  look_at 0
  scale 1.5
}

background{ 
  rgb<0.2,0.2,0.4>
}

light_source {
  <10,30,-3> 
  color White
}

// Basic isosurface
isosurface {
  function { x*x + y*y + z*z - 1 }
  contained_by { box { -1.5, 1.5 } }
  max_gradient 5
  accuracy 0.001
  pigment { color Red }
}

// Isosurface with different settings
isosurface {
  function { sin(x*3) + sin(y*3) + sin(z*3) }
  contained_by { box { -2, 2 } }
  max_gradient 3
  accuracy 0.01
  pigment { color Green }
  translate <3, 0, 0>
}

// Isosurface with space transformation
isosurface {
  function { x*x + y*y + z*z - 1 }
  contained_by { box { -1.5, 1.5 } }
  max_gradient 5
  accuracy 0.001
  pigment { color Blue }
  scale <1, 0.5, 1>
  rotate <0, 45, 0>
  translate <-3, 2, 0>
}

// Isosurface composed of multiple operations
isosurface {
  function {
    f1 = x*x + y*y + z*z - 1
    f2 = (x-1)*(x-1) + (y-1)*(y-1) + (z-1)*(z-1) - 0.5
    min(f1, f2)
  }
  contained_by { box { -2, 2 } }
  max_gradient 5
  accuracy 0.001
  pigment { color Yellow }
  translate <0, -2, 0>
}

// Keep the existing objects
cylinder {
  0, 10*x, 0.03
  pigment { Red }
}

cylinder {
  0, 10*y, 0.03
  pigment { Green }
}

cylinder {
  0, 10*z, 0.03
  pigment { Blue }
}  

#declare ground = prism {
  -1,0, 4
  <8,8>,
  <-8,8>,
  <-8,-8>,
  <8,-8>
  texture {
    pigment { wood }
    finish { phong 1 }
  }
}

object { ground }
