#include <Windows.h>

#include "Render.h"

#include <GL/GL.h>
#include <GL/GLU.h>
#include <cmath>
#include <numbers>
#include <vector>

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
  vec3 blue = {0, 0, 1};
  vec3 purple = {1, 0, 1};
} color;

vec3 f(vec3 a, vec3 b, float t) { return a * (1 - t) + b * t; }
vec3 second_order_bezier(vec3 p1, vec3 p2, vec3 p3, float t) {
  return p1 * (1 - t) * (1 - t)    //
         + 2.0f * p2 * t * (1 - t) //
         + p3 * t * t;
}
vec3 third_order_bezier(vec3 p0, vec3 p1, vec3 p2, vec3 p3, float t) {
  return std::pow(1 - t, 3.0f) * p0             //
         + 3 * t * std::pow(1 - t, 2.0f) * p1 + //
         3 * std::pow(t, 2.0f) * (1 - t) * p2   //
         + std::pow(t, 3.0f) * p3;
}

vec3 hermite(vec3 p1, vec3 p4, vec3 r1, vec3 r4, float t) {
  return p1 * (2 * t * t * t - 3 * t * t + 1) + //
         p4 * (-2 * t * t * t + 3 * t * t) +    //
         r1 * (t * t * t - 2 * t * t + t) + r4 * (t * t * t - t * t);
}

float t_max = 0;

void draw_second_order_bezier(vec3 p0, vec3 p1, vec3 p2, float t_max) {
  glBegin(GL_LINES);
  draw(p0);
  draw(p1);
  draw(p1);
  draw(p2);
  glEnd();

  glPointSize(10);
  glLineWidth(3);
  colorize(color.green);

  vec3 a;
  vec3 b;
  vec3 p;

  glBegin(GL_LINE_STRIP);
  for (float t = 0; t <= t_max; t += 0.01) {
    a = f(p0, p1, t);
    b = f(p1, p2, t);
    p = second_order_bezier(p0, p1, p2, t);
    draw(p);
  }
  glEnd();

  colorize(color.purple);
  glLineWidth(1);

  glBegin(GL_LINES);
  draw(a);
  draw(b);
  glEnd();

  glBegin(GL_POINTS);
  draw(a);
  draw(b);
  draw(p);
  glEnd();

  colorize(color.red);
  glBegin(GL_POINTS);
  draw(p0);
  draw(p1);
  draw(p2);
  glEnd();
}

void draw_third_order_bezier(vec3 p0, vec3 p1, vec3 p2, vec3 p3, float t_max) {
  glBegin(GL_LINE_STRIP);
  draw(p0);
  draw(p1);
  draw(p2);
  draw(p3);
  glEnd();

  glPointSize(10);
  glLineWidth(3);
  colorize(color.green);

  vec3 p;

  glBegin(GL_LINE_STRIP);
  for (float t = 0; t <= t_max; t += 0.01) {
    p = third_order_bezier(p0, p1, p2, p3, t);
    draw(p);
  }
  glEnd();

  glLineWidth(1);
  colorize(color.purple);

  glBegin(GL_POINTS);
  draw(p);
  glEnd();

  colorize(color.red);

  glBegin(GL_POINTS);
  draw(p0);
  draw(p1);
  draw(p2);
  draw(p3);
  glEnd();
  colorize(color.black);
}

void draw_hermite(vec3 p1, vec3 p4, vec3 r1, vec3 r4, float t_max) {
  glBegin(GL_LINES);
  draw(p1);
  draw(r1);
  draw(p4);
  draw(r4);
  glEnd();

  glPointSize(10);
  glLineWidth(3);
  colorize(color.blue);

  vec3 p;

  glBegin(GL_LINE_STRIP);
  for (float t = 0; t <= t_max; t += 0.01) {
    p = hermite(p1, p4, r1, r4, t);
    draw(p);
  }
  glEnd();

  glLineWidth(1);
  colorize(color.purple);

  glBegin(GL_POINTS);
  draw(p);
  glEnd();

  colorize(color.red);

  glBegin(GL_POINTS);
  draw(p1);
  draw(p4);
  draw(r1);
  draw(r4);
  glEnd();
  colorize(color.black);
}

int factorial(const int n) {
  int f = 1;
  for (int i = 1; i <= n; ++i)
    f *= i;
  return f;
}

float bernstein(float u, int n, int i) {
  return factorial(n) / float(factorial(i) * factorial(n - i)) //
         * std::pow(u, i) * std::pow(1 - u, n - i);
}

vec3 bezier_surface(std::vector<std::vector<vec3>> ps, float u, float v) {
  int n = ps.size();
  int m = ps[0].size();

  vec3 sum = {0, 0, 0};
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < m; j++) {
      sum += bernstein(u, n, i) * bernstein(u, m, j) * ps[i][j];
    }
  }
  return sum;
}

void draw_bezier_surface(std::vector<std::vector<vec3>> ps, float t_max) {
  int n = ps.size();
  int m = ps[0].size();

  colorize(color.red);
  glPointSize(3);
  glBegin(GL_POINTS);
  for (auto &row : ps) {
    for (auto &p : row) {
      draw(p);
    }
  }
  glEnd();

  colorize(color.blue);

  for (int i = 0; i < n ; i++) {
    for (int j = 0; j < m ; j++) {
      vec3 top_left = ps[i][j];
      vec3 top_right = ps[i][(j + 1) % m];
      vec3 bottom_left = ps[(i + 1) % n][j];
      vec3 bottom_right = ps[(i + 1) % n][(j + 1) % m];

      glBegin(GL_LINE_STRIP);
      draw(top_left);
      draw(top_right);
      draw(bottom_left);
      draw(bottom_right);
      glEnd();
    }
  }
}

void Render(double delta_time) {
  t_max += delta_time / 5;
  if (t_max > 1)
    t_max = 0;

  vec3 p0 = {0, 0, 0};
  vec3 p1 = {-4, 6, 7};
  vec3 p2 = {10, 10, 0};
  vec3 p3 = {5, 5, 5};

  // draw_third_order_bezier(p0, p1, p2, p3, t_max);
  // draw_third_order_bezier(p0 * 2.0f, p1 / 3.0f, p2 * -1.0f, p3 + 5.0f,
  // t_max);

  // draw_hermite(p0, p1, p2, p3, t_max);
  // draw_hermite(p0 * 2.0f, p1 / 3.0f, p2 * -1.0f, p3 + 5.0f, t_max);

  std::vector<std::vector<vec3>> ps = {
      {{-5, 5, 0}, {-3, 3, 3}, {3, 3, 3}, {5, 5, 0}},
      {{-2, 2, 4}, {-1, 3, 6}, {1, 3, 6}, {5, 5, 4}},
      {{-2, -2, 4}, {-1, -3, 6}, {1, -3, 6}, {2, -2, 4}},
      {{-5, -5, 0}, {-3, -3, 3}, {3, -3, 3}, {5, -5, 0}},

  };

  draw_bezier_surface(ps, t_max);
}
