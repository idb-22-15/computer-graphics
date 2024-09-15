#include "colors.inc"    

background{ 
  rgb<0.2,0.2,0.4>
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

// camera 1

camera {
  angle 80
  location <5, 7, -10>
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

#declare Ground = prism {
  -1,0, 4
  <8,8>,
  <-8,8>,
  <-8,-8>,
  <8,-8>
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
    <0, 0, 0>, 1,
    <0, CheeseHeight, 1>, 1
  }
  cone {
    <0.5, 0.5, 0>, 0.3,
    <-0.1, 0.1, 1>, 0.7
    rotate <-10, 30, 20>
  }
}


#declare HouseWall = box {
  <--5, 0, 5>
  <-4.9, 4, 4.9>
  pigment {
    Brown
  }
}


#declare HouseWalls = union {
  object { HouseWall }
  box {
    <-5, 0, -5>
    <5, 0.1, 5>
    pigment { Gray }
  }
  object { 
    HouseWall
    rotate <0, 90, 0>
  }
  object { 
    HouseWall
    rotate <0, 180, 0>
  }
  object { 
    HouseWall
    rotate <0, 270, 0>
  }
}

#declare WindowInner = box {
  <2 + 0.1, 1 + 0.1, -5.2>
  <4 - 0.1, 3 - 0.1, -4.7>
  pigment { DarkBrown }
}

#declare Window = union {
  difference {
  box {
    <2, 1, -5.1>
    <4, 3, -4.8>
    pigment { DarkBrown }
  }

  object { WindowInner }
  }
  box {
    <2 + 0.1, 1 + 0.1, -5.01>
    <4 - 0.1, 3 - 0.1, -4.99>
    pigment {
      color Blue transmit 0.5
    }
  }
}

#declare DoorHole = box {
  <-1, 0.11, -5.2>
  <1, 3, -4.8>
  pigment { Black }
}

#declare Door = box {
  <-1, 0.11, -0.05>
  <1, 3, 0.05>
  pigment { DarkBrown }
}

#declare Light = union {
  cone {
    <0, 3.7, 0>, 1
    <0, 4, 0>, 0.7
    pigment { DarkBrown }
  }
  sphere {
    <0, 3.7, 0>, 0.3
    pigment { Yellow }
  }
  light_source {
    <0.1, 3, 0.1>
    color Yellow
  }
}

#declare Roof =
prism {
    conic_sweep    // Тип призмы
    linear_spline
    4, 5            // Высота призмы от 0 до 1
    4,              // Количество точек (вершин)
    <-1, 1>         // Первая точка (основание)
    <1 1>         // Вторая точка (пик треугольника)
    <1, -1>         // Третья точка (другое основание)
    <-1, -1>          // Возвращаемся к началу для замыкания контура
    rotate <180, 0, 0>
    translate <0, 9, 0>
    texture {
      pigment { Red }
    }
}

#declare Home = union {
  difference {
    object { HouseWalls }
    object { DoorHole }
    object { WindowInner }
  }
  object {
    Door
    rotate<0, 60, 0>
    translate<-0.3, 0, -5.9>
  }
  object { Window }
  object { Light }
  object { Roof }
}

object { Home }

object { Ground }

object { 
  Cheese
  scale 0.5
  translate <2, 0, 2>
}
