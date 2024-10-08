#include "colors.inc"

camera {
  angle 80
  location <5, 2, -5>
  look_at <0, 0, 0>
}

background {
  rgb <0.2, 0.2, 0.4>
}

light_source {
  <10, 30, -10>
  color rgb <1, 1, 1>
}

// Функция для тела рыбы
#declare FishBody = function {
  // Основная форма тела
  (x*x + 0.5*(y*y) + (z - 0.4)*(z - 0.4) - 1) * (y < 0.5) + // Основная часть
  (x*x + (y - 1)*(y - 1) + (z*z) - 0.4) * (y >= 0.5) // Хвост
};

// Исоповерхность для тела рыбы
isosurface {
  function { FishBody(x, y, z) }
  contained_by { box {-2, 2} }
  max_gradient 5
  accuracy 0.001
  pigment { color rgb <0.8, 0.4, 0.4> }
}

// Исоповерхность для плавников
isosurface {
  function { (x - 0.5)*(x - 0.5) + (y - 0.3)*(y - 0.3) + z*z - 0.2 }
  contained_by { box {-2, 2} }
  max_gradient 5
  accuracy 0.001
  pigment { color rgb <0.5, 0.3, 0.2> }
  translate <0, 0.5, 0.5>
}

// Пол
plane {
  y, -2
  texture {
    pigment { color rgb <0.5, 0.5, 0.5> }
    finish { ambient 0.2 diffuse 0.8 }
  }
}
