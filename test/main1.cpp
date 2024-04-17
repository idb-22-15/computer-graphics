#include <GL/freeglut.h>
#include <iostream>

#include "render.cpp"

int lastMouseX, lastMouseY;
int rotationAngleX = 0;
int rotationAngleY = 0;
float scale = 0.1;
float cameraX = 0.0, cameraY = 0.0, cameraZ = 5.0;

void init() {
  glClearColor(1.0, 1.0, 1.0, 1.0); // Set background color to white
}

void drawAxes() {
  glBegin(GL_LINES);

  // X-axis (red)
  glColor3f(1.0, 0.0, 0.0); // Red
  glVertex3f(0.0, 0.0, 0.0);
  glVertex3f(10.0, 0.0, 0.0);

  // Y-axis (green)
  glColor3f(0.0, 1.0, 0.0); // Green
  glVertex3f(0.0, 0.0, 0.0);
  glVertex3f(0.0, 10.0, 0.0);

  // Z-axis (blue)
  glColor3f(0.0, 0.0, 1.0); // Blue
  glVertex3f(0.0, 0.0, 0.0);
  glVertex3f(0.0, 0.0, 10.0);

  glEnd();
}

void display() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();

  gluLookAt(cameraX, cameraY, cameraZ, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

  // Apply rotation
  glRotated(rotationAngleX, 1, 0, 0);
  glRotated(rotationAngleY, 0, 1, 0);

  // // Apply scaling
  glScalef(scale, scale, scale);

  drawAxes();

  render();

  glutSwapBuffers();
}

void mouse(int button, int state, int x, int y) {
  if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
    lastMouseX = x;
    lastMouseY = y;
  }
}

void motion(int x, int y) {
  rotationAngleY += (x - lastMouseX);
  rotationAngleX += (y - lastMouseY);
  lastMouseX = x;
  lastMouseY = y;
  glutPostRedisplay();
}

void mouseWheel(int wheel, int direction, int x, int y) {
  if (direction == 1) {
    if (scale + 0.01 < 0.1)
      scale += 0.01;
  } else {
    if (scale - 0.01 > 0)
      scale -= 0.01;
  }
  std::cout << scale << std::endl;
  glutPostRedisplay();
}

void specialKeys(int key, int x, int y) {
  std::cout << "key: " << key << std::endl;
  switch (key) {
  case GLUT_KEY_UP:
    cameraY += 0.1;
    break;
  case GLUT_KEY_DOWN:
    cameraY -= 0.1;
    break;
  case GLUT_KEY_LEFT:
    cameraX -= 0.1;
    break;
  case GLUT_KEY_RIGHT:
    cameraX += 0.1;
    break;
  }
  glutPostRedisplay();
}

int main(int argc, char **argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(800, 600);
  glutCreateWindow("3D Scene with XYZ Axes");

  init();

  glutDisplayFunc(display);

  glutSpecialFunc(specialKeys);
  glutMouseFunc(mouse);
  glutMotionFunc(motion);
  glutMouseWheelFunc(mouseWheel);

  glEnable(GL_DEPTH_TEST);

  glutMainLoop();

  return 0;
}