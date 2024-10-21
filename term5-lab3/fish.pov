camera {
  location <8, 3, -10> 
  look_at <0, 1, 0>
}

light_source {
  <0, 10, -10>
  color rgb <1, 1, 1>
}

#declare Fish_Texture = texture {
  pigment { color rgb <0.3, 0.7, 1> } 
  finish {
    reflection 0.2   
    specular 0.8     
    ambient 0.2    
  }
}

isosurface {
  function {
    // Формула для тела рыбы в форме капли (обратный эллипсоид)
    pow(x, 2)/1.5 + pow(y, 2)/1 + pow(z/3 + 1, 2) - 1
    // Сужение в области хвоста
    - 0.3 * exp(-3 * pow(z - 2, 2))
  }
  max_gradient 5
  contained_by { box { <-2, -1.5, -4>, <2, 2, 4> } } 
  texture { Fish_Texture }
}

// Голова
sphere {
  <0, 0, -4>, 1 
  texture { Fish_Texture }
}

// Позиция правый глаз
sphere {
  <0.6, 0.4, -4.7>, 0.1
  texture {
    pigment { color rgb <0, 0, 0> }
  }
}

// Левый глаз
sphere {
  <-0.6, 0.4, -4.7>, 0.1
  texture {
    pigment { color rgb <0, 0, 0> }
  }
}

// Правый плавник
cylinder {
  <0.7, 0, 0>, <1.5, 0.5, 1>, 0.05
  texture { Fish_Texture }
}

// Левый плавник
cylinder {
  <-0.7, 0, 0>, <-1.5, 0.5, 1>, 0.05  
  texture { Fish_Texture }
}

// Верхний плавник
cone {
  <0, 0.2, -0.5>, 0.1,
  <0, 1.5, 1.5>, 0.5
  texture { Fish_Texture }
}


plane {
  y, -2
  texture {
    pigment { color rgb <0.2, 0.4, 1> } 
    finish {
      reflection 0.5
    }
  }
}
