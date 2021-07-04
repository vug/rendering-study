#include <iostream>

#include "GLFW/glfw3.h"

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    std::cout << "key: " << key << std::endl;
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

int main() {
	std::cout << "Welcome to Hackamonth Study Editor!" << std::endl;

    GLFWwindow* window;
    if (!glfwInit()) return -1;
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window) { glfwTerminate(); return -1; }
    glfwSetKeyCallback(window, key_callback);

    glfwMakeContextCurrent(window);
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        //glClear(GL_COLOR_BUFFER_BIT);

        // glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}