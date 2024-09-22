#include "colors.inc"    

background{ 
  rgb<0.2,0.2,0.4>
}

light_source {
  <10,30,-3> 
  color White
}


light_source {
  <-7, 20, -5> 
  color Yellow
}


// sky_sphere {
//   pigment {
//     gradient y
//     color_map {
//       [0 color White]
//       [1 color Blue]
//     }
//   }
// }

// camera 1

camera {
  angle 80
  location <2, 2, -2>
  look_at 0
}

// camera 2

// camera {
//   angle 80
//   location <-1, 2.5, -4>
//   look_at <4, 3.5, 4>
// }

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
  pigment {
    checker Black White scale .5
  }
}

object { ground }

#declare book_cover = union {
  box {
    <-1, -0.05, 1>
    <0, 0.05, -1>
    pigment {
      Brown
    }
    finish {
      reflection {
        0.01
        metallic 1
      }
    }
    rotate <0, 0, -15>
  }
  box {
    <-1, -0.05, 1>
    <0, 0.05, -1>
    pigment {
      Brown
    }
    finish {
      reflection {
        0.01
        metallic 1
      }
    }
    rotate <0, 0, 195>
  }
  translate <0.7, 0, -0.2>
}

#macro make_page(ang)
  box {
     <-0.9, -0.005, 0.9>
    <0, 0.005, -0.9>
    pigment {
      White
    }
    rotate <0, 0, ang>
    translate <0.7, 0.05, -0.2>
  }
#end

#for (ang, -20, -160, -10)
  make_page(ang)

  #if ((ang < -120)) 
    make_page(ang - 5)
    make_page(ang - 3)
  #end
#end

object {
  book_cover
}

union {
  lathe {
    cubic_spline
    9
    <0, 0>, <1, 0.1>, <1.5, 0.15>, <1, 0.2>
    <0.2, 0.3>, <1.5, 0.5>, <2, 2>, <2.2, 2.1>, <2.2, 2.15>
    pigment {
      gradient <0.5, 1, 0.2> 
        color_map {
          [0 Red]
          [2.15 Blue]
        }
      
    }
    finish {
      reflection {
        0.01
        metallic 0.7
      }
    }
  }
  lathe {
    linear_spline
    4
    <0, 1.5>, <1.85, 1.5>, <1.85, 1.6>, <0, 1.6>
    pigment {rgb<166 / 255, 217 / 255, 255 / 255> }
  }
  scale 0.4
  translate <-1.5, 0, -0.5>
}