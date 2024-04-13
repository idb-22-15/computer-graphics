#pragma once

#include <vector>
#include <algorithm>
#include <iterator>
#include <cmath>
#include <numeric>
#include <numbers>

using std::numbers::pi;

struct Vertex {
	double x;
	double y;
	double z;
	Vertex() {};
	Vertex(double x, double y, double z) : x(x), y(y), z(z) {};

	void draw() {
		glColor3d(x / 10, y / 10, z / 10);
		glVertex3d(x, y, z);
	}
	void draw(double r, double g, double b) {
		glColor3d(r, g, b);
		glVertex3d(x, y, z);
	}
	Vertex operator + (const Vertex& other) {
		return Vertex(x + other.x, y + other.y, z + other.z);
	}
	Vertex operator - (const Vertex& other) {
		return Vertex(x - other.x, y - other.y, z - other.z);
	}
	Vertex operator * (double a) {
		return Vertex(x * a, y * a, z * a);
	}
	Vertex operator / (double a) {
		return Vertex(x / a, y / a, z / a);
	}
};

struct Line {
	Vertex start;
	Vertex end;
	Vertex mid;
	double length;
	Line(Vertex start, Vertex end): start(start), end(end) {
		mid = (start + end) / 2.;

		Vertex v = start - end;
		length = std::sqrt(std::pow(v.x, 2) + std::pow(v.y, 2) + std::pow(v.z, 2));

	}
};

Vertex fig_center(std::vector<Vertex> fig) {
	Vertex sum = std::accumulate(fig.begin(), fig.end(), Vertex(0, 0, 0), [](Vertex total, const Vertex& v) { return total + v; });
	return sum / fig.size();
}

//std::vector<Vertex> make_circle(Vertex center, double radius, double start_angle, double end_angle) {
//	std::vector<Vertex> circle;
//
//	int count = 100;
//	for (int i = 0; i < count + 1; i++) {
//		const double angle = start_angle + (end_angle - start_angle) / count * i;
//		Vertex v(center.x + cos(angle) * radius, center.y + sin(angle) * radius, center.z);
//		circle.push_back(v);
//	}
//	return circle;
//}

double to_rad(double deg) {
	return deg / 180 * pi;
}

double get_angle(Vertex start, Vertex end) {
	Vertex v = start - end;
	double angle_in_rad = std::atan2(v.y, v.x);
	return angle_in_rad;
}


//void drawHalfCircle(Vertex start, Vertex end) {
//	// ��������� ����� ����������
//	float centerX = (start.x + end.x) / 2.0f;
//	float centerY = (start.y + end.y) / 2.0f;
//	Vertex center = line_center(start, end);
//
//	// ��������� ������ (�������� ����� ��������)
//	float radius = sqrt(pow((end.x - start.x), 2) + pow((end.y - start.y), 2)) / 2.0f;
//
//	// ���������� ���� ������ � ����� ���� (�������� ����������)
//	float startAngle = get_angle(start, center); atan2(start.y - centerY, start.x - centerX);
//	float endAngle = get_angle(end, center); atan2(end.y - centerY, end.x - centerX);
//
//	// ������ ���� ���������� (�������� ����������)
//	glBegin(GL_LINE_STRIP);
//	for (float angle = startAngle; angle <= endAngle; angle += 0.01f) {
//		float x = center.x + radius * cos(angle);
//		float y = center.y + radius * sin(angle);
//		glVertex3f(x, y, 0);
//	}
//	glEnd();
//}

Vertex find_normal(Vertex start, Vertex end, double distance = 1) {
	Vertex dv = end - start;
	Line line = Line(start, end);
	double nx = -dv.y / line.length;  // ������������ �� 90 ��������
	double ny = dv.x / line.length;
	Vertex n(nx , ny , start.z);

	double normalX = line.mid.x + n.x * distance;
	double normalY = line.mid.y + n.y * distance;

	return Vertex(normalX, normalY, start.z);
}

const int circle_points = 100;

struct Circle {
	Vertex center;
	double radius;
	double start_angle;
	double end_angle;
	std::vector<Vertex> vs;
	Circle(Vertex center, double radius, double start_angle, double end_angle) :
		center(center), radius(radius), start_angle(start_angle), end_angle(end_angle)
	{
		for (int i = 0; i < circle_points + 1; i++) {
			const double angle = start_angle + (end_angle - start_angle) / circle_points * i;
			Vertex v(center.x + cos(angle) * radius, center.y + sin(angle) * radius, center.z);
			vs.push_back(v);
		}
	}

	void draw() {
		for (auto& v : vs) {
			v.draw();
		}
	}
};

struct Figure {
	std::vector<Vertex> vs;
	Circle* green_circle;
	Circle* blue_circle;
	
	Figure(double height) {
		vs = {
		{ 10, 1, height }, // green
		{ 6, 5, height },
		{ 8, 9, height },
		{ 2, 11, height },
		{ 5, 16, height }, //blue
		{ 13, 13, height }, //blue
		{ 10, 9, height },
		{ 15, 5, height } //green
		};
		size_t rows = vs.size();

		// green

		Line green_line = Line(vs[0], vs[7]);
		Vertex green_circle_center = green_line.mid;
		double green_circle_radius = Line(green_circle_center, vs[0]).length;
		double green_circle_start_angle = get_angle(vs[7], green_circle_center);
		double green_circle_end_angle = get_angle(vs[0], green_circle_center);
		green_circle = new Circle(green_circle_center, green_circle_radius, green_circle_start_angle, green_circle_end_angle);

		// blue
		Line blue_line = Line(vs[4], vs[5]);
		Vertex blue_mid = blue_line.mid;
		Vertex blue_normal = find_normal(vs[4], vs[5]);
		Vertex blue_circle_center = blue_normal;
		double blue_circle_radius = Line(blue_circle_center, vs[4]).length;
		double blue_circle_start_angle = get_angle(vs[4], blue_circle_center);
		double blue_circle_end_angle = get_angle(vs[5], blue_circle_center) + 2 * pi;
		blue_circle = new Circle(blue_circle_center, blue_circle_radius, blue_circle_start_angle, blue_circle_end_angle);
	}
	~Figure() {
		delete green_circle;
		delete blue_circle;
	}

	void draw() {
		glBegin(GL_TRIANGLE_FAN);
		green_circle->center.draw();
		green_circle->draw();
		glEnd();


		glBegin(GL_TRIANGLE_FAN);
		vs[3].draw();
		for (int i = 0; i <= circle_points / 2; i++) {
			blue_circle->vs[i].draw();
		}
		glEnd();

		glBegin(GL_TRIANGLE_FAN);
		vs[2].draw();
		for (int i = circle_points / 3; i <= circle_points / 3 * 2; i++) {
			blue_circle->vs[i].draw();
		}
		glEnd();

		glBegin(GL_TRIANGLE_FAN);
		vs[6].draw();
		for (int i = circle_points / 2; i <= circle_points; i++) {
			blue_circle->vs[i].draw();
		}
		glEnd();

		glBegin(GL_TRIANGLES);
		vs[3].draw();
		vs[2].draw();
		blue_circle->vs[circle_points / 2].draw();

		vs[2].draw();
		vs[6].draw();
		blue_circle->vs[circle_points / 2].draw();
		glEnd();

		// 

		glBegin(GL_POLYGON);
		vs[0].draw();
		vs[1].draw();
		vs[2].draw();
		vs[6].draw();
		vs[7].draw();
		glEnd();
	}
};


void render()
{    
	Figure fig(0);
	Figure fig_top(2);
	fig.draw();
	fig_top.draw();
	
	int len = fig.vs.size();
	
	glBegin(GL_QUADS);
		for (int i = 0; i < len; i++) {
			if (i == 4) continue;
			fig.vs[i].draw();
			fig.vs[(i + 1) % len].draw();
			fig_top.vs[(i + 1) % len].draw();
			fig_top.vs[i].draw();
		}
		for (int i = 0; i < fig.green_circle->vs.size(); i++) {
			fig.green_circle->vs[i].draw();
			fig.green_circle->vs[(i + 1) % len].draw();
			fig_top.green_circle->vs[(i + 1) % len].draw();
			fig_top.green_circle->vs[i].draw();
		}
		glEnd();
		glBegin(GL_QUADS);

		for (int i = 0; i < fig.blue_circle->vs.size() - 1  ; i++) {
			fig.blue_circle->vs[i].draw();
			fig.blue_circle->vs[i + 1].draw();
			fig_top.blue_circle->vs[i + 1].draw();
			fig_top.blue_circle->vs[i].draw();
		}
	glEnd();
}   

