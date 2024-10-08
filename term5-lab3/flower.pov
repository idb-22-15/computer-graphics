// Настройки камеры
camera {
  location <0, 5, -10>  // Позиция камеры
  look_at <0, 1, 0>     // Точка, на которую направлена камера
}

// Освещение
light_source {
  <0, 10, -10>          // Позиция источника света
  color rgb <1, 1, 1>   // Цвет света (белый)
}

// Текстура лепестков
#declare Petal_Texture = texture {
  pigment { color rgb <1, 0.3, 0.5> } // Розовый цвет лепестков
  finish { ambient 0.2 diffuse 0.8 }
}

// Текстура стебля
#declare Stem_Texture = texture {
  pigment { color rgb <0.2, 0.8, 0.2> } // Зеленый цвет стебля
  finish { ambient 0.2 diffuse 0.8 }
}

// Функция для лепестка
#declare Petal_Function = function {
  // Создание лепестка в форме объема
  pow(x, 2) + pow(y - 0.5, 2) + pow(z, 2) - 0.25
};

// Лепесток 1
isosurface {
  function { 
    max(
      Petal_Function,
      pow(x - 0.5, 2) + pow(z, 2) - 0.25  // Лепесток 1
    )
  }
  texture { Petal_Texture }
}

// Лепесток 2
isosurface {
  function { 
    max(
      Petal_Function,
      pow(x + 0.5, 2) + pow(z, 2) - 0.25  // Лепесток 2
    )
  }
  texture { Petal_Texture }
}

// Лепесток 3
isosurface {
  function { 
    max(
      Petal_Function,
      pow(z - 0.5, 2) + pow(y - 0.5, 2) - 0.25  // Лепесток 3
    )
  }
  texture { Petal_Texture }
}

// Лепесток 4
isosurface {
  function { 
    max(
      Petal_Function,
      pow(z + 0.5, 2) + pow(y - 0.5, 2) - 0.25  // Лепесток 4
    )
  }
  texture { Petal_Texture }
}

// Стебель цветка
isosurface {
  function {
    pow(x, 2) + pow(z, 2) - 0.05  // Стебель
  }
  translate <0, -1, 0>  // Позиция стебля
  texture { Stem_Texture }
}

// Плоскость для визуализации
plane {
  y, -2
  texture {
    pigment { color rgb <0.2, 0.4, 1> }  // Цвет фона
    finish {
      reflection 0.5    // Отражение
    }
  }
}
