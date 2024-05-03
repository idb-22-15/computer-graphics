#ifndef RENDER_H
#define RENDER_H

class OpenGL;

// ���������� ������� ����
void mouseEvent(OpenGL *ogl, int mX, int mY);

// ���������� ������� �������� ���� ����
void mouseWheelEvent(OpenGL *ogl, int delta);

// ���������� ������� ������� ������ (���� + �����)
void keyDownEvent(OpenGL *ogl, int key);

// ���������� ������� ���������� ������ (������!11 �����)
void keyUpEvent(OpenGL *ogl, int key);

// ����������� �����  ������ ��������
void initRender(OpenGL *ogl);

// ������
void Render(OpenGL *ogl);

#endif // ! RENDER_H