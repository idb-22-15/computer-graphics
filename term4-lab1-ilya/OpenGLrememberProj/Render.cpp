#include "Render.h"
#include <Windows.h>

#include "./glm/glm.hpp"
#include <GL\GL.h>
#include <GL\GLU.h>
#include <vector>

using glm::vec3;
using std::vector;
const float pi = 3.14159265;

void draw(vec3 v) { glVertex3f(v.x, v.y, v.z); }
void colorize(vec3 v) { glColor3f(v.x, v.y, v.z); }
vec3 rgb_to_gl(vec3 v) { return vec3(v.x / 255., v.y / 255., v.z / 255.); };

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

double get_angle_in_rad(vec3 start, vec3 end) {
  vec3 v = vec3(start[0] - end[0], start[1] - end[1], start[2]);
  double angle_in_rad = std::atan2(v[1], v[0]);
  return angle_in_rad;
}

double normalize_angle_in_rad(float angle) {
  return (2 * pi + angle) / (2 * pi);
}

void my_render(double delta_time) {
  vector<vec3> vs = {
      {2, 0, 0},  // 0 blue
      {4, 5, 0},  // 1
      {0, 6, 0},  // 2
      {0, 12, 0}, // 3 green
      {7, 14, 0}, // 4 green
      {6, 6, 0},  // 5
      {11, 4, 0}, // 6
      {10, 0, 0}, // 7 blue
  };

  const float h = 3;
  vector<vec3> vs_h = {};
  for (vec3 &v : vs) {
    vs_h.push_back(vec3(v[0], v[1], h));
  }

  vec3 color_cover = rgb_to_gl(vec3(194, 239, 235));
  vec3 color_wall = rgb_to_gl(vec3(158, 229, 223));

  // bottom
  colorize(color_cover);

  glBegin(GL_TRIANGLES);
  draw(vs[0]);
  draw(vs[1]);
  draw(vs[5]);

  draw(vs[1]);
  draw(vs[2]);
  draw(vs[3]);

  draw(vs[1]);
  draw(vs[3]);
  draw(vs[4]);

  draw(vs[1]);
  draw(vs[4]);
  draw(vs[5]);

  draw(vs[1]);
  draw(vs[5]);
  draw(vs[6]);

  draw(vs[5]);
  draw(vs[6]);
  draw(vs[7]);
  glEnd();

  // top

  glBegin(GL_TRIANGLES);
  draw(vs_h[0]);
  draw(vs_h[1]);
  draw(vs_h[5]);

  draw(vs_h[1]);
  draw(vs_h[2]);
  draw(vs_h[3]);

  draw(vs_h[1]);
  draw(vs_h[3]);
  draw(vs_h[4]);

  draw(vs_h[1]);
  draw(vs_h[4]);
  draw(vs_h[5]);

  draw(vs_h[1]);
  draw(vs_h[5]);
  draw(vs_h[6]);

  draw(vs_h[5]);
  draw(vs_h[6]);
  draw(vs_h[7]);
  glEnd();

  // walls
  colorize(color_wall);
  for (int i = 0; i < vs.size(); i++) {
    if (i == vs.size() - 1)
      continue;

    int nextI = (i + 1) % vs.size();

    glBegin(GL_QUADS);
    draw(vs[i]);
    draw(vs[nextI]);
    draw(vs_h[nextI]);
    draw(vs_h[i]);
    glEnd();
  }

  // circle green
  vec3 color_cover_green = rgb_to_gl(vec3(46, 196, 183));
  vec3 color_wall_green = rgb_to_gl(vec3(43, 182, 168));
  vec3 g_line = vec3((vs[4][0] - vs[3][0]), (vs[4][1] - vs[3][1]), 0);
  vec3 g_center =
      vec3((vs[4][0] + vs[3][0]) / 2., (vs[4][1] + vs[3][1]) / 2., 0);
  float g_radius =
      std::sqrt(std::pow(g_line[0] / 2., 2) + std::pow(g_line[1] / 2., 2));

  float g_small_angle = 0.088;
  float g_start_angle = pi * g_small_angle;
  float g_end_angle = pi + pi * g_small_angle;
  std::vector<vec3> g_circle =
      make_circle(g_center, g_radius, g_start_angle, g_end_angle);

  // circle green bottom
  glBegin(GL_TRIANGLE_FAN);
  colorize(color_cover_green);
  draw(g_center);
  for (vec3 &v : g_circle) {
    draw(v);
  }
  glEnd();

  // circle green top
  vec3 g_center_h = vec3(g_center[0], g_center[1], h);
  std::vector<vec3> g_circle_h = {};
  for (vec3 &v : g_circle) {
    g_circle_h.push_back(vec3(v[0], v[1], h));
  }

  glBegin(GL_TRIANGLE_FAN);
  draw(g_center_h);
  for (vec3 &v : g_circle_h) {
    draw(v);
  }
  glEnd();

  // circle green wall
  colorize(color_wall_green);
  int g_circle_dots = g_circle.size();
  for (int i = 0; i < g_circle_dots; i++) {
    int nextI = (i + 1) % g_circle_dots;

    glBegin(GL_QUADS);
    draw(g_circle[i]);
    draw(g_circle[nextI]);
    draw(g_circle_h[nextI]);
    draw(g_circle_h[i]);
    glEnd();
  }
  colorize(color_cover);

  // circle blue
  vec3 color_cover_blue = rgb_to_gl(vec3(42, 148, 223));
  vec3 color_wall_blue = rgb_to_gl(vec3(27, 115, 177));
  vec3 b_center = {6, -3, 0};
  float b_radius = 5;

  float b_angle = 0.645;
  float b_start_angle = b_angle;
  float b_end_angle = pi - b_angle;
  std::vector<vec3> b_circle =
      make_circle(b_center, b_radius, b_start_angle, b_end_angle);

  // circle blue bottom
  glBegin(GL_TRIANGLE_FAN);
  draw(vs[5]);
  for (vec3 &v : b_circle) {
    draw(v);
  }
  glEnd();

  // circle blue top
  vec3 b_center_h = vec3(b_center[0], b_center[1], h);
  std::vector<vec3> b_circle_h = {};
  for (vec3 &v : b_circle) {
    b_circle_h.push_back(vec3(v[0], v[1], h));
  }

  glBegin(GL_TRIANGLE_FAN);
  draw(vs_h[5]);
  for (vec3 &v : b_circle_h) {
    draw(v);
  }
  glEnd();

  // circle blue wall
  colorize(color_wall_blue);
  int b_circle_dots = b_circle.size();
  for (int i = 0; i < b_circle_dots; i++) {
    if (i == b_circle_dots - 1)
      continue;
    int nextI = (i + 1) % b_circle_dots;

    glBegin(GL_QUADS);
    draw(b_circle[i]);
    draw(b_circle[nextI]);
    draw(b_circle_h[nextI]);
    draw(b_circle_h[i]);
    glEnd();
  }
}

void Render(double delta_time) { my_render(delta_time); }
