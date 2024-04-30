#include "Render.h"

#include <iostream>
#include <sstream>

#include <GL\GL.h>
#include <GL\GLU.h>
#include <windows.h>

#include "MyOGL.h"

#include "Camera.h"
#include "Light.h"
#include "Primitives.h"

#include "GUItextRectangle.h"

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
    GLfloat amb[] = {1.2, 0.2, 1.2, 0};
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
  int texW, texH;
  OpenGL::LoadBMP("texture.bmp", &texW, &texH, &texarray);
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

void Render(OpenGL *ogl) {

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
  //===================================
  // Прогать тут

  // Начало рисования квадратика станкина
  // double A[2] = { -4, -4 };
  // double B[2] = { 4, -4 };
  // double C[2] = { 4, 4 };
  // double D[2] = { -4, 4 };

  // glBindTexture(GL_TEXTURE_2D, texId);

  // glColor3d(0.6, 0.6, 0.6);
  // glBegin(GL_QUADS);

  // glNormal3d(0, 0, 1);
  // glTexCoord2d(0, 0);
  // glVertex2dv(A);
  // glTexCoord2d(1, 0);
  // glVertex2dv(B);
  // glTexCoord2d(1, 1);
  // glVertex2dv(C);
  // glTexCoord2d(0, 1);
  // glVertex2dv(D);

  // glEnd();

  double Z = 3;
  double A[] = {0, 0, 0};
  double B[] = {0, 9, 0};
  double C[] = {2, 2, 0};

  double D[] = {7, 2, 0};
  double E[] = {9, -3, 0};
  double F[] = {2, -1, 0};

  double G[] = {-1, -6, 0};
  double H[] = {-5, -2, 0};

  double A1[] = {A[0], A[1], Z};
  double B1[] = {B[0], B[1], Z};
  double C1[] = {C[0], C[1], Z};

  double D1[] = {D[0], D[1], Z};
  double E1[] = {E[0], E[1], Z};
  double F1[] = {F[0], F[1], Z};

  double G1[] = {G[0], G[1], Z};
  double H1[] = {H[0], H[1], Z};

  glBindTexture(GL_TEXTURE_2D, texId);
  glBegin(GL_POLYGON);

  glNormal3d(0, 0, -1);
  glTexCoord2d((34.0 - A[0] - 6) / 34.0, (A[1] + 24) / 34.0);
  glVertex2dv(A);
  glTexCoord2d((34.0 - B[0] - 6) / 34.0, (B[1] + 24) / 34.0);
  glVertex2dv(B);
  glTexCoord2d((34.0 - C[0] - 6) / 34.0, (C[1] + 24) / 34.0);
  glVertex2dv(C);
  glTexCoord2d((34.0 - E[0] - 6) / 34.0, (E[1] + 24) / 34.0);
  glVertex2dv(E);
  glTexCoord2d((34.0 - F[0] - 6) / 34.0, (F[1] + 24) / 34.0);
  glVertex2dv(F);

  glColor3d(1, 0, 0);
  glVertex3dv(A);
  glVertex3dv(B);
  glVertex3dv(C);
  glVertex3dv(E);
  glVertex3dv(F);
  glEnd();

  glBindTexture(GL_TEXTURE_2D, texId);
  glBegin(GL_POLYGON);

  glNormal3d(0, 0, 1);
  glTexCoord3d((A[0] + 9) / 34.0, (A[1] + 24) / 34.0, Z);
  glVertex3dv(A1);
  glTexCoord3d((B[0] + 9) / 34.0, (B[1] + 24) / 34.0, Z);
  glVertex3dv(B1);
  glTexCoord3d((C[0] + 9) / 34.0, (C[1] + 24) / 34.0, Z);
  glVertex3dv(C1);
  glTexCoord3d((E[0] + 9) / 34.0, (E[1] + 24) / 34.0, Z);
  glVertex3dv(E1);
  glTexCoord3d((F[0] + 9) / 34.0, (F[1] + 24) / 34.0, Z);
  glVertex3dv(F1);
  glColor3d(0.7, 0.2, 1);
  glVertex3dv(A1);
  glVertex3dv(B1);
  glVertex3dv(C1);
  glVertex3dv(E1);
  glVertex3dv(F1);
  glEnd();

  glBindTexture(GL_TEXTURE_2D, texId);
  glBegin(GL_QUADS);

  glNormal3d(-1, 0, 0);
  glTexCoord3d(1 / 34.0, 14 / 34.0, 0);
  glVertex3dv(B);
  glTexCoord3d(1 / 34.0, 17 / 34.0, Z);
  glVertex3dv(B1);
  glTexCoord3d(10 / 34.0, 17 / 34.0, Z);
  glVertex3dv(A1);
  glTexCoord3d(10 / 34.0, 14 / 34.0, 0);
  glVertex3dv(A);

  glVertex3dv(A);
  glVertex3dv(A1);
  glVertex3dv(B1);
  glVertex3dv(B);
  glEnd();

  glBindTexture(GL_TEXTURE_2D, texId);
  glBegin(GL_QUADS);

  glNormal3d(7, 2, 0);
  glTexCoord3d(11 / 34.0, 14 / 34.0, 0);
  glVertex3dv(C);
  glTexCoord3d(11 / 34.0, 17 / 34.0, Z);
  glVertex3dv(C1);
  glTexCoord3d(18.28 / 34.0, 17 / 34.0, Z);
  glVertex3dv(B1);
  glTexCoord3d(18.28 / 34.0, 14 / 34.0, 0);
  glVertex3dv(B);

  glVertex3dv(B);
  glVertex3dv(B1);
  glVertex3dv(C1);
  glVertex3dv(C);

  glBindTexture(GL_TEXTURE_2D, texId);
  glBegin(GL_QUADS);

  glNormal3d(0, 1, 0);
  glTexCoord3d(20 / 34.0, 14 / 34.0, 0);
  glVertex3dv(D);
  glTexCoord3d(20 / 34.0, 17 / 34.0, Z);
  glVertex3dv(D1);
  glTexCoord3d(25 / 34.0, 17 / 34.0, Z);
  glVertex3dv(C1);
  glTexCoord3d(25 / 34.0, 14 / 34.0, 0);
  glVertex3dv(C);

  glVertex3dv(C);
  glVertex3dv(C1);
  glVertex3dv(D1);
  glVertex3dv(D);

  glBindTexture(GL_TEXTURE_2D, texId);
  glBegin(GL_QUADS);

  glNormal3d(-2, -7, 0);
  glTexCoord3d(11 / 34.0, 10 / 34.0, 0);
  glVertex3dv(F);
  glTexCoord3d(11 / 34.0, 13 / 34.0, Z);
  glVertex3dv(F1);
  glTexCoord3d(18.28 / 34.0, 13 / 34.0, Z);
  glVertex3dv(E1);
  glTexCoord3d(18.28 / 34.0, 10 / 34.0, 0);
  glVertex3dv(E);

  glVertex3dv(E);
  glVertex3dv(E1);
  glVertex3dv(F1);
  glVertex3dv(F);
  glEnd();

  glBegin(GL_QUADS);
  glNormal3d(5, -3, 0);

  glBindTexture(GL_TEXTURE_2D, texId);
  glTexCoord3d(20 / 34.0, 10 / 34.0, 0);
  glVertex3dv(G);
  glTexCoord3d(20 / 34.0, 13 / 34.0, Z);
  glVertex3dv(G1);
  glTexCoord3d(25.83 / 34.0, 13 / 34.0, Z);
  glVertex3dv(F1);
  glTexCoord3d(25.83 / 34.0, 10 / 34.0, 0);
  glVertex3dv(F);

  glVertex3dv(F);
  glVertex3dv(F1);
  glVertex3dv(G1);
  glVertex3dv(G);
  glEnd();

  glBindTexture(GL_TEXTURE_2D, texId);
  glBegin(GL_QUADS);
  glNormal3d(-2, 5, 0);

  glTexCoord3d(11 / 34.0, 6 / 34.0, 0);
  glVertex3dv(A);
  glTexCoord3d(11 / 34.0, 9 / 34.0, Z);
  glVertex3dv(A1);
  glTexCoord3d(16.4 / 34.0, 9 / 34.0, Z);
  glVertex3dv(H1);
  glTexCoord3d(16.4 / 34.0, 6 / 34.0, 0);
  glVertex3dv(H);

  glVertex3dv(H);
  glVertex3dv(H1);
  glVertex3dv(A1);
  glVertex3dv(A);
  glEnd();

  double L = sqrt(pow((D[0] - E[0]), 2) + pow((D[1] - E[1]), 2));

  double R = 5;
  double alpha = (asin(L / (2 * R)) * 2) * 180.0 / PI;

  double x0 = 0.5 * (D[0] + E[0]) +
              0.5 * sqrt(4 * pow(R, 2) / pow(L, 2) - 1) * (E[1] - D[1]);
  double y0 = 0.5 * (D[1] + E[1]) +
              0.5 * sqrt(4 * pow(R, 2) / pow(L, 2) - 1) * (D[0] - E[0]);

  double beta = atan((E[1] - y0) / (E[0] - x0)) * 180.0 / PI;

  double angle = 0.1;

  for (double i = beta; i < alpha + beta; i = i + angle) {

    double X[] = {x0 + R * cos(i * PI / 180.0), y0 + R * sin(i * PI / 180.0),
                  0};
    double Y[] = {x0 + R * cos((i + angle) * PI / 180.0),
                  y0 + R * sin((i + angle) * PI / 180.0), 0};

    double X1[] = {X[0], X[1], Z};
    double Y1[] = {Y[0], Y[1], Z};

    glBindTexture(GL_TEXTURE_2D, texId);
    glBegin(GL_TRIANGLES);
    glNormal3d(0, 0, -1);

    glTexCoord2d((34.0 - C[0] - 6) / 34.0, (C[1] + 24) / 34.0);
    glVertex2dv(C);
    glTexCoord2d((34.0 - X[0] - 6) / 34.0, (X[1] + 24) / 34.0);
    glVertex2dv(X);
    glTexCoord2d((34.0 - Y[0] - 6) / 34.0, (Y[1] + 24) / 34.0);
    glVertex2dv(Y);

    glColor3d(0.7, 0.2, 0);
    glVertex3dv(C);
    glVertex3dv(X);
    glVertex3dv(Y);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, texId);
    glBegin(GL_TRIANGLES);
    glNormal3d(0, 0, 1);
    glTexCoord3d((C[0] + 9) / 34.0, (C[1] + 24) / 34.0, Z);
    glVertex3dv(C1);
    glTexCoord3d((X[0] + 9) / 34.0, (X[1] + 24) / 34.0, Z);
    glVertex3dv(X1);
    glTexCoord3d((Y[0] + 9) / 34.0, (Y[1] + 24) / 34.0, Z);
    glVertex3dv(Y1);

    glColor3d(0.7, 0.2, 0);
    glVertex3dv(C1);
    glVertex3dv(X1);
    glVertex3dv(Y1);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, texId);
    glBegin(GL_QUADS);
    glNormal3d((Y[1] - X[1]), -(Y[0] - X[0]), 0);

    glTexCoord3d((1 + 5.7 * (i + angle - beta) / alpha) / 34.0, 10 / 34.0, 0);
    glVertex3dv(Y);
    glTexCoord3d((1 + 5.7 * (i + angle - beta) / alpha) / 34.0, 13 / 34.0, Z);
    glVertex3dv(Y1);
    glTexCoord3d((1 + 5.7 * (i - beta) / alpha) / 34.0, 13 / 34.0, Z);
    glVertex3dv(X1);
    glTexCoord3d((1 + 5.7 * (i - beta) / alpha) / 34.0, 10 / 34.0, 0);
    glVertex3dv(X);

    glColor3d(0.3, 0.2, 0);
    glVertex3dv(X);
    glVertex3dv(X1);
    glVertex3dv(Y1);
    glVertex3dv(Y);
    glEnd();
  }

  double L1 = sqrt(pow((H[0] - G[0]), 2) + pow((H[1] - G[1]), 2));

  double R1 = 5;
  double alpha1 = (asin(L1 / (2 * R1)) * 2) * 180.0 / PI;

  double x01 = 0.5 * (H[0] + G[0]) +
               0.5 * sqrt(4 * pow(R1, 2) / pow(L1, 2) - 1) * (G[1] - H[1]);
  double y01 = 0.5 * (H[1] + G[1]) +
               0.5 * sqrt(4 * pow(R1, 2) / pow(L1, 2) - 1) * (H[0] - G[0]);

  double beta1 = atan((G[1] - y01) / (G[0] - x01)) * 180.0 / PI;

  double angle1 = 1;
  bool flag = 1;
  for (double i = beta1; i < alpha1 + beta1; i = i + angle1) {

    double X[] = {x01 + R1 * cos(i * PI / 180.0),
                  y01 + R1 * sin(i * PI / 180.0), 0};
    double Y[] = {x01 + R1 * cos((i + angle1) * PI / 180.0),
                  y01 + R1 * sin((i + angle1) * PI / 180.0), 0};
    double X1[] = {X[0], X[1], Z};
    double Y1[] = {Y[0], Y[1], Z};
    if (i < alpha1 / 2 + beta1) {

      glBindTexture(GL_TEXTURE_2D, texId);
      glBegin(GL_TRIANGLES);
      glNormal3d(0, 0, -1);
      glTexCoord3d((34.0 - F[0] - 6) / 34.0, (F[1] + 24) / 34.0, 0);
      glVertex3dv(F);
      glTexCoord3d((34.0 - X[0] - 6) / 34.0, (X[1] + 24) / 34.0, 0);
      glVertex3dv(X);
      glTexCoord3d((34.0 - Y[0] - 6) / 34.0, (Y[1] + 24) / 34.0, 0);
      glVertex3dv(Y);

      glColor3d(0.7, 1, 0);
      glVertex3dv(F);
      glVertex3dv(X);
      glVertex3dv(Y);
      glEnd();

      glBindTexture(GL_TEXTURE_2D, texId);
      glBegin(GL_TRIANGLES);
      glNormal3d(0, 0, 1);
      glTexCoord3d((F[0] + 9) / 34.0, (F[1] + 24) / 34.0, Z);
      glVertex3dv(F1);
      glTexCoord3d((X[0] + 9) / 34.0, (X[1] + 24) / 34.0, Z);
      glVertex3dv(X1);
      glTexCoord3d((Y[0] + 9) / 34.0, (Y[1] + 24) / 34.0, Z);
      glVertex3dv(Y1);
      glColor3d(0.7, 1, 0);
      glVertex3dv(F1);
      glVertex3dv(X1);
      glVertex3dv(Y1);
      glEnd();

      glBindTexture(GL_TEXTURE_2D, texId);
      glBegin(GL_QUADS);
      glNormal3d(-(Y[1] - X[1]), (Y[0] - X[0]), 0);

      glTexCoord3d((7 - 6 * (i + angle1 - beta1) / alpha1) / 34.0, 6 / 34.0, 0);
      glVertex3dv(Y);
      glTexCoord3d((7 - 6 * (i + angle1 - beta1) / alpha1) / 34.0, 9 / 34.0, Z);
      glVertex3dv(Y1);
      glTexCoord3d((7 - 6 * (i - beta1) / alpha1) / 34.0, 9 / 34.0, Z);
      glVertex3dv(X1);
      glTexCoord3d((7 - 6 * (i - beta1) / alpha1) / 34.0, 6 / 34.0, 0);
      glVertex3dv(X);

      glColor3d(0.8, 0.0, 1);
      glVertex3dv(X);
      glVertex3dv(X1);
      glVertex3dv(Y1);
      glVertex3dv(Y);
      glEnd();
    } else {
      if (flag) {

        glBindTexture(GL_TEXTURE_2D, texId);
        glBegin(GL_TRIANGLES);
        glNormal3d(0, 0, -1);
        glTexCoord3d((34.0 - F[0] - 6) / 34.0, (F[1] + 24) / 34.0, 0);
        glVertex3dv(F);
        glTexCoord3d((34.0 - X[0] - 6) / 34.0, (X[1] + 24) / 34.0, 0);
        glVertex3dv(X);
        glTexCoord3d((34.0 - A[0] - 6) / 34.0, (A[1] + 24) / 34.0, 0);
        glVertex3dv(A);

        glColor3d(1, 1, 0);
        glVertex3dv(F);
        glVertex3dv(X);
        glVertex3dv(A);
        glEnd();

        glBindTexture(GL_TEXTURE_2D, texId);
        glBegin(GL_TRIANGLES);
        glNormal3d(0, 0, 1);
        glTexCoord3d((F[0] + 9) / 34.0, (F[1] + 24) / 34.0, Z);
        glVertex3dv(F1);
        glTexCoord3d((X[0] + 9) / 34.0, (X[1] + 24) / 34.0, Z);
        glVertex3dv(X1);
        glTexCoord3d((A[0] + 9) / 34.0, (A[1] + 24) / 34.0, Z);
        glVertex3dv(A1);

        glColor3d(1, 1, 0);
        glVertex3dv(F1);
        glVertex3dv(X1);
        glVertex3dv(A1);
        glEnd();
      }

      glBindTexture(GL_TEXTURE_2D, texId);
      glBegin(GL_TRIANGLES);
      glNormal3d(0, 0, -1);
      glTexCoord2d((34.0 - A[0] - 6) / 34.0, (A[1] + 24) / 34.0);
      glVertex2dv(A);
      glTexCoord2d((34.0 - X[0] - 6) / 34.0, (X[1] + 24) / 34.0);
      glVertex2dv(X);
      glTexCoord2d((34.0 - Y[0] - 6) / 34.0, (Y[1] + 24) / 34.0);
      glVertex2dv(Y);

      glColor3d(0.7, 1, 0);
      glVertex3dv(A);
      glVertex3dv(X);
      glVertex3dv(Y);
      glEnd();

      glBindTexture(GL_TEXTURE_2D, texId);
      glBegin(GL_TRIANGLES);
      glNormal3d(0, 0, 1);
      glTexCoord3d((A[0] + 9) / 34.0, (A[1] + 24) / 34.0, Z);
      glVertex3dv(A1);
      glTexCoord3d((X[0] + 9) / 34.0, (X[1] + 24) / 34.0, Z);
      glVertex3dv(X1);
      glTexCoord3d((Y[0] + 9) / 34.0, (Y[1] + 24) / 34.0, Z);
      glVertex3dv(Y1);
      glColor3d(0.7, 1, 0);
      glVertex3dv(A1);
      glVertex3dv(X1);
      glVertex3dv(Y1);
      glEnd();

      glBindTexture(GL_TEXTURE_2D, texId);
      glBegin(GL_QUADS);
      glNormal3d(-(Y[1] - X[1]), (Y[0] - X[0]), 0);

      glTexCoord3d((7 - 6 * (i + angle1 - beta1) / alpha1) / 34.0, 6 / 34.0, 0);
      glVertex3dv(Y);
      glTexCoord3d((7 - 6 * (i + angle1 - beta1) / alpha1) / 34.0, 9 / 34.0, Z);
      glVertex3dv(Y1);
      glTexCoord3d((7 - 6 * (i - beta1) / alpha1) / 34.0, 9 / 34.0, Z);
      glVertex3dv(X1);
      glTexCoord3d((7 - 6 * (i - beta1) / alpha1) / 34.0, 6 / 34.0, 0);
      glVertex3dv(X);

      glColor3d(0.8, 0.0, 1);
      glVertex3dv(X);
      glVertex3dv(X1);
      glVertex3dv(Y1);
      glVertex3dv(Y);
      glEnd();
      flag = 0;
    }
  }

  // конец рисования квадратика станкина

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