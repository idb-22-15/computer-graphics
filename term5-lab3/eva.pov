#version 3.7;

// global_settings {
//   assumed_gamma 1.0
// }

camera {
  angle 80
  location <5, 1.5, -6>
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

// isosurface {
//   function {
//     pow(x, 2) + pow(y, 2) + pow(z, 3) - 1.5 * pow(z, 2)
//   }
//   contained_by { box {-3, 3} }
//   // max_gradient 1
//   // threshold 0.1
//    texture {
//     pigment { color rgb <0.2, 0.4, 1> }  // Цвет воды
   
//   }
// }

#declare Drop_Shape =
  isosurface {
    function {
      // Уравнение для капли
      pow(x, 2) + pow(y, 2) + pow(z - 1, 2) - pow(z, 2)
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

object { Drop_Shape }

// Определяем форму Евы
#declare Eva_Shape =
  isosurface {
    function {
      // Упрощенная форма
      pow(x, 2) + pow(y, 2) + pow(z, 1.5) - 1  // Изменение для лучшей видимости
    }
    // accuracy 0.001
    // max_gradient 2
    contained_by { box {-2, 2} }
    threshold 1
    texture {
      pigment { color rgb <0.8, 0.9, 1> }
      finish {
        reflection 0.2
        specular 0.8
        ambient 0.2
      }
    }
  }

// object { Eva_Shape}  // Сдвигаем Еву вверх
