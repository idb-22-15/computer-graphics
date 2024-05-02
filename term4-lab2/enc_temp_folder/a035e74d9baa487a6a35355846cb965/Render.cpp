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

#include <algorithm>
#include <cmath>
#include <iterator>
#include <numbers>
#include <numeric>
#include <vector>

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
const double scale = 20;

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
  char *texCharArray;

  OpenGL::LoadBMP("tex2.bmp", &texW, &texH, &texarray);
  OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);

  // генерируем ИД для текстуры
  glGenTextures(1, &texId);
  // биндим айдишник, все что будет происходить с текстурой, будте происходить
  // по этому ИД
  glBindTexture(GL_TEXTURE_2D, texId);

  // загружаем текстуру в видеопямять, в оперативке нам больше  она не нужна
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, texCharArray);

  // отчистка памяти
  free(texCharArray);
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

// !======================================================================

const double pi = 3.1415;

struct Vertex {
  double x;
  double y;
  double z;
  Vertex(){};
  Vertex(double x, double y, double z) : x(x), y(y), z(z){};

  double len() { return std::sqrt(x * x + y * y + z * z); }

  Vertex toRGB() { return Vertex(x / 255., y / 255., z / 255.); }

  Vertex toNormalized() {
    double l = len();
    return Vertex(x / l, y / l, z / l);
  }
  void colorize() { glColor3d(x, y, z); }

  void draw() { glVertex3d(x, y, z); }

  void tex(double scale, double w, double h) {
    glTexCoord3d(x * scale / w, y * scale / h, z);
  }

  Vertex operator+(const Vertex &other) {
    return Vertex(x + other.x, y + other.y, z + other.z);
  }

  Vertex operator-(const Vertex &other) {
    return Vertex(x - other.x, y - other.y, z - other.z);
  }

  Vertex operator*(double a) { return Vertex(x * a, y * a, z * a); }

  Vertex operator/(double a) { return Vertex(x / a, y / a, z / a); }
};

struct Line {
  Vertex start;
  Vertex end;
  Vertex mid;
  double length;
  Line(Vertex start, Vertex end) : start(start), end(end) {
    mid = (start + end) / 2.;

    Vertex v = start - end;
    length = v.len();
  }
};

Vertex fig_center(std::vector<Vertex> fig) {
  Vertex sum =
      std::accumulate(fig.begin(), fig.end(), Vertex(0, 0, 0),
                      [](Vertex total, const Vertex &v) { return total + v; });
  return sum / fig.size();
}

double deg_to_rad(double deg) { return deg / 180 * pi; }

double get_angle_in_rad(Vertex start, Vertex end) {
  Vertex v = start - end;
  double angle_in_rad = std::atan2(v.y, v.x);
  return angle_in_rad;
}

Vertex find_normal_of_line(Vertex start, Vertex end, double distance = 1) {
  Vertex dv = end - start;
  Line line = Line(start, end);
  double nx = -dv.y / line.length; //   90 
  double ny = dv.x / line.length;
  Vertex n(nx, ny, start.z);

  double normalX = line.mid.x + n.x * distance;
  double normalY = line.mid.y + n.y * distance;

  return Vertex(normalX, normalY, start.z);
}

const int circle_points = 100;

struct MyCircle {
  Vertex center;
  double radius;
  double start_angle;
  double end_angle;
  std::vector<Vertex> vs;
  MyCircle(Vertex center, double radius, double start_angle, double end_angle)
      : center(center), radius(radius), start_angle(start_angle),
        end_angle(end_angle) {
    for (int i = 0; i < circle_points + 1; i++) {
      const double angle =
          start_angle + (end_angle - start_angle) / circle_points * i;
      Vertex v(center.x + cos(angle) * radius, center.y + sin(angle) * radius,
               center.z);
      vs.push_back(v);
    }
  }

  void draw() {
    for (auto &v : vs) {
      v.tex(scale, texW, texH);
      v.draw();
    }
  }
};

Vertex normal_of_triangle(const Vertex &v1, const Vertex &v2,
                          const Vertex &v3) {
  Vertex b = {v2.x - v1.x, v2.y - v1.y, v2.z - v1.z};
  Vertex a = {v3.x - v1.x, v3.y - v1.y, v3.z - v1.z};

  Vertex normal = {a.y * b.z - b.y * a.z, -a.x * b.z + b.x * a.z,
                   a.x * b.y - b.x * a.y};

  return normal.toNormalized();
}

struct Cover {
  std::vector<Vertex> vs;
  Vertex normal;
  Vertex color;
  MyCircle *green_circle;
  MyCircle *blue_circle;

  Cover(double height, Vertex normal, Vertex color) {
    vs = {
        {10, 1, height},  // green
        {6, 5, height},   //
        {8, 9, height},   //
        {2, 11, height},  //
        {5, 16, height},  // blue
        {13, 13, height}, // blue
        {10, 9, height},  //
        {15, 5, height}   // green
    };
    this->normal = normal;
    this->color = color;
    size_t rows = vs.size();

    // green

    Line green_line = Line(vs[0], vs[7]);
    Vertex green_circle_center = green_line.mid;
    double green_circle_radius = Line(green_circle_center, vs[0]).length;
    double green_circle_start_angle =
        get_angle_in_rad(vs[7], green_circle_center);
    double green_circle_end_angle =
        get_angle_in_rad(vs[0], green_circle_center);
    green_circle =
        new MyCircle(green_circle_center, green_circle_radius,
                     green_circle_start_angle, green_circle_end_angle);

    // blue
    Line blue_line = Line(vs[4], vs[5]);
    Vertex blue_mid = blue_line.mid;
    Vertex blue_normal = find_normal_of_line(vs[4], vs[5]);
    Vertex blue_circle_center = blue_normal;
    double blue_circle_radius = Line(blue_circle_center, vs[4]).length;
    double blue_circle_start_angle =
        get_angle_in_rad(vs[4], blue_circle_center);
    double blue_circle_end_angle =
        get_angle_in_rad(vs[5], blue_circle_center) + 2 * pi;
    blue_circle = new MyCircle(blue_circle_center, blue_circle_radius,
                               blue_circle_start_angle, blue_circle_end_angle);
  }
  ~Cover() {
    delete green_circle;
    delete blue_circle;
  }

  void draw() {
    glBegin(GL_TRIANGLE_FAN);
    glNormal3d(normal.x, normal.y, normal.z);
    color.colorize();
    green_circle->center.tex(scale, texW, texH);
    green_circle->center.draw();
    green_circle->draw();
    glEnd();

    glBegin(GL_TRIANGLE_FAN);
    vs[3].tex(scale, texW, texH);
    vs[3].draw();
    for (int i = 0; i <= circle_points / 2; i++) {
      blue_circle->vs[i].tex(scale, texW, texH);
      blue_circle->vs[i].draw();
    }
    glEnd();

    glBegin(GL_TRIANGLE_FAN);
    vs[2].tex(scale, texW, texH);
    vs[2].draw();
    for (int i = circle_points / 3; i <= circle_points / 3 * 2; i++) {
      blue_circle->vs[i].tex(scale, texW, texH);
      blue_circle->vs[i].draw();
    }
    glEnd();

    glBegin(GL_TRIANGLE_FAN);
    vs[6].tex(scale, texW, texH);
    vs[6].draw();
    for (int i = circle_points / 2; i <= circle_points; i++) {
      blue_circle->vs[i].tex(scale, texW, texH);
      blue_circle->vs[i].draw();
    }
    glEnd();

    glBegin(GL_TRIANGLES);
    vs[3].tex(scale, texW, texH);
    vs[3].draw();
    vs[2].tex(scale, texW, texH);
    vs[2].draw();
    blue_circle->vs[circle_points / 2].tex(scale, texW, texH);
    blue_circle->vs[circle_points / 2].draw();

    vs[2].tex(scale, texW, texH);
    vs[2].draw();
    vs[6].tex(scale, texW, texH);
    vs[6].draw();
    blue_circle->vs[circle_points / 2].tex(scale, texW, texH);
    blue_circle->vs[circle_points / 2].draw();
    glEnd();

    //

    glBegin(GL_POLYGON);
    vs[0].tex(scale, texW, texH);
    vs[0].draw();
    vs[1].tex(scale, texW, texH);
    vs[1].draw();
    vs[2].tex(scale, texW, texH);
    vs[2].draw();
    vs[6].tex(scale, texW, texH);
    vs[6].draw();
    vs[7].tex(scale, texW, texH);
    vs[7].draw();
    glEnd();
  }
};

// !============================================

void Render(OpenGL *ogl) {
  glDisable(GL_TEXTURE_2D);
  glDisable(GL_LIGHTING);

  glEnable(GL_DEPTH_TEST);
  if (textureMode)
    glEnable(GL_TEXTURE_2D);

  if (lightMode)
    glEnable(GL_LIGHTING);

  glBindTexture(GL_TEXTURE_2D, texId);
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

  //!=======================================================
  // Прогать тут
  Vertex color_white = Vertex(255, 255, 255).toRGB();
  color_white.colorize();

  Vertex n = {0, 0, -1};
  Vertex n_top = {0, 0, 1};

  Cover cover(0, n, color_white);
  Cover cover_top(2, n_top, color_white);

  // covers
  cover.draw();
  cover_top.draw();

  // walls
  color_white.colorize();
  for (int i = 0; i < cover.vs.size(); i++) {
    if (i == 4)
      continue;

    int len = cover.vs.size();
    int nextI = (i + 1) % len;
    Vertex n =
        normal_of_triangle(cover.vs[i], cover.vs[nextI], cover_top.vs[i]);

    glNormal3d(n.x, n.y, n.z);

    glBegin(GL_QUADS);
    cover.vs[i].tex(scale, texW, texH);
    cover.vs[i].draw();
    cover.vs[nextI].tex(scale, texW, texH);
    cover.vs[nextI].draw();
    cover_top.vs[nextI].tex(scale, texW, texH);
    cover_top.vs[nextI].draw();
    cover_top.vs[i].tex(scale, texW, texH);
    cover_top.vs[i].draw();
    glEnd();

    glBegin(GL_LINES);
    cover.vs[i].draw();
    (cover.vs[i] + n).draw();
    cover_top.vs[i].draw();
    (cover_top.vs[i] + n).draw();
    cover.vs[nextI].draw();
    (cover.vs[nextI] + n).draw();
    cover_top.vs[nextI].draw();
    (cover_top.vs[nextI] + n).draw();
    glEnd();
  }

  // green circle wall
  for (int i = 0; i < cover.green_circle->vs.size(); i++) {
    glBegin(GL_QUADS);
    int len = cover.green_circle->vs.size();
    int nextI = (i + 1) % len;
    Vertex n = normal_of_triangle(cover.green_circle->vs[i],
                                  cover.green_circle->vs[nextI],
                                  cover_top.green_circle->vs[i]);

    glNormal3d(n.x, n.y, n.z);

    cover.green_circle->vs[i].tex(scale, texW, texH);
    cover.green_circle->vs[i].draw();
    cover.green_circle->vs[nextI].tex(scale, texW, texH);
    cover.green_circle->vs[nextI].draw();
    cover_top.green_circle->vs[nextI].tex(scale, texW, texH);
    cover_top.green_circle->vs[nextI].draw();
    cover_top.green_circle->vs[i].tex(scale, texW, texH);
    cover_top.green_circle->vs[i].draw();
    glEnd();
  }

  // blue circle wall //! ==== ???? ??? ??
  for (int i = 0; i < cover.blue_circle->vs.size() - 1; i++) {
    int len = cover.blue_circle->vs.size();
    int nextI = (i + 1) % len;
    Vertex n = normal_of_triangle(cover.blue_circle->vs[i],
                                  cover.blue_circle->vs[nextI],
                                  cover_top.blue_circle->vs[i]);

    glNormal3d(n.x, n.y, n.z);

    glBegin(GL_QUADS);
    cover.blue_circle->vs[i].tex(scale, texW, texH);
    cover.blue_circle->vs[i].draw();
    cover.blue_circle->vs[nextI].tex(scale, texW, texH);
    cover.blue_circle->vs[nextI].draw();
    cover_top.blue_circle->vs[nextI].tex(scale, texW, texH);
    cover_top.blue_circle->vs[nextI].draw();
    cover_top.blue_circle->vs[i].tex(scale, texW, texH);
    cover_top.blue_circle->vs[i].draw();
    glEnd();
  }

  // !=========================================
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