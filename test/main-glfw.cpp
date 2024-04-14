#include <GLFW/glfw3.h>
#include <iostream>

void drawAxes() {
    // X-axis (Red)
    glColor3f(1.0f, 0.0f, 0.0f); // Red color
    glBegin(GL_LINES);
    glVertex3f(-100.0f, 0.0f, 0.0f);
    glVertex3f(100.0f, 0.0f, 0.0f);
    glEnd();

    // Y-axis (Green)
    glColor3f(0.0f, 1.0f, 0.0f); // Green color
    glBegin(GL_LINES);
    glVertex3f(0.0f, -100.0f, 0.0f);
    glVertex3f(0.0f, 100.0f, 0.0f);
    glEnd();

    // Z-axis (Blue)
    glColor3f(0.0f, 0.0f, 1.0f); // Blue color
    glBegin(GL_LINES);
    glVertex3f(0.0f, 0.0f, -100.0f);
    glVertex3f(0.0f, 0.0f, 100.0f);
    glEnd();
}

void render(GLFWwindow* window) {
    // Clear the color and depth buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Draw the axes
    drawAxes();

    // Swap buffers
    glfwSwapBuffers(window);
}

int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    // Create a windowed mode window and its OpenGL context
    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL XYZ Axes", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);

    // Set the viewport
    glViewport(0, 0, 800, 600);

    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window)) {
        // Render
        render(window);

        // Poll for and process events
        glfwPollEvents();
    }

    // Terminate GLFW
    glfwTerminate();
    return 0;
}