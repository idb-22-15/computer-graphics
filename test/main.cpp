#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <iostream>

// Cube vertices
GLfloat vertices[] = {
    -0.5f, -0.5f, -0.5f,  // 0
     0.5f, -0.5f, -0.5f,  // 1
     0.5f,  0.5f, -0.5f,  // 2
    -0.5f,  0.5f, -0.5f,  // 3
    -0.5f, -0.5f,  0.5f,  // 4
     0.5f, -0.5f,  0.5f,  // 5
     0.5f,  0.5f,  0.5f,  // 6
    -0.5f,  0.5f,  0.5f   // 7
};

// Cube indices
GLuint indices[] = {
    0, 1, 2,  // Front face
    2, 3, 0,
    1, 5, 6,  // Right face
    6, 2, 1,
    4, 5, 6,  // Back face
    6, 7, 4,
    0, 4, 7,  // Left face
    7, 3, 0,
    3, 2, 6,  // Top face
    6, 7, 3,
    0, 1, 5,  // Bottom face
    5, 4, 0
};

// Mouse input variables
bool mouseDown = false;
double lastX = 0.0, lastY = 0.0;
double yaw = -90.0, pitch = 0.0;
const double sensitivity = 0.1;

void drawCube() {
    // Draw cube
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

void processMouseInput(GLFWwindow* window, double xpos, double ypos) {
    if (mouseDown) {
        double xOffset = xpos - lastX;
        double yOffset = lastY - ypos; // Reversed since y-coordinates go from bottom to top

        xOffset *= sensitivity;
        yOffset *= sensitivity;

        yaw += xOffset;
        pitch += yOffset;

        // Clamp pitch to avoid flipping
        if (pitch > 89.0) pitch = 89.0;
        if (pitch < -89.0) pitch = -89.0;

        lastX = xpos;
        lastY = ypos;
    }
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            mouseDown = true;
            glfwGetCursorPos(window, &lastX, &lastY);
        } else if (action == GLFW_RELEASE) {
            mouseDown = false;
        }
    }
}

int main() {
    GLFWwindow* window;

    // Initialize the library
    if (!glfwInit()) {
        return -1;
    }

    // Create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(800, 600, "term4-lab1", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);

    // Set up OpenGL viewport and projection matrix
    glViewport(0, 0, 800, 600);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (GLfloat)800 / (GLfloat)600, 0.1f, 100.0f);

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);

    // Set up mouse input callbacks
    glfwSetCursorPosCallback(window, processMouseInput);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);

    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window)) {
        // Clear the color and depth buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Reset the modelview matrix
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        // Move the cube back a bit so we can see it
        glTranslatef(0.0f, 0.0f, -5.0f);

        // Rotate the cube based on mouse input
        glRotatef(yaw, 0.0f, 1.0f, 0.0f);
        glRotatef(pitch, 1.0f, 0.0f, 0.0f);

        // Draw the cube
        drawCube();

        // Swap front and back buffers
        glfwSwapBuffers(window);

        // Poll for and process events
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}