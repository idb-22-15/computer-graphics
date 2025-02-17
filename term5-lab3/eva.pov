camera {
  location <5, 2.5, -6>
  look_at 0
}

light_source {
  <10, 10, -10>
  color rgb <1, 1, 1>
}

plane {
  y, -2
  texture {
    pigment { color rgb <0.2, 0.4, 1> }
    finish {
      reflection 0.1
    }
  }
}

#declare BodyF = 
function {
  pow(x, 2) + pow(y, 2) + pow(z, 2) - 1.8
}

#declare HeadF =
function {
  sqrt(pow(x,2) + pow((y + 3.2) * 1.2, 2) + pow(z,2)) - 1.2
}

#declare Body =
isosurface {
  function { BodyF(x, (y - 1.5) / 1.7, z)  }
  accuracy 0.001
  max_gradient 10
  contained_by { box {-2, 2} }
  texture {
    pigment { color rgb <0.8, 0.9, 1> }
    finish {
      reflection 0.2
      specular 0.8
      ambient 0.2
    }
  }
  translate <0, -0.5, 0>
}

#declare Head = 
isosurface {
  function { HeadF(x, y - 2, z) }
  accuracy 0.001
  max_gradient 10
  contained_by { box {-1.5, 1.5} }
  texture {
    pigment { color rgb <0.8, 0.9, 1> }
    finish {
      reflection 0.2
      specular 0.8
      ambient 0.2
    }
  }
  translate <0, 3.2, 0>
}

#declare Arms =
union {
  cone {
    <1, 1.0, 0>, 0.4
    <2.0, -0.5, 0>, 0.15
    texture {
      pigment { color rgb <1, 1, 1> }
      finish {
        reflection 0.3
        specular 0.8
      }
    }
    rotate <0, 0, -15>
    translate <-0.1, 0.4, 0>
  }

  cone {
    <-1, 1.0, 0>, 0.4
    <-2.0, -0.5, 0>, 0.15
    texture {
      pigment { color rgb <1, 1, 1> }
      finish {
        reflection 0.3
        specular 0.8
      }
    }
    rotate <0, 0, -15>
    translate <0.1, 0.4, 0>
  }
}

#declare Face =
sphere {
  <0, 2.3, -0.6>, 0.5
  scale <2.5, 1, 1> 
  texture {
    pigment { color rgb <10 / 255 , 20 / 255, 30 / 255> }
    finish {
      ambient 0.2
    }
  }
}

#declare Eyes =
union {
  sphere {
    <-0.7, 2.4, -1>, 0.1
    texture {
      pigment { color rgb <0.2, 0.8, 1> }
      finish {
        ambient 0.8
      }
    }
  }

  sphere {
    <1, 2.4, -1>, 0.1
    texture {
      pigment { color rgb <0.2, 0.8, 1> }
      finish {
        ambient 0.8
      }
    }
  }
}

// Ева
// object { Head }
// object { Face }
// object { Body }
// object { Arms }
// object { Eyes }


#declare F1 =
function {
  sqrt(pow(x,2) + pow(z,2)) - 1
}

#declare F2 = 
function {
  sqrt(pow(x,2) + pow(y,2) + pow(z,2)) - 2
}

#declare Drop =
isosurface {
  function { F1(x,y * 1.5,z) + F2(x,y - 2,z) } 
  max_gradient 10
  contained_by { box {-3, 3} }
  texture {
    pigment { color rgb <0.8, 0.9, 1> }
    finish {
      reflection 0.2
      specular 0.8
      ambient 0.2
    }
  }
}

// Капля
object { Drop }