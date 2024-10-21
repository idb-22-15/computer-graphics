// Настройки камеры
camera {
  location <0, 5, -15>  // Позиция камеры
  look_at <0, 1, 0>     // Точка, на которую направлена камера
}

// Освещение
light_source {
  <0, 10, -10>          // Позиция источника света
  color rgb <1, 1, 1>   // Цвет света (белый)
}

// Текстура
#declare Surface_Texture = texture {
  pigment { color rgb <0.3, 0.5, 0.8> }
  finish { ambient 0.2 diffuse 0.8 }
}

// Изоповерхности с разными настройками
isosurface {
  function { pow(x, 2) + pow(y, 2) + pow(z, 2) - 1 } // Сфера радиусом 1
  texture { Surface_Texture }
}

isosurface {
  function { pow(x/1.5, 2) + pow(y, 2) + pow(z/2, 2) - 1 } // Эллипсоид
  translate <3, 0, 0>
  texture { Surface_Texture }
}

isosurface {
  function { pow(sqrt(pow(x, 2) + pow(z, 2)) - 1, 2) + pow(y, 2) - 0.25 } // Тор
  translate <-3, 0, 0>
  texture { Surface_Texture }
}

// Изоповерхности с преобразованиями
isosurface {
  function { pow(x, 2) + pow(y, 2) + pow(z/0.5, 2) - 1 } // Сжимаемая сфера
  texture { Surface_Texture }
}

isosurface {
  function { pow(x/2, 2) + pow(y, 2) + pow(z, 2) - 1 } // Растянутая сфера
  translate <3, 0, 0>
  texture { Surface_Texture }
}

isosurface {
  function { pow(x, 2) + pow(y/3, 2) + pow(z, 2) - 1 } // Вытянутая сфера
  translate <-3, 0, 0>
  texture { Surface_Texture }
}

// Изоповерхности, состоящие из нескольких операций
isosurface {
  function {
    max(
      pow(x, 2) + pow(y, 2) + pow(z, 2) - 1,  // Сфера 1
      pow(x - 1, 2) + pow(y - 1, 2) + pow(z, 2) - 1 // Сфера 2
    )
  }
  translate <0, 0, 2> // Сдвигаем вверх
  texture { Surface_Texture }
}

isosurface {
  function {
    max(
      pow(sqrt(pow(x, 2) + pow(z, 2)) - 1, 2) + pow(y, 2) - 0.25, // Тор 1
      pow(sqrt(pow(x - 1, 2) + pow(z - 1, 2)) - 0.5, 2) + pow(y, 2) - 0.1 // Тор 2
    )
  }
  translate <2, 0, 2> // Сдвигаем вверх
  texture { Surface_Texture }
}

// Плоскость для визуализации
plane {
  y, -2
  texture {
    pigment { color rgb <0.2, 0.4, 1> }  // Цвет воды
    finish {
      reflection 0.5    // Отражение воды
    }
  }
}
