#include <Windows.h>

#include <GL/GL.h>
#include <GL/GLU.h>
#include <cmath>
#include <vector>

#include "./glm/glm.hpp"
#include "glm/geometric.hpp"

#include "./my_render.cpp"

using glm::vec3;
const float texScale = 0.1;

void colorize(vec3 v) { glColor3f(v.x, v.y, v.z); }

vec3 to_rgb(vec3 v) { return v / 255.0f; }

struct _color {
  vec3 white = {1, 1, 1};
  vec3 black = {0, 0, 0};
  vec3 red = {1, 0, 0};
  vec3 green = {0, 1, 0};
  vec3 blue = {0, 0, 1};
  vec3 purple = {1, 0, 1};
} color;

struct _normal {
  vec3 top = {0, 0, 1};
  vec3 bottom = {0, 0, -1};
} normal;

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

vec3 derivative_third_order_bezier(vec3 p0, vec3 p1, vec3 p2, vec3 p3,
                                   float t) {
  vec3 dt = -3.0f * std::pow(1.0f - t, 2.0f) * p0 +
            3.0f * std::pow(1.0f - t, 2.0f) * p1 - 6.0f * t * (1.0f - t) * p1 -
            3.0f * std::pow(t, 2.0f) * p2 + 6.0f * t * (1.0f - t) * p2 +
            3.0f * std::pow(t, 2.0f) * p3;

  return dt;
}

vec3 hermite(vec3 p1, vec3 p4, vec3 r1, vec3 r4, float t) {
  return p1 * (2 * t * t * t - 3 * t * t + 1) + //
         p4 * (-2 * t * t * t + 3 * t * t) +    //
         r1 * (t * t * t - 2 * t * t + t) +     //
         r4 * (t * t * t - t * t);              //
}

vec3 derivative_hermite(vec3 p1, vec3 p4, vec3 r1, vec3 r4, float t) {
  vec3 dt = p1 * (6 * t * t - 6 * t) +     //
            p4 * (-6 * t * t + 6 * t) +    //
            r1 * (3 * t * t - 4 * t + 1) + //
            r4 * (3 * t * t - t);          //
  return dt;
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

void draw_third_order_bezier(vec3 p0, vec3 p1, vec3 p2, vec3 p3, float t_max,
                             bool show) {
  glBegin(GL_LINE_STRIP);
  draw(p0);
  draw(p1);
  draw(p2);
  draw(p3);
  glEnd();

  glPointSize(10);
  glLineWidth(3);
  colorize(color.green);

  glBegin(GL_LINE_STRIP);
  for (float t = 0; t <= 1; t += 0.01) {
    vec3 p = third_order_bezier(p0, p1, p2, p3, t);
    draw(p);
  }
  glEnd();

  vec3 cur_p = third_order_bezier(p0, p1, p2, p3, t_max);
  vec3 tangent =
      glm::normalize(derivative_third_order_bezier(p0, p1, p2, p3, t_max));

  float roll = atan2(-tangent.y, tangent.x); // Угол крена ox
  float pitch = -asin(tangent.z);            // Угол тангажа oy
  float yaw = atan2(tangent.y, tangent.x);   // Угол рысканья oz

  glPushMatrix();
  glTranslated(cur_p.x - 3, cur_p.y - 7, cur_p.z - 1.5);
  glRotatef(roll, 0.0f, 0.0f, 1.0f);
  glRotatef(pitch, 1.0f, 0.0f, 0.0f);
  glRotatef(yaw, 0.0f, 1.0f, 0.0f);
  if (show)
    my_render();

  glPopMatrix();

  glLineWidth(1);
  colorize(color.red);

  glBegin(GL_POINTS);
  draw(p0);
  draw(p1);
  draw(p2);
  draw(p3);
  glEnd();
  colorize(color.black);
}

void draw_hermite(vec3 p1, vec3 p4, vec3 r1, vec3 r4, float t_max, bool show) {
  glBegin(GL_LINES);
  draw(p1);
  draw(r1);
  draw(p4);
  draw(r4);
  glEnd();

  glPointSize(10);
  glLineWidth(3);
  colorize(color.blue);

  glBegin(GL_LINE_STRIP);
  for (float t = 0; t <= 1; t += 0.01) {
    vec3 p = hermite(p1, p4, r1, r4, t);
    draw(p);
  }
  glEnd();

  vec3 cur_p = hermite(p1, p4, r1, r4, t_max);
  vec3 tangent =
      glm::normalize(derivative_third_order_bezier(p1, p4, r1, r4, t_max));

  float roll = atan2(-tangent.y, tangent.x); // Угол крена ox
  float pitch = -asin(tangent.z);            // Угол тангажа oy
  float yaw = atan2(tangent.y, tangent.x);   // Угол рысканья oz

  glPushMatrix();
  glTranslated(cur_p.x - 3, cur_p.y - 7, cur_p.z - 1.5);
  glRotatef(roll, 0.0f, 0.0f, 1.0f);
  glRotatef(pitch, 1.0f, 0.0f, 0.0f);
  glRotatef(yaw, 0.0f, 1.0f, 0.0f);
  if (show)
    my_render();

  glPopMatrix();

  glLineWidth(1);
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
  return factorial(n) / float(factorial(i) * factorial(n - i)) * //
         std::pow(u, i) * std::pow(1 - u, n - i);
}

vec3 bezier_surface(std::vector<std::vector<vec3>> &ps, float u, float v) {
  int n = ps.size();
  int m = ps[0].size();

  vec3 sum = {0, 0, 0};
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < m; j++) {
      sum += bernstein(u, n, i) * bernstein(v, m, j) * ps[i][j];
    }
  }
  return sum;
}

void draw_bezier_surface(std::vector<std::vector<vec3>> &ps, float t_max) {
  int n = ps.size();
  int m = ps[0].size();

  colorize(color.red);
  glPointSize(5);
  glBegin(GL_POINTS);
  for (auto &row : ps) {
    for (auto &p : row) {
      draw(p);
    }
  }
  glEnd();

  for (int i = 0; i < n - 1; i++) {
    for (int j = 0; j < m - 1; j++) {
      vec3 top_left = ps[i][j];
      vec3 top_right = ps[i][j + 1];
      vec3 bottom_left = ps[i + 1][j];
      vec3 bottom_right = ps[i + 1][j + 1];

      glBegin(GL_LINE_LOOP);
      draw(top_left);
      draw(top_right);
      draw(bottom_left);
      draw(bottom_right);
      glEnd();
    }
  }

  float step = 0.1;
  for (float u = 0; u < 1 - step; u += step) {
    for (float v = 0; v < 1 - step; v += step) {
      vec3 c = color.black + v / 0.7f + u / 0.7f;
      colorize(c);

      vec3 top_left = bezier_surface(ps, u, v);
      vec3 top_right = bezier_surface(ps, u, v + step);
      vec3 bottom_left = bezier_surface(ps, u + step, v);
      vec3 bottom_right = bezier_surface(ps, u + step, v + step);

      vec3 edge1 = top_right - top_left;
      vec3 edge2 = bottom_left - top_left;
      vec3 n = glm::normalize(glm::cross(edge1, edge2));

      set_normal(n);

      glBegin(GL_POLYGON);
      set_tex(top_left);
      draw(top_left);
      set_tex(top_right);
      draw(top_right);
      set_tex(bottom_right);
      draw(bottom_right);
      set_tex(bottom_left);
      draw(bottom_left);
      glEnd();

      colorize(color.blue);
      glBegin(GL_LINES);
      draw(top_left);
      draw(top_left + n);
      draw(top_right);
      draw(top_right + n);
      draw(bottom_left);
      draw(bottom_left + n);
      draw(bottom_right);
      draw(bottom_right + n);
      glEnd();
    }
  }
}

bool isReversed = false;

void task(float delta_time) {
  float t_step = delta_time;

  if (!isReversed)
    t_max += t_step;
  else
    t_max -= t_step;

  if (t_max > 1) {
    isReversed = true;
    t_max = 1;
  } else if (t_max < 0) {
    isReversed = false;
    t_max = 0;
  }

  vec3 p0 = {0, 0, 0};
  vec3 p1 = {-4, -7, 7};
  vec3 p2 = {10, 30, 0};
  vec3 p3 = {5, 5, 20};

  vec3 v1 = {1, 2, 8};
  vec3 v2 = {15, 15, 8};
  vec3 v3 = {6, 7, 8};
  vec3 v4 = {7, 9, 8};

  // Зелёные
  draw_third_order_bezier(2.0f * p0, 2.0f * p1, 2.0f * p2, 2.0f * p3, t_max,
                          true);
  draw_third_order_bezier(p0 * 2.0f, p1 / 3.0f, p2 * -1.0f, p3 + 5.0f, t_max,
                          false);

  // Синие
  draw_hermite(5.0f * p0, 5.0f * p1, 5.0f * p2, 5.0f * p3, t_max, true);
  draw_hermite(p0, v1, v3, v4, t_max, false);

  // std::vector<std::vector<vec3>> ps = {
  //     {
  //         {-5, 5, 3},
  //         {5, 5, 3},
  //     }, //
  //     {
  //         {-5, 5, 3},
  //         {5, -5, 3},
  //     }, //

  // };

  // draw_bezier_surface(ps, t_max);
}
