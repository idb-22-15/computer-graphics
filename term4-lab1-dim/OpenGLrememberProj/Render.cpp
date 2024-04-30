
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

std::vector<vec3> make_circle(vec3 center, double radius, double start_angle, double end_angle) {
    const double circle_points = 100;
    std::vector<vec3> circle = {};

    for (double i = 0; i < circle_points; i++) {
        const double angle =
            start_angle + (std::abs(end_angle - start_angle)) / (circle_points - 1) * i;
        vec3 v( center[0] + std::cos(angle) * radius,
                center[1] + std::sin(angle) * radius,
                center[2]);
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

void Render(double delta_time) {
  std::vector<vec3> vs = {
      {2, 3, 0},   // 0 green
      {1, 8, 0},   // 1 green
      {7, 10, 0},  // 2
      {7, 15, 0},  // 3
      {13, 14, 0}, // 4
      {11, 9, 0},  // 5
      {15, 5, 0},  // 6
      {9, 8, 0},   // 7
  };

  const double h = 3;
  std::vector<vec3> vs_h = {};
  for (vec3 &v : vs) {
    vs_h.push_back(vec3(v[0], v[1], h));
  }

  vec3 color_cover = rgb_to_normal(vec3(100, 100, 100));
  vec3 color_wall = rgb_to_normal(vec3(200, 200, 200));

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
  vs[3].draw();
  vs[4].draw();

  vs[2].draw();
  vs[4].draw();
  vs[5].draw();

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
  vs_h[3].draw();
  vs_h[4].draw();

  vs_h[2].draw();
  vs_h[4].draw();
  vs_h[5].draw();

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
      vs[i].draw();
      vs[(i + 1) % vs.size()].draw();
      vs_h[(i + 1) % vs.size()].draw();
      vs_h[i].draw();
  }
  glEnd();

  // circle
  vec3 line = vec3((vs[1][0] - vs[0][0]), (vs[1][1] - vs[0][1]), 0);
  vec3 center = vec3((vs[1][0] + vs[0][0]) / 2., (vs[1][1] + vs[0][1]) / 2., 0);
  double radius = std::sqrt(std::pow(line[0] / 2., 2) + std::pow(line[1] / 2., 2));
  double start_angle = 1.7;
  double end_angle = start_angle + pi + 0.1;
  std::vector circle = make_circle(center, radius, start_angle, end_angle);
  
  // bottom
  glBegin(GL_TRIANGLE_FAN);
  vec3(0, 255, 0).colorize();
  center.draw();
  for (vec3& v : circle) {
      v.draw();
  }
  glEnd();

  // top
  vec3 center_h = vec3(center[0], center[1], h);
  std::vector<vec3> circle_h = {};
  for (vec3& v : circle) {
      circle_h.push_back(vec3(v[0], v[1], h));
  }

  glBegin(GL_TRIANGLE_FAN);
  vec3(0, 255, 0).colorize();
  center_h.draw();
  for (vec3& v : circle_h) {
      v.draw();
  }
  glEnd();

  // wall
  glBegin(GL_QUADS);
  vec3(50, 100, 0).colorize();
  int circle_dots = circle.size();
    for (int i = 0; i < circle_dots; i++) {
        circle[i].draw();
        circle[(i + 1) % circle_dots].draw();
        circle_h[(i + 1) % circle_dots].draw();
        circle_h[i].draw();
    }
  glEnd();
}
