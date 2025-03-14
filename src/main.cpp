#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "Shader.h"
#include "Texture.h"
#include "ShapeManager.h"

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // build and compile our shader program
    // ------------------------------------
    Shader ourShader("../shaders/board_vs.vert", "../shaders/board_fs.frag");
    Shader pieceShader("../shaders/piece_vs.vert", "../shaders/piece_fs.frag");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float rectangleVertices[] = {
        // positions         // colors
        1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f,  // bottom right
        -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // bottom left
        -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,  // top left
        1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f    // top right
    };

    unsigned int rectangleIndices[] = {
        0, 1, 2, // first triangle
        0, 2, 3  // second triangle
    };

    float quadVertices[] = {
        // positions   // texture coords
        1.0f, 1.0f, 1.0f, 1.0f,   // top right
        1.0f, -1.0f, 1.0f, 0.0f,  // bottom right
        -1.0f, -1.0f, 0.0f, 0.0f, // bottom left
        -1.0f, 1.0f, 0.0f, 1.0f   // top left
    };
    unsigned int quadIndices[] = {
        0, 1, 2, // first triangle
        0, 2, 3  // second triangle
    };

    ShapeManager rectangle;
    int rectangleIndex = rectangle.createShape(rectangleVertices, sizeof(rectangleVertices), GL_STATIC_DRAW, rectangleIndices, sizeof(rectangleIndices));
    rectangle.addAttribute(rectangleIndex, 0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    rectangle.addAttribute(rectangleIndex, 1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));

    ShapeManager quad;
    int quadIndex = quad.createShape(quadVertices, sizeof(quadVertices), GL_STATIC_DRAW, quadIndices, sizeof(quadIndices));
    quad.addAttribute(quadIndex, 0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
    quad.addAttribute(quadIndex, 1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
    // Load texture
    // Enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Render rectangle
        ourShader.use();
        rectangle.renderShape(rectangleIndex, 6, 6, GL_TRIANGLES);

        // Render quad with texture
        pieceShader.use();
        quad.renderShape(quadIndex, 6, 6, GL_TRIANGLES);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}