#include "colors.inc"    

camera {
  location <0, 2, -3>
  look_at  <0, 1,  2>
}


// sphere {
// <0, 1, 2>, 2
// texture {
//   pigment { color Yellow }
// }
// 

background{ 
  rgb<0.2,0.2,0.4>
}

camera {
  angle 80
  location <11,8,7>
  look_at 0
}


light_source {
  <10,30,-3> 
  color White
}

sky_sphere {
  pigment {
    gradient y
    color_map {
      [0 color White]
      [1 color Blue]
    }
  }
}

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

#declare Ground = prism {
  -1,0, 4
  <5,5>,
  <-5,5>,
  <-5,-5>,
  <5,-5>
  pigment {
    checker Black White scale .5
  }
}



#declare CheeseCenter = 0;
#declare CheeseHeight = 1;
#declare CheeseBody = prism {  
  0, CheeseHeight, 3,
  <-1, -1>,
  <1, 2>,
  <1, 0>
  pigment {
    color Yellow
  }
}

#declare Cheese = difference {
  object { CheeseBody }
  cone {
    <0, 0, 0>, 1,
    <.5, 0.3, 1>, 0.6
  }
  cone {
    <0.5, 0.5, 0>, 0.3,
    <-0.1, 0.1, 1>, 0.7
    rotate <-10, 30, 20>
  }
  
}

object { Ground }
object { Cheese }