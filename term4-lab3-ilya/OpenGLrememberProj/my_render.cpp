#include <windows.h>

#include <vector>

#include <GL\GL.h>
#include <GL\GLU.h>

#include "./glm/glm.hpp"
#include "glm/geometric.hpp"

using glm::vec3;
using std::vector;
const float pi = 3.14159265;
const float texCell = 0.05;

void draw(vec3 v) { glVertex3f(v.x, v.y, v.z); }
void set_color(vec3 v) { glColor3f(v.x, v.y, v.z); }
void set_color(vec3 v, float alpha) { glColor4f(v.x, v.y, v.z, alpha); }
vec3 rgb_to_gl(vec3 v) { return vec3(v.x / 255., v.y / 255., v.z / 255.); };

void set_normal(vec3 v) { glNormal3f(v.x, v.y, v.z); }
void set_tex(vec3 v) {
  v = v * texCell;
  glTexCoord3f(v.x, v.y, v.z);
}

vec3 normal_to_triangle(const vec3 &v1, const vec3 &v2, const vec3 &v3) {
  vec3 a = v2 - v1;
  vec3 b = v3 - v1;
  vec3 n = glm::normalize(glm::cross(a, b));
  return n;
}

vector<vec3> make_circle(vec3 center, float radius, float start_angle,
                         float end_angle) {
  const int circle_points = 100;
  vector<vec3> circle = {};

  for (double i = 0; i < circle_points; i++) {
    const double angle = start_angle + (std::abs(end_angle - start_angle)) /
                                           (circle_points - 1) * i;
    vec3 v(center[0] + std::cos(angle) * radius,
           center[1] + std::sin(angle) * radius, center[2]);
    circle.push_back(v);
  }
  return circle;
}

void my_render() {
  vec3 normal_top = {0, 0, 1};
  vec3 normal_bottom = {0, 0, -1};

  vector<vec3> vs = {
      {2, 0, 0},  // 0
      {4, 5, 0},  // 1
      {0, 6, 0},  // 2
      {0, 12, 0}, // 3 green
      {7, 14, 0}, // 4 green
      {6, 6, 0},  // 5
      {11, 4, 0}, // 6
      {10, 0, 0}, // 7
  };

  const float h = 3;
  vector<vec3> vs_h = {};
  for (vec3 &v : vs) {
    vs_h.push_back(vec3(v[0], v[1], h));
  }

  vec3 color_cover = rgb_to_gl(vec3(194, 239, 235));
  vec3 color_wall = rgb_to_gl(vec3(158, 229, 223));
  // walls
  for (int i = 0; i < vs.size(); i++) {
    int nextI = (i + 1) % vs.size();
    vec3 n = normal_to_triangle(vs_h[i], vs[nextI], vs[i]);
    glBegin(GL_QUADS);
    set_color(color_wall);
    set_normal(n);
    draw(vs[i]);
    draw(vs[nextI]);
    draw(vs_h[nextI]);
    draw(vs_h[i]);
    glEnd();

    // Нормали

    // glBegin(GL_LINES);
    // draw(vs[i]);
    // draw(vs[i] + n);
    // draw(vs_h[i]);
    // draw(vs_h[i] + n);
    // draw(vs[nextI]);
    // draw(vs[nextI] + n);
    // draw(vs_h[nextI]);
    // draw(vs_h[nextI] + n);
    // glEnd();
  }

  // circle
  vec3 color_cover_green = rgb_to_gl(vec3(46, 196, 183));
  vec3 color_wall_green = rgb_to_gl(vec3(43, 182, 168));
  vec3 line = vec3((vs[4][0] - vs[3][0]), (vs[4][1] - vs[3][1]), 0);
  vec3 center = vec3((vs[4][0] + vs[3][0]) / 2., (vs[4][1] + vs[3][1]) / 2., 0);
  float radius =
      std::sqrt(std::pow(line[0] / 2., 2) + std::pow(line[1] / 2., 2));

  float small_angle = 0.088;
  float start_angle = pi * small_angle;
  float end_angle = pi + pi * small_angle;
  std::vector<vec3> circle =
      make_circle(center, radius, start_angle, end_angle);

  // circle bottom
  glBegin(GL_TRIANGLE_FAN);
  set_color(color_cover_green);
  set_normal(normal_bottom);
  set_tex(center);
  draw(center);
  for (vec3 &v : circle) {
    set_tex(v);
    draw(v);
  }
  glEnd();

  // circle top
  vec3 center_h = vec3(center[0], center[1], h);
  std::vector<vec3> circle_h = {};
  for (vec3 &v : circle) {
    circle_h.push_back(vec3(v[0], v[1], h));
  }

  glBegin(GL_TRIANGLE_FAN);
  set_normal(normal_top);
  set_color(color_cover, 0.5);
  set_tex(center_h);
  draw(center_h);
  for (vec3 &v : circle_h) {
    set_tex(v);
    draw(v);
  }
  glEnd();

  // circle wall
  int circle_dots = circle.size();
  for (int i = 0; i < circle_dots; i++) {
    int nextI = (i + 1) % circle_dots;
    vec3 n = normal_to_triangle(circle_h[i], circle[nextI], circle[i]);
    n[0] = -n[0];
    n[1] = -n[1];

    set_normal(n);
    glBegin(GL_QUADS);
    set_color(color_wall_green);
    set_normal(n);
    draw(circle[i]);
    draw(circle[nextI]);
    draw(circle_h[nextI]);
    draw(circle_h[i]);
    glEnd();

    // Нормали

    // glBegin(GL_LINES);
    // draw(circle[i]);
    // draw(circle[i] + n);
    // draw(circle_h[i]);
    // draw(circle_h[i] + n);
    // draw(circle[nextI]);
    // draw(circle[nextI] + n);
    // draw(circle_h[nextI]);
    // draw(circle_h[nextI] + n);
    // glEnd();
  }

  // bottom

  glBegin(GL_TRIANGLES);
  set_color(color_cover);
  set_normal(normal_bottom);

  set_tex(vs[0]);
  draw(vs[0]);
  set_tex(vs[1]);
  draw(vs[1]);
  set_tex(vs[7]);
  draw(vs[7]);

  set_tex(vs[1]);
  draw(vs[1]);
  set_tex(vs[2]);
  draw(vs[2]);
  set_tex(vs[3]);
  draw(vs[3]);

  set_tex(vs[1]);
  draw(vs[1]);
  set_tex(vs[3]);
  draw(vs[3]);
  set_tex(vs[4]);
  draw(vs[4]);

  set_tex(vs[1]);
  draw(vs[1]);
  set_tex(vs[4]);
  draw(vs[4]);
  set_tex(vs[5]);
  draw(vs[5]);

  set_tex(vs[1]);
  draw(vs[1]);
  set_tex(vs[5]);
  draw(vs[5]);
  set_tex(vs[6]);
  draw(vs[6]);

  set_tex(vs[1]);
  draw(vs[1]);
  set_tex(vs[5]);
  draw(vs[5]);
  set_tex(vs[7]);
  draw(vs[7]);

  set_tex(vs[5]);
  draw(vs[5]);
  set_tex(vs[6]);
  draw(vs[6]);
  set_tex(vs[7]);
  draw(vs[7]);
  glEnd();

  // top
  glBegin(GL_TRIANGLES);
  set_color(color_cover, 0.5);
  set_normal(normal_top);

  set_tex(vs_h[0]);
  draw(vs_h[0]);
  set_tex(vs_h[1]);
  draw(vs_h[1]);
  set_tex(vs_h[7]);
  draw(vs_h[7]);

  set_tex(vs_h[1]);
  draw(vs_h[1]);
  set_tex(vs_h[2]);
  draw(vs_h[2]);
  set_tex(vs_h[3]);
  draw(vs_h[3]);

  set_tex(vs_h[1]);
  draw(vs_h[1]);
  set_tex(vs_h[3]);
  draw(vs_h[3]);
  set_tex(vs_h[4]);
  draw(vs_h[4]);

  set_tex(vs_h[1]);
  draw(vs_h[1]);
  set_tex(vs_h[4]);
  draw(vs_h[4]);
  set_tex(vs_h[5]);
  draw(vs_h[5]);

  set_tex(vs_h[1]);
  draw(vs_h[1]);
  set_tex(vs_h[5]);
  draw(vs_h[5]);
  set_tex(vs_h[6]);
  draw(vs_h[6]);

  set_tex(vs_h[1]);
  draw(vs_h[1]);
  set_tex(vs_h[5]);
  draw(vs_h[5]);
  set_tex(vs_h[7]);
  draw(vs_h[7]);

  set_tex(vs_h[5]);
  draw(vs_h[5]);
  set_tex(vs_h[6]);
  draw(vs_h[6]);
  set_tex(vs_h[7]);
  draw(vs_h[7]);
  glEnd();
}
