#include <Windows.h>

#include "Render.h"

#include <GL/GL.h>
#include <GL/GLU.h>

#include "./glm/glm.hpp"

void Render(double delta_time) {
  glm::vec3 v = {1, 2, 3};

  glBegin(GL_POINTS);
  glColor3d(1, 0, 0);

  glVertex3d(v.x, v.y, v.z);
  glEnd();
}
