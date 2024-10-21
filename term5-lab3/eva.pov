#version 3.7;

global_settings {
  assumed_gamma 1.0
}

camera {
  location <0, 2, -5>
  look_at <0, 1, 0>
}

light_source {
  <10, 10, -10>
  color rgb <1, 1, 1>
}

background {
  color rgb <0.1, 0.1, 0.1>
}

// Определяем форму Евы
#declare Eva_Shape =
  isosurface {
    function {
      // Основная форма Евы
      (pow(x, 2) + pow(y - 0.5, 2) + pow(z, 1.5) - 1) *
      (pow(y - 1, 2) - 0.2) // Для создания контуров
    }
    accuracy 0.001
    max_gradient 2
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

object { Eva_Shape }

// Добавляем руки
#declare Hand =
  isosurface {
    function {
      (pow(x - 1, 2) + pow(y - 0.5, 2) + pow(z, 2) - 0.2) +
      (pow(x + 1, 2) + pow(y - 0.5, 2) + pow(z, 2) - 0.2) // Две руки
    }
    accuracy 0.001
    max_gradient 2
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

object { Hand }
