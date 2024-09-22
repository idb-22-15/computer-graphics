#include "colors.inc"    

// camera 1

camera {
  angle 80
  location <3, 2, -3>
  look_at 0
  scale 1.5
}

// camera 2

// camera {
//   angle 80
//   location <4, 1.5, -5>
//   look_at 0
// }

background{ 
  rgb<0.2,0.2,0.4>
}

light_source {
  <10,30,-3> 
  color White
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

#declare lamp = union {
  cylinder {
    <0, 0, 0>
    <0, 0.1, 0>
    1
  }
  cylinder {
    <0, 0.1, 0>
    <0, 3, 0>,
    0.15
  }
  box {
    <-1, 3, 0.2>, <3.4, 3.05, -0.2>
    rotate <0, 0, 5>
  }
  light_source {
    <2, 3, 0> 
    color Yellow
  }
  sphere_sweep{
    cubic_spline, 5 
        <-2, 5, 0>, 0.05
        <-1, 2.9, 0>, 0.05
        <-1.5, 2.4, 0>, 0.05
        <-0, 2.8, 0>, 0.05
        <-0, 2.8, 0>, 0.05
    pigment { rgb<255 / 255, 95 / 255, 23 / 255> }
  }
  pigment { White }
  rotate <0, -30, 0>
  translate <0, 0, -2>
}

object { lamp }

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
  translate <2, 0, -0.2>
}

#macro make_page(ang)
  box {
     <-0.9, -0.005, 0.9>
    <0, 0.005, -0.9>
    pigment {
      White
    }
    rotate <0, 0, ang>
    translate <2, 0.05, -0.2>
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

#declare cup = union {
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
  scale 0.3
  translate <1.5, 0, -2.5>
}

object { cup }