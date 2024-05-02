#include <Windows.h>

#include "Render.h"

#include <GL/GL.h>
#include <GL/GLU.h>
#include <cmath>
#include <numbers>

#include "./glm/glm.hpp"

using glm::vec3;
using std::numbers::pi;

void colorize(vec3 v) { glColor3f(v.x, v.y, v.z); }
void draw(vec3 v) { glVertex3f(v.x, v.y, v.z); }
vec3 to_rgb(vec3 v) { return v / 255.0f; }

struct _color {
  vec3 white = {1, 1, 1};
  vec3 black = {0, 0, 0};
  vec3 red = {1, 0, 0};
  vec3 green = {0, 1, 0};
  vec3 glue = {0, 0, 1};
  vec3 purple = { 1, 0, 1 };
} color;

vec3 f(vec3 a, vec3 b, float t) { return a * (1 - t) + b * t; }


float t_max = 0;
void Render(double delta_time) {
	t_max += delta_time / 5;
	if (t_max > 1) t_max = 0;
  // glm::vec3 v = {1, 2, 3};

  // glBegin(GL_POINTS);
  // glColor3d(1, 0, 0);

  // glVertex3f(v.x, v.y, v.z);
  // glEnd();

  /*vec3 A = {1, 3, 4};
  vec3 B = {4, 6, 7};

  glBegin(GL_LINE_STRIP);
  glVertex3f(A.x, A.y, A.z);
  glVertex3f(B.x, B.y, B.z);*/

  // for (float t = 0; t <= 2 * pi; t += 0.01) {
  //   // vec3 v = f(A, B, t);
  //   vec3 v = 5.0f * vec3(cos(t), sin(t), 0);
  //   glVertex3d(v.x, v.y, v.z);
  // }
  //glEnd();


  //glBegin(GL_POINTS); // нарисуем точки A и B
  //glVertex3f(A.x, A.y, A.z);
  //glVertex3f(B.x, B.y, B.z);
  //glEnd();

	
  vec3 P1 = {0, 0, 0};
  vec3 P2 = {-4, 6, 7};
  vec3 P3 = {10, 10, 0};

  glBegin(GL_LINES);
  draw(P1);
  draw(P2);
  draw(P2);
  draw(P3);
  glEnd();
  
  glPointSize(10);
  glLineWidth(3);
  colorize(color.green);

  vec3 A;
  vec3 B;
  vec3 P;

  glBegin(GL_LINE_STRIP);
  for (float t = 0; t <= t_max; t += 0.01) {

	  A = f(P1, P2, t);
	  B = f(P2, P3, t);
	  P = f(A, B, t);
	  draw(P);
  }
  glEnd();

  colorize(color.purple);
  glLineWidth(1);

  glBegin(GL_LINES);
  draw(A);
  draw(B);
  glEnd();

  glBegin(GL_POINTS);
  draw(A);
  draw(B);
  draw(P);
  glEnd();

  colorize(color.red);
  glBegin(GL_POINTS);
  draw(P1);
  draw(P2);
  draw(P3);
  glEnd();

  //glBegin(GL_POINTS);
}
