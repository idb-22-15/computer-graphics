camera {
  // angle 80
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
    pigment { color rgb <0.2, 0.4, 1> }  // Цвет воды
    finish {
      reflection 0.1    // Отражение воды
    }
  }
}

// Тело Евы (капля)
#declare Drop_Shape =
isosurface {
  function {
    // Уравнение для капли
    pow(x, 2) + pow((y - 1) / 1.2, 2) + pow(z, 2) - 2
  }
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
}

object { 
  Drop_Shape
  translate <0, -0.5, 0>
}

#declare HeadF =
function {
  sqrt(pow(x,2) + pow((y + 3.2) * 1.2, 2) + pow(z,2)) - 1.2
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
}

object {
  Head
  translate <0, 3.2, 0>
}

// Голова Евы (сфера)
// sphere {
//   <0, 1.5, 0>, 0.75  // Центр сферы и радиус
//   texture {
//     pigment { color rgb <1, 1, 1> }  // Белая поверхность
//     finish {
//       reflection 0.3
//       specular 0.8
//     }
//   }
// }

// Левая рука (лепесток)
cone {
  <1, 0.5, 0>, 0.2  // Нижняя точка и радиус
  <1.5, 1.0, 0>, 0.05  // Верхняя точка и радиус
  texture {
    pigment { color rgb <1, 1, 1> }
    finish {
      reflection 0.3
      specular 0.8
    }
  }
}

// Правая рука (лепесток)
cone {
  <-1, 0.5, 0>, 0.2  // Нижняя точка и радиус
  <-1.5, 1.0, 0>, 0.05  // Верхняя точка и радиус
  texture {
    pigment { color rgb <1, 1, 1> }
    finish {
      reflection 0.3
      specular 0.8
    }
  }
}
