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

#include "stb_image.h"

bool textureMode = true;
bool lightMode = true;

// класс для настройки камеры
class CustomCamera : public Camera {
public:
  // дистанция камеры
  double camDist;
  // углы поворота камеры
  double fi1, fi2;

  // значния масеры по умолчанию
  CustomCamera() {
    camDist = 15;
    fi1 = 1;
    fi2 = 1;
  }

  // считает позицию камеры, исходя из углов поворота, вызывается движком
  void SetUpCamera() {
    // отвечает за поворот камеры мышкой
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
    // функция настройки камеры
    gluLookAt(pos.X(), pos.Y(), pos.Z(), lookPoint.X(), lookPoint.Y(),
              lookPoint.Z(), normal.X(), normal.Y(), normal.Z());
  }

} camera; // создаем объект камеры

// Класс для настройки света
class CustomLight : public Light {
public:
  CustomLight() {
    // начальная позиция света
    pos = Vector3(1, 1, 3);
  }

  // рисует сферу и линии под источником света, вызывается движком
  void DrawLightGhismo() {
    glDisable(GL_LIGHTING);

    glColor3d(0.9, 0.8, 0);
    Sphere s;
    s.pos = pos;
    s.scale = s.scale * 0.08;
    s.Show();

    if (OpenGL::isKeyPressed('G')) {
      glColor3d(0, 0, 0);
      // линия от источника света до окружности
      glBegin(GL_LINES);
      glVertex3d(pos.X(), pos.Y(), pos.Z());
      glVertex3d(pos.X(), pos.Y(), 0);
      glEnd();

      // рисуем окруность
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

    // параметры источника света
    glLightfv(GL_LIGHT0, GL_POSITION, position);
    // характеристики излучаемого света
    // фоновое освещение (рассеянный свет)
    glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
    // диффузная составляющая света
    glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
    // зеркально отражаемая составляющая света
    glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

    glEnable(GL_LIGHT0);
  }

} light; // создаем источник света

// старые координаты мыши
int mouseX = 0, mouseY = 0;

void mouseEvent(OpenGL *ogl, int mX, int mY) {
  int dx = mouseX - mX;
  int dy = mouseY - mY;
  mouseX = mX;
  mouseY = mY;

  // меняем углы камеры при нажатой левой кнопке мыши
  if (OpenGL::isKeyPressed(VK_RBUTTON)) {
    camera.fi1 += 0.01 * dx;
    camera.fi2 += -0.01 * dy;
  }

  // двигаем свет по плоскости, в точку где мышь
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
// выполняется перед первым рендером
void initRender(OpenGL *ogl) {
  // настройка текстур

  // 4 байта на хранение пикселя
  glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

  // настройка режима наложения текстур
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

  // включаем текстуры
  glEnable(GL_TEXTURE_2D);

  // массив трехбайтных элементов  (R G B)
  RGBTRIPLE *texarray;

  // массив символов, (высота*ширина*4      4, потомучто   выше, мы указали
  // использовать по 4 байта на пиксель текстуры - R G B A)
  //char *texCharArray;
  int cnt;
  unsigned char* texCharArray = stbi_load("uv1024.jpg", &texW, &texH, &cnt, 0);
  //OpenGL::LoadBMP("texture.bmp", &texW, &texH, &texarray);
  //OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);

  // генерируем ИД для текстуры
  glGenTextures(1, &texId);
  // биндим айдишник, все что будет происходить с текстурой, будте происходить
  // по этому ИД
  glBindTexture(GL_TEXTURE_2D, texId);

  // загружаем текстуру в видеопямять, в оперативке нам больше  она не нужна
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, texCharArray);

  // отчистка памяти
  stbi_image_free(texCharArray);
  //free(texCharArray);
  free(texarray);

  // наводим шмон
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

  // камеру и свет привязываем к "движку"
  ogl->mainCamera = &camera;
  ogl->mainLight = &light;

  // нормализация нормалей : их длины будет равна 1
  glEnable(GL_NORMALIZE);

  // устранение ступенчатости для линий
  glEnable(GL_LINE_SMOOTH);

  //   задать параметры освещения
  //  параметр GL_LIGHT_MODEL_TWO_SIDE -
  //                0 -  лицевые и изнаночные рисуются одинаково(по умолчанию),
  //                1 - лицевые и изнаночные обрабатываются разными режимами
  //                соответственно лицевым и изнаночным свойствам материалов.
  //  параметр GL_LIGHT_MODEL_AMBIENT - задать фоновое освещение,
  //                не зависящее от сточников
  // по умолчанию (0.2, 0.2, 0.2, 1.0)

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
  void tex(double scale, double teW, double texH) {
    vec3 v = {(data[0] * scale) / texW, (texH - data[1] * scale) / texH,
              data[2]};
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
  // Вычисляем векторы, образованные двумя сторонами треугольника
  vec3 edge1 = {v2[0] - v1[0], v2[1] - v1[1], v2[2] - v1[2]};
  vec3 edge2 = {v3[0] - v1[0], v3[1] - v1[1], v3[2] - v1[2]};

  // Находим нормаль как векторное произведение этих двух сторон
  vec3 normal = {edge1[1] * edge2[2] - edge1[2] * edge2[1],
                 edge1[2] * edge2[0] - edge1[0] * edge2[2],
                 edge1[0] * edge2[1] - edge1[1] * edge2[0]};

  // Нормализуем нормаль, чтобы получить единичный вектор
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
  const int cell = 5; // px
  glDisable(GL_TEXTURE_2D);
  glDisable(GL_LIGHTING);

  glEnable(GL_DEPTH_TEST);
  if (textureMode)
    glEnable(GL_TEXTURE_2D);

  if (lightMode)
    glEnable(GL_LIGHTING);

  // альфаналожение
  // glEnable(GL_BLEND);
  // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // настройка материала
  GLfloat amb[] = {0.2, 0.2, 0.1, 1.};
  GLfloat dif[] = {0.4, 0.65, 0.5, 1.};
  GLfloat spec[] = {0.9, 0.8, 0.3, 1.};
  GLfloat sh = 0.1f * 256;

  // фоновая
  glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
  // дифузная
  glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
  // зеркальная
  glMaterialfv(GL_FRONT, GL_SPECULAR, spec); // размер блика
  glMaterialf(GL_FRONT, GL_SHININESS, sh);

  // чтоб было красиво, без квадратиков (сглаживание освещения)
  glShadeModel(GL_SMOOTH);

  // Прогать тут
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

  std::vector<vec3> texs = {
      {5, 15, 0},  // 0
      {3, 11, 0},  // 1
      {3, 11, 0},  // 2
      {9, 9, 0},   // 3
      {9, 4, 0},   // 4
      {15, 5, 0},  // 5
      {13, 10, 0}, // 6
      {17, 14, 0}  // 7
  };

  const double h = 3;
  std::vector<vec3> vs_h = {};
  std::vector<vec3> texs_h = {};

  for (vec3 &v : vs) {
    vs_h.push_back(vec3(v[0], v[1], h));
  }
  for (vec3 &v : vs) {
    texs_h.push_back((vec3(v[0], v[1], h)));
  }

  vec3 color_white = rgb_to_normal(vec3(255, 255, 255));
  vec3 color_cover = rgb_to_normal(vec3(200, 200, 200));
  vec3 color_wall = rgb_to_normal(vec3(100, 100, 100));
  vec3 normal_top = vec3(0, 0, 1);
  vec3 normal_bottom = vec3(0, 0, -1);
  color_white.colorize();
  // bottom
  // color_cover.colorize();
  glBegin(GL_TRIANGLES);
  glNormal3dv(normal_bottom.data);
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
  glNormal3dv(normal_top.data);
  // glBindTexture(GL_TEXTURE_2D, texId);
  // texs[0].tex(cell);
  // vs_h[0].draw();
  // texs[1].tex(cell);
  // vs_h[1].draw();
  // texs[2].tex(cell);
  // vs_h[2].draw();

  vec3(5, 5, 0).tex(cell, texW, texH);
  vs_h[0].draw();
  vec3(0, 5, 0).tex(cell, texW, texH);
  vs_h[1].draw();
  vec3(5, 0, 0).tex(cell, texW, texH);
  vs_h[2].draw();
  glEnd();

  // texs[0].tex(cell);
  // vs_h[0].draw();
  // texs[2].tex(cell);
  // vs_h[2].draw();
  // texs[7].tex(cell);
  // vs_h[7].draw();
  /*vs_h[2].draw();
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
  vs_h[7].draw();*/

  // texs[2].tex(cell);
  // texs[3].tex(cell);
  // texs[4].tex(cell);

  // texs[2].tex(cell);
  // texs[4].tex(cell);
  // texs[5].tex(cell);

  // texs[2].tex(cell);
  // texs[5].tex(cell);
  // texs[7].tex(cell);

  // texs[5].tex(cell);
  // texs[6].tex(cell);
  // texs[7].tex(cell);

  glEnd();

  // walls
  glBegin(GL_QUADS);
  color_wall.colorize();
  for (int i = 0; i < vs.size(); i++) {
    vec3 normal = normal_to_triangle(vs[i], vs[(i + i) / vs.size()], vs_h[i]);
    glNormal3dv(normal.data);
    vs[i].draw();
    vs[(i + 1) % vs.size()].draw();
    vs_h[(i + 1) % vs.size()].draw();
    vs_h[i].draw();
  }
  glEnd();

  // circle
  vec3 color_cover_green = rgb_to_normal(vec3(22, 224, 0));
  vec3 color_wall_green = rgb_to_normal(vec3(23, 161, 8));
  vec3 line = vec3((vs[1][0] - vs[0][0]), (vs[1][1] - vs[0][1]), 0);
  vec3 center = vec3((vs[1][0] + vs[0][0]) / 2., (vs[1][1] + vs[0][1]) / 2., 0);
  double radius =
      std::sqrt(std::pow(line[0] / 2., 2) + std::pow(line[1] / 2., 2));
  double start_angle_in_grad = 71.85;
  double end_angle_in_grad = -78.65;
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
    v.draw();
  }
  glEnd();

  // circle wall
  glBegin(GL_QUADS);
  color_wall_green.colorize();
  int circle_dots = circle.size();
  for (int i = 0; i < circle_dots; i++) {
    vec3 normal = normal_to_triangle(circle[i], circle[(i + i) / circle_dots],
                                     circle_h[i]);
    glNormal3dv(normal.data);
    circle[i].draw();
    circle[(i + 1) % circle_dots].draw();
    circle_h[(i + 1) % circle_dots].draw();
    circle_h[i].draw();
  }
  glEnd();

  // ================================================================================
  // Сообщение вверху экрана

  glMatrixMode(
      GL_PROJECTION); // Делаем активной матрицу проекций.
                      //(всек матричные операции, будут ее видоизменять.)
  glPushMatrix(); // сохраняем текущую матрицу проецирования (которая описывает
                  // перспективную проекцию) в стек
  glLoadIdentity(); // Загружаем единичную матрицу
  glOrtho(0, ogl->getWidth(), 0, ogl->getHeight(), 0,
          1); // врубаем режим ортогональной проекции

  glMatrixMode(GL_MODELVIEW); // переключаемся на модел-вью матрицу
  glPushMatrix(); // сохраняем текущую матрицу в стек (положение камеры,
                  // фактически)
  glLoadIdentity(); // сбрасываем ее в дефолт

  glDisable(GL_LIGHTING);

  GuiTextRectangle
      rec; // классик моего авторства для удобной работы с рендером текста.
  rec.setSize(300, 150);
  rec.setPosition(10, ogl->getHeight() - 150 - 10);

  std::stringstream ss;
  ss << "T - вкл/выкл текстур" << std::endl;
  ss << "L - вкл/выкл освещение" << std::endl;
  ss << "F - Свет из камеры" << std::endl;
  ss << "G - двигать свет по горизонтали" << std::endl;
  ss << "G+ЛКМ двигать свет по вертекали" << std::endl;
  ss << "Коорд. света: (" << light.pos.X() << ", " << light.pos.Y() << ", "
     << light.pos.Z() << ")" << std::endl;
  ss << "Коорд. камеры: (" << camera.pos.X() << ", " << camera.pos.Y() << ", "
     << camera.pos.Z() << ")" << std::endl;
  ss << "Параметры камеры: R=" << camera.camDist << ", fi1=" << camera.fi1
     << ", fi2=" << camera.fi2 << std::endl;

  rec.setText(ss.str().c_str());
  rec.Draw();

  glMatrixMode(GL_PROJECTION); // восстанавливаем матрицы проекции и модел-вью
                               // обратьно из стека.
  glPopMatrix();

  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
}