
#include <Windows.h>

#include "Render.h"
#include <GL\GL.h>
#include <GL\GLU.h>
#include <cmath>
#include <vector>

const double pi = 3.1415;

struct vec3 {
  double data[3];

  // Constructor to initialize elements
  vec3(double x, double y, double z) {
    data[0] = x;
    data[1] = y;
    data[2] = z;
  }

  // Overload operator[] for accessing elements by index
  double &operator[](int index) { return data[index]; }

  // Overload const version of operator[] for const objects
  const double &operator[](int index) const { return data[index]; }
  void draw() { glVertex3dv(data); }
  void colorize() { glColor3dv(data); }
  double *raw() { return data; }
};

vec3 rgb_to_normal(vec3 rgb) {
  return vec3(rgb[0] / 255., rgb[1] / 255., rgb[2] / 255.);
}

std::vector<vec3> make_circle(vec3 center, double radius, double start_angle,
                              double end_angle) {
  const double circle_points = 100;
  std::vector<vec3> circle = {};

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

double normalize_angle_in_rad(double angle) {
  return (2 * pi + angle) / (2 * pi);
}

double h = 3;
bool isReversed = false;

void Render(double delta_time) {
  if (isReversed)
    delta_time = -delta_time;

  h += delta_time;
  if (h > 5) {
    h = 5;
    isReversed = true;
  } else if (h < 3) {
    h = 3;
    isReversed = false;
  }

  std::vector<vec3> vs = {
      {2, 3, 0},   // 0 green
      {1, 8, 0},   // 1 green
      {7, 10, 0},  // 2
      {7, 15, 0},  // 3 blue
      {13, 14, 0}, // 4 blue
      {11, 9, 0},  // 5
      {15, 5, 0},  // 6
      {9, 8, 0},   // 7
  };

  std::vector<vec3> vs_h = {};
  for (vec3 &v : vs) {
    vs_h.push_back(vec3(v[0], v[1], h));
  }

  vec3 color_cover = rgb_to_normal(vec3(200, 200, 200));
  vec3 color_wall = rgb_to_normal(vec3(100, 100, 100));

  // bottom
  color_cover.colorize();

  glBegin(GL_TRIANGLES);
  vs[0].draw();
  vs[1].draw();
  vs[2].draw();

  vs[0].draw();
  vs[2].draw();
  vs[7].draw();

  vs[2].draw();
  vs[5].draw();
  vs[7].draw();

  vs[5].draw();
  vs[6].draw();
  vs[7].draw();
  glEnd();

  // top

  glBegin(GL_TRIANGLES);
  vs_h[0].draw();
  vs_h[1].draw();
  vs_h[2].draw();

  vs_h[0].draw();
  vs_h[2].draw();
  vs_h[7].draw();

  vs_h[2].draw();
  vs_h[5].draw();
  vs_h[7].draw();

  vs_h[5].draw();
  vs_h[6].draw();
  vs_h[7].draw();
  glEnd();

  // walls
  color_wall.colorize();
  glBegin(GL_QUADS);
  for (int i = 0; i < vs.size(); i++) {
    if (i == 3)
      continue;

    vs[i].draw();
    vs[(i + 1) % vs.size()].draw();
    vs_h[(i + 1) % vs.size()].draw();
    vs_h[i].draw();
  }
  glEnd();

  // circle green
  vec3 color_cover_green = rgb_to_normal(vec3(22, 224, 0));
  vec3 color_wall_green = rgb_to_normal(vec3(23, 161, 8));
  vec3 green_line = vec3((vs[1][0] - vs[0][0]), (vs[1][1] - vs[0][1]), 0);
  vec3 green_center =
      vec3((vs[1][0] + vs[0][0]) / 2., (vs[1][1] + vs[0][1]) / 2., 0);
  double green_radius = std::sqrt(std::pow(green_line[0] / 2., 2) +
                                  std::pow(green_line[1] / 2., 2));
  double green_start_angle = 1.7;
  double green_end_angle = green_start_angle + pi + 0.1;
  std::vector<vec3> green_circle = make_circle(
      green_center, green_radius, green_start_angle, green_end_angle);

  // circle green bottom
  glBegin(GL_TRIANGLE_FAN);
  color_cover_green.colorize();
  green_center.draw();
  for (vec3 &v : green_circle) {
    v.draw();
  }
  glEnd();

  // circle green top
  vec3 green_center_h = vec3(green_center[0], green_center[1], h);
  std::vector<vec3> green_circle_h = {};
  for (vec3 &v : green_circle) {
    green_circle_h.push_back(vec3(v[0], v[1], h));
  }

  glBegin(GL_TRIANGLE_FAN);
  green_center_h.draw();
  for (vec3 &v : green_circle_h) {
    v.draw();
  }
  glEnd();

  // circle green wall
  glBegin(GL_QUADS);
  color_wall_green.colorize();
  int green_circle_dots = green_circle.size();
  for (int i = 0; i < green_circle_dots; i++) {
    green_circle[i].draw();
    green_circle[(i + 1) % green_circle_dots].draw();
    green_circle_h[(i + 1) % green_circle_dots].draw();
    green_circle_h[i].draw();
  }
  glEnd();

  // circle blue
  vec3 color_cover_blue = rgb_to_normal(vec3(88, 105, 218));
  vec3 color_wall_blue = rgb_to_normal(vec3(61, 82, 213));
  vec3 blue_line = vec3((vs[4][0] - vs[3][0]), (vs[4][1] - vs[3][1]), 0);
  double blue_start_angle = pi + 0.5;
  double blue_end_angle = 2 * pi - 0.82;
  vec3 blue_center = vec3(10.38889, 16.83333, 0);
  double blue_radius = 3.85301;

  std::vector<vec3> blue_circle =
      make_circle(blue_center, blue_radius, blue_start_angle, blue_end_angle);

  // circle blue bottom
  glBegin(GL_TRIANGLE_FAN);
  color_cover_blue.colorize();
  vs[2].draw();
  for (int i = 0; i < blue_circle.size(); i++) {
    blue_circle[i].draw();
  }
  glEnd();

  glBegin(GL_TRIANGLES);
  vs[2].draw();
  vs[4].draw();
  vs[5].draw();
  glEnd();

  // circle blue top
  vec3 blue_center_h = vec3(blue_center[0], blue_center[1], h);
  std::vector<vec3> blue_circle_h = {};
  for (vec3 &v : blue_circle) {
    blue_circle_h.push_back(vec3(v[0], v[1], h));
  }

  glBegin(GL_TRIANGLE_FAN);
  color_cover_blue.colorize();
  vs_h[2].draw();
  for (int i = 0; i < blue_circle.size(); i++) {
    blue_circle_h[i].draw();
  }
  glEnd();

  glBegin(GL_TRIANGLES);
  vs_h[2].draw();
  vs_h[4].draw();
  vs_h[5].draw();
  glEnd();

  // circle blue wall
  glBegin(GL_QUADS);
  color_wall_blue.colorize();
  int blue_circle_dots = blue_circle.size();
  for (int i = 0; i < blue_circle_dots; i++) {
    if (i == blue_circle_dots - 1)
      continue;
    blue_circle[i].draw();
    blue_circle[(i + 1) % blue_circle_dots].draw();
    blue_circle_h[(i + 1) % blue_circle_dots].draw();
    blue_circle_h[i].draw();
  }
  glEnd();
}
