
using std::numbers::pi;

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
  double *raw() { return data; }
};

vec3 rgb_to_normal(vec3 rgb) {
  return vec3(rgb[0] / 255., rgb[1] / 255., rgb[2] / 255.);
}

void Render(double delta_time) {

  std::vector<vec3> vs = {
      {2, 3, 0},   // 0 green
      {1, 8, 0},   // 1 green
      {7, 10, 0},  // 2
      {7, 15, 0},  //
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
  glColor3dv(color_cover.raw());

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
  glColor3dv(color_cover.raw());

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
}
