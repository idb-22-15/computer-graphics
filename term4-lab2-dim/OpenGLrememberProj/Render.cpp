#include <windows.h>

#include "Render.h"

#include <iostream>
#include <sstream>

#include <GL\GL.h>
#include <GL\GLU.h>

#include "MyOGL.h"

#include "Camera.h"
#include "Light.h"
#include "Primitives.h"

#include "GUItextRectangle.h"

bool textureMode = true;
bool lightMode = true;

// ����� ��� ��������� ������
class CustomCamera : public Camera {
public:
  // ��������� ������
  double camDist;
  // ���� �������� ������
  double fi1, fi2;

  // ������� ������ �� ���������
  CustomCamera() {
    camDist = 15;
    fi1 = 1;
    fi2 = 1;
  }

  // ������� ������� ������, ������ �� ����� ��������, ���������� �������
  void SetUpCamera() {
    // �������� �� ������� ������ ������
    lookPoint.setCoords(0, 0, 0);

    pos.setCoords(camDist * cos(fi2) * cos(fi1), camDist * cos(fi2) * sin(fi1),
                  camDist * sin(fi2));

    if (cos(fi2) <= 0)
      normal.setCoords(0, 0, -1);
    else
      normal.setCoords(0, 0, 1);

    LookAt();
  }

  void CustomCamera::LookAt() {
    // ������� ��������� ������
    gluLookAt(pos.X(), pos.Y(), pos.Z(), lookPoint.X(), lookPoint.Y(),
              lookPoint.Z(), normal.X(), normal.Y(), normal.Z());
  }

} camera; // ������� ������ ������

// ����� ��� ��������� �����
class CustomLight : public Light {
public:
  CustomLight() {
    // ��������� ������� �����
    pos = Vector3(1, 1, 3);
  }

  // ������ ����� � ����� ��� ���������� �����, ���������� �������
  void DrawLightGhismo() {
    glDisable(GL_LIGHTING);

    glColor3d(0.9, 0.8, 0);
    Sphere s;
    s.pos = pos;
    s.scale = s.scale * 0.08;
    s.Show();

    if (OpenGL::isKeyPressed('G')) {
      glColor3d(0, 0, 0);
      // ����� �� ��������� ����� �� ����������
      glBegin(GL_LINES);
      glVertex3d(pos.X(), pos.Y(), pos.Z());
      glVertex3d(pos.X(), pos.Y(), 0);
      glEnd();

      // ������ ���������
      Circle c;
      c.pos.setCoords(pos.X(), pos.Y(), 0);
      c.scale = c.scale * 1.5;
      c.Show();
    }
  }

  void SetUpLight() {
    GLfloat amb[] = {0.2, 0.2, 0.2, 0};
    GLfloat dif[] = {1.0, 1.0, 1.0, 0};
    GLfloat spec[] = {.7, .7, .7, 0};
    GLfloat position[] = {pos.X(), pos.Y(), pos.Z(), 1.};

    // ��������� ��������� �����
    glLightfv(GL_LIGHT0, GL_POSITION, position);
    // �������������� ����������� �����
    // ������� ��������� (���������� ����)
    glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
    // ��������� ������������ �����
    glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
    // ��������� ���������� ������������ �����
    glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

    glEnable(GL_LIGHT0);
  }

} light; // ������� �������� �����

// ������ ���������� ����
int mouseX = 0, mouseY = 0;

void mouseEvent(OpenGL *ogl, int mX, int mY) {
  int dx = mouseX - mX;
  int dy = mouseY - mY;
  mouseX = mX;
  mouseY = mY;

  // ������ ���� ������ ��� ������� ����� ������ ����
  if (OpenGL::isKeyPressed(VK_RBUTTON)) {
    camera.fi1 += 0.01 * dx;
    camera.fi2 += -0.01 * dy;
  }

  // ������� ���� �� ���������, � ����� ��� ����
  if (OpenGL::isKeyPressed('G') && !OpenGL::isKeyPressed(VK_LBUTTON)) {
    LPPOINT POINT = new tagPOINT();
    GetCursorPos(POINT);
    ScreenToClient(ogl->getHwnd(), POINT);
    POINT->y = ogl->getHeight() - POINT->y;

    Ray r = camera.getLookRay(POINT->x, POINT->y);

    double z = light.pos.Z();

    double k = 0, x = 0, y = 0;
    if (r.direction.Z() == 0)
      k = 0;
    else
      k = (z - r.origin.Z()) / r.direction.Z();

    x = k * r.direction.X() + r.origin.X();
    y = k * r.direction.Y() + r.origin.Y();

    light.pos = Vector3(x, y, z);
  }

  if (OpenGL::isKeyPressed('G') && OpenGL::isKeyPressed(VK_LBUTTON)) {
    light.pos = light.pos + Vector3(0, 0, 0.02 * dy);
  }
}

void mouseWheelEvent(OpenGL *ogl, int delta) {

  if (delta < 0 && camera.camDist <= 1)
    return;
  if (delta > 0 && camera.camDist >= 100)
    return;

  camera.camDist += 0.01 * delta;
}

void keyDownEvent(OpenGL *ogl, int key) {
  if (key == 'L') {
    lightMode = !lightMode;
  }

  if (key == 'T') {
    textureMode = !textureMode;
  }

  if (key == 'R') {
    camera.fi1 = 1;
    camera.fi2 = 1;
    camera.camDist = 15;

    light.pos = Vector3(1, 1, 3);
  }

  if (key == 'F') {
    light.pos = camera.pos;
  }
}

void keyUpEvent(OpenGL *ogl, int key) {}

GLuint texId;
int texW, texH;
// ����������� ����� ������ ��������
void initRender(OpenGL *ogl) {
  // ��������� �������

  // 4 ����� �� �������� �������
  glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

  // ��������� ������ ��������� �������
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

  // �������� ��������
  glEnable(GL_TEXTURE_2D);

  // ������ ����������� ���������  (R G B)
  RGBTRIPLE *texarray;

  // ������ ��������, (������*������*4      4, ���������   ����, �� �������
  // ������������ �� 4 ����� �� ������� �������� - R G B A)
  char *texCharArray;
  int cnt;
  // unsigned char* texCharArray = stbi_load("uv1024.jpg", &texW, &texH, &cnt,
  // 0);
  OpenGL::LoadBMP("tex2.bmp", &texW, &texH, &texarray);
  OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);

  // ���������� �� ��� ��������
  glGenTextures(1, &texId);
  // ������ ��������, ��� ��� ����� ����������� � ���������, ����� �����������
  // �� ����� ��
  glBindTexture(GL_TEXTURE_2D, texId);

  // ��������� �������� � �����������, � ���������� ��� ������  ��� �� �����
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, texCharArray);

  // �������� ������
  // stbi_image_free(texCharArray);
  free(texCharArray);
  free(texarray);

  // ������� ����
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

  // ������ � ���� ����������� � "������"
  ogl->mainCamera = &camera;
  ogl->mainLight = &light;

  // ������������ �������� : �� ����� ����� ����� 1
  glEnable(GL_NORMALIZE);

  // ���������� ������������� ��� �����
  glEnable(GL_LINE_SMOOTH);

  //   ������ ��������� ���������
  //  �������� GL_LIGHT_MODEL_TWO_SIDE -
  //                0 -  ������� � ���������� �������� ���������(�� ���������),
  //                1 - ������� � ���������� �������������� ������� ��������
  //                �������������� ������� � ���������� ��������� ����������.
  //  �������� GL_LIGHT_MODEL_AMBIENT - ������ ������� ���������,
  //                �� ��������� �� ���������
  // �� ��������� (0.2, 0.2, 0.2, 1.0)

  glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);

  camera.fi1 = -1.3;
  camera.fi2 = 0.8;
}

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
  void tex(double scale, double texW, double texH) {
    vec3 v = {(data[0] * scale) / (double)texW,
              (texH - data[1] * scale) / (double)texH, data[2]};
    glTexCoord3dv(v.data);
  }
  double len() {
    return std::sqrt(data[0] * data[0] + data[1] * data[1] + data[2] * data[2]);
  }
  vec3 normalize() {
    double l = len();
    return vec3(data[0] / l, data[1] / l, data[2] / l);
  }
};

vec3 normal_to_triangle(const vec3 &v1, const vec3 &v2, const vec3 &v3) {
  // ��������� �������, ������������ ����� ��������� ������������
  vec3 a = {v2[0] - v1[0], v2[1] - v1[1], v2[2] - v1[2]};
  vec3 b = {v3[0] - v1[0], v3[1] - v1[1], v3[2] - v1[2]};

  // ������� ������� ��� ��������� ������������ ���� ���� ������
  vec3 normal = {a[1] * b[2] - b[1] * a[2],  //
                 -a[0] * b[2] + b[0] * a[2], //
                 a[0] * b[1] - b[0] * a[1]}; //

  // ����������� �������, ����� �������� ��������� ������
  return normal.normalize();
}

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

void Render(OpenGL *ogl) {
  glDisable(GL_TEXTURE_2D);
  glDisable(GL_LIGHTING);
  glEnable(GL_COLOR_MATERIAL);
  glEnable(GL_NORMALIZE);
  glEnable(GL_DEPTH_TEST);
  if (textureMode)
    glEnable(GL_TEXTURE_2D);

  if (lightMode)
    glEnable(GL_LIGHTING);

  // ��������������
  // glEnable(GL_BLEND);
  // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // ��������� ���������
  GLfloat amb[] = {0.2, 0.2, 0.1, 1.};
  GLfloat dif[] = {0.4, 0.65, 0.5, 1.};
  GLfloat spec[] = {0.9, 0.8, 0.3, 1.};
  GLfloat sh = 0.1f * 256;

  // �������
  glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
  // ��������
  glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
  // ����������
  glMaterialfv(GL_FRONT, GL_SPECULAR, spec); // ������ �����
  glMaterialf(GL_FRONT, GL_SHININESS, sh);

  // ���� ���� �������, ��� ����������� (����������� ���������)
  glShadeModel(GL_SMOOTH);

  // ������� ���
  //================================================================================
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
  std::vector<vec3> texs_h = {
      {4, 16, h},  // 0
      {3, 11, h},  // 1
      {9, 9, h},   // 2
      {9, 4, h},   // 3
      {15, 5, h},  // 4
      {13, 10, h}, // 5
      {17, 14, h}, // 6
      {11, 11, h}  // 7
  };

  std::vector<vec3> tesx_w = {
      /* {1, 9, 0},
       {1, 6, h},
       {1, 6 + 6.325, h},
       {}*/
  };

  std::vector<vec3> vs_h = {};
  std::vector<vec3> texs = {};

  for (vec3 &v : vs) {
    vs_h.push_back(vec3(v[0], v[1], h));
  }
  for (vec3 &v : vs) {
    texs.push_back((vec3(v[0], v[1], 0)));
  }

  vec3 color_white = rgb_to_normal(vec3(255, 255, 255));
  vec3 color_cover = rgb_to_normal(vec3(200, 200, 200));
  vec3 color_wall = rgb_to_normal(vec3(100, 100, 100));
  vec3 normal_top = vec3(0, 0, 1);
  vec3 normal_bottom = vec3(0, 0, -1);
  const int cell = 40; // px
  glBindTexture(GL_TEXTURE_2D, texId);
  color_white.colorize();
  // bottom
  // color_cover.colorize();
  glBegin(GL_TRIANGLES);
  glNormal3dv(normal_bottom.data);
  texs[0].tex(cell, texW, texH);
  vs[0].draw();
  texs[1].tex(cell, texW, texH);
  vs[1].draw();
  texs_h[2].tex(cell, texW, texH);
  vs[2].draw();

  texs[0].tex(cell, texW, texH);
  vs[0].draw();
  texs[2].tex(cell, texW, texH);
  vs[2].draw();
  texs[7].tex(cell, texW, texH);
  vs[7].draw();

  texs[2].tex(cell, texW, texH);
  vs[2].draw();
  texs[3].tex(cell, texW, texH);
  vs[3].draw();
  texs[4].tex(cell, texW, texH);
  vs[4].draw();

  texs[2].tex(cell, texW, texH);
  vs[2].draw();
  texs[4].tex(cell, texW, texH);
  vs[4].draw();
  texs[5].tex(cell, texW, texH);
  vs[5].draw();

  texs[2].tex(cell, texW, texH);
  vs[2].draw();
  texs[5].tex(cell, texW, texH);
  vs[5].draw();
  texs[7].tex(cell, texW, texH);
  vs[7].draw();

  texs[5].tex(cell, texW, texH);
  vs[5].draw();
  texs[6].tex(cell, texW, texH);
  vs[6].draw();
  texs[7].tex(cell, texW, texH);
  vs[7].draw();
  glEnd();

  // top
  glBegin(GL_TRIANGLES);
  vec3 n = normal_to_triangle(vs_h[0], vs_h[1], vs_h[2]);
  glNormal3dv(normal_top.data);
  texs_h[0].tex(cell, texW, texH);
  vs_h[0].draw();
  texs_h[1].tex(cell, texW, texH);
  vs_h[1].draw();
  texs_h[2].tex(cell, texW, texH);
  vs_h[2].draw();

  texs_h[0].tex(cell, texW, texH);
  vs_h[0].draw();
  texs_h[2].tex(cell, texW, texH);
  vs_h[2].draw();
  texs_h[7].tex(cell, texW, texH);
  vs_h[7].draw();

  texs_h[2].tex(cell, texW, texH);
  vs_h[2].draw();
  texs_h[3].tex(cell, texW, texH);
  vs_h[3].draw();
  texs_h[4].tex(cell, texW, texH);
  vs_h[4].draw();

  texs_h[2].tex(cell, texW, texH);
  vs_h[2].draw();
  texs_h[4].tex(cell, texW, texH);
  vs_h[4].draw();
  texs_h[5].tex(cell, texW, texH);
  vs_h[5].draw();

  texs_h[2].tex(cell, texW, texH);
  vs_h[2].draw();
  texs_h[5].tex(cell, texW, texH);
  vs_h[5].draw();
  texs_h[7].tex(cell, texW, texH);
  vs_h[7].draw();

  texs_h[5].tex(cell, texW, texH);
  vs_h[5].draw();
  texs_h[6].tex(cell, texW, texH);
  vs_h[6].draw();
  texs_h[7].tex(cell, texW, texH);
  vs_h[7].draw();
  glEnd();

  // walls
  // color_wall.colorize();
  color_white.colorize();
  for (int i = 0; i < vs.size(); i++) {
    int nextI = (i + 1) % vs.size();
    vec3 normal = {0,0,0};
    normal = normal_to_triangle(vs_h[i], vs[nextI], vs[i]);
    glNormal3dv(normal.data);
    glBegin(GL_QUADS);
    
    vs[i].draw();
    vs[nextI].draw();
    vs_h[nextI].draw();
    vs_h[i].draw();
    glEnd();

    glLineWidth(3);
    glBegin(GL_LINES);
    vs[i].draw();
    vec3(vs[i][0] + normal[0], vs[i][1] + normal[1], vs[i][2] + normal[2]).draw();
    vs[nextI].draw();
    vec3(vs[nextI][0] + normal[0], vs[nextI][1] + normal[1], vs[nextI][2] + normal[2]).draw();
    vs_h[nextI].draw();
    vec3(vs_h[(i + 1) % vs.size()][0] + normal[0], vs_h[nextI][1] + normal[1], vs_h[nextI][2] + normal[2]).draw();
    vs_h[i].draw();
    vec3(vs_h[i][0] + normal[0], vs_h[i][1] + normal[1], vs_h[i][2] + normal[2]).draw();
    glEnd();
    glLineWidth(1);
  }

  // circle
  vec3 color_cover_green = rgb_to_normal(vec3(165, 214, 167));
  vec3 color_wall_green = rgb_to_normal(vec3(197, 225, 165));
  vec3 line = vec3((vs[1][0] - vs[0][0]), (vs[1][1] - vs[0][1]), 0);
  vec3 center = vec3((vs[1][0] + vs[0][0]) / 2., (vs[1][1] + vs[0][1]) / 2., 0);
  double radius =
      std::sqrt(std::pow(line[0] / 2., 2) + std::pow(line[1] / 2., 2));
  double start_angle = 1.7;
  double end_angle = start_angle + pi + 0.1;
  std::vector<vec3> circle =
      make_circle(center, radius, start_angle, end_angle);

  // circle bottom
  glBegin(GL_TRIANGLE_FAN);
  glNormal3dv(normal_bottom.data);
  color_cover_green.colorize();
  center.draw();
  for (vec3 &v : circle) {
    v.tex(cell, texW, texH);
    v.draw();
  }
  glEnd();

  // circle top
  vec3 center_h = vec3(center[0], center[1], h);
  std::vector<vec3> circle_h = {};
  for (vec3 &v : circle) {
    circle_h.push_back(vec3(v[0], v[1], h));
  }

  glBegin(GL_TRIANGLE_FAN);
  glNormal3dv(normal_top.data);
  color_cover_green.colorize();
  center_h.draw();
  for (vec3 &v : circle_h) {
    v.tex(cell, texW, texH);
    v.draw();
  }
  glEnd();

  // circle wall
  glBegin(GL_QUADS);
  color_wall_green.colorize();
  int circle_dots = circle.size();
  for (int i = 0; i < circle_dots; i++) {
    int nextI = (i + 1) % circle_dots;
    vec3 normal = normal_to_triangle(circle_h[i], circle[nextI],
                                     circle[i]);
    normal[0] = -normal[0];
    normal[1] = -normal[1];
    glNormal3dv(normal.data);
    circle[i].draw();
    circle[(i + 1) % circle_dots].draw();
    circle_h[(i + 1) % circle_dots].draw();
    circle_h[i].draw();
  }
  glEnd();

  // ================================================================================
  // ��������� ������ ������

  glMatrixMode(
      GL_PROJECTION); // ������ �������� ������� ��������.
                      //(���� ��������� ��������, ����� �� ������������.)
  glPushMatrix(); // ��������� ������� ������� ������������� (������� ���������
                  // ������������� ��������) � ����
  glLoadIdentity(); // ��������� ��������� �������
  glOrtho(0, ogl->getWidth(), 0, ogl->getHeight(), 0,
          1); // ������� ����� ������������� ��������

  glMatrixMode(GL_MODELVIEW); // ������������� �� �����-��� �������
  glPushMatrix(); // ��������� ������� ������� � ���� (��������� ������,
                  // ����������)
  glLoadIdentity(); // ���������� �� � ������

  glDisable(GL_LIGHTING);

  GuiTextRectangle
      rec; // ������� ����� ��������� ��� ������� ������ � �������� ������.
  rec.setSize(300, 150);
  rec.setPosition(10, ogl->getHeight() - 150 - 10);

  std::stringstream ss;
  ss << "T - ���/���� �������" << std::endl;
  ss << "L - ���/���� ���������" << std::endl;
  ss << "F - ���� �� ������" << std::endl;
  ss << "G - ������� ���� �� �����������" << std::endl;
  ss << "G+��� ������� ���� �� ���������" << std::endl;
  ss << "�����. �����: (" << light.pos.X() << ", " << light.pos.Y() << ", "
     << light.pos.Z() << ")" << std::endl;
  ss << "�����. ������: (" << camera.pos.X() << ", " << camera.pos.Y() << ", "
     << camera.pos.Z() << ")" << std::endl;
  ss << "��������� ������: R=" << camera.camDist << ", fi1=" << camera.fi1
     << ", fi2=" << camera.fi2 << std::endl;

  rec.setText(ss.str().c_str());
  rec.Draw();

  glMatrixMode(GL_PROJECTION); // ��������������� ������� �������� � �����-���
                               // �������� �� �����.
  glPopMatrix();

  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
}