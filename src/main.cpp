#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Texture.h"
#include "ShapeManager.h"
#include "Board.h"

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;

int main()
{
    // -----------------------------------------------
    // INITIALIZE GLFW
    // -----------------------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // Create window
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    // Callback functions
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // -----------------------------------------------
    // LOAD GLAD
    // -----------------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // -----------------------------------------------
    // LOAD SHADERS
    // -----------------------------------------------
    Shader ourShader("../shaders/board_vs.vert", "../shaders/board_fs.frag");
    Shader pieceShader("../shaders/piece_vs.vert", "../shaders/piece_fs.frag");

    // -----------------------------------------------
    // SETUP VERTEX DATA
    // -----------------------------------------------
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

    // -----------------------------------------------
    // CREATE SHAPES
    // -----------------------------------------------
    // Create rectangle
    ShapeManager rectangle;
    int rectangleIndex = rectangle.createShape(rectangleVertices, sizeof(rectangleVertices), GL_STATIC_DRAW, rectangleIndices, sizeof(rectangleIndices));
    rectangle.addAttribute(rectangleIndex, 0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    rectangle.addAttribute(rectangleIndex, 1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    // Create full quad for textures
    ShapeManager quad;
    int quadIndex = quad.createShape(quadVertices, sizeof(quadVertices), GL_STATIC_DRAW, quadIndices, sizeof(quadIndices));
    quad.addAttribute(quadIndex, 0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
    quad.addAttribute(quadIndex, 1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));

    // -----------------------------------------------
    // LOAD TEXTURES
    // -----------------------------------------------
    // Enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // Load textures for pieces
    Texture wPawnTexture("../resources/w_pawn.png", GL_TEXTURE_2D, GL_RGBA, GL_REPEAT, GL_REPEAT);
    Texture bPawnTexture("../resources/b_pawn.png", GL_TEXTURE_2D, GL_RGBA, GL_REPEAT, GL_REPEAT);
    Texture wRookTexture("../resources/w_rook.png", GL_TEXTURE_2D, GL_RGBA, GL_REPEAT, GL_REPEAT);
    Texture bRookTexture("../resources/b_rook.png", GL_TEXTURE_2D, GL_RGBA, GL_REPEAT, GL_REPEAT);
    Texture wKnightTexture("../resources/w_knight.png", GL_TEXTURE_2D, GL_RGBA, GL_REPEAT, GL_REPEAT);
    Texture bKnightTexture("../resources/b_knight.png", GL_TEXTURE_2D, GL_RGBA, GL_REPEAT, GL_REPEAT);
    Texture bBishopTexture("../resources/b_bishop.png", GL_TEXTURE_2D, GL_RGBA, GL_REPEAT, GL_REPEAT);
    Texture wQueenTexture("../resources/w_queen.png", GL_TEXTURE_2D, GL_RGBA, GL_REPEAT, GL_REPEAT);
    Texture bQueenTexture("../resources/b_queen.png", GL_TEXTURE_2D, GL_RGBA, GL_REPEAT, GL_REPEAT);
    Texture wKingTexture("../resources/w_king.png", GL_TEXTURE_2D, GL_RGBA, GL_REPEAT, GL_REPEAT);
    Texture bKingTexture("../resources/b_king.png", GL_TEXTURE_2D, GL_RGBA, GL_REPEAT, GL_REPEAT);
    // Setup shader uniform
    pieceShader.use();
    pieceShader.setInt("pieceTexture", 0);

    // for (int i = 0; i < SCR_WIDTH; ++i)
    // {
    //     for (int j = 0; j < SCR_HEIGHT; ++j)
    //     {
    //         float fWidth = static_cast<float>(i / SCR_WIDTH) * 2.0f - 1.0f;
    //         float fHeight = 1.0f - (static_cast<float>(j) / SCR_HEIGHT) * 2.0f;
    //         float quadVertices[] = {
    //             // positions   // texture coords
    //             1.0f  , 1.0f  , 1.0f , 1.0f ,   // top right
    //             1.0f  , -1.0f , 1.0f , 0.0f ,  // bottom right
    //             -1.0f , -1.0f , 0.0f , 0.0f , // bottom left
    //             -1.0f , 1.0f  , 0.0f , 1.0f    // top left
    //         };
    //     }
    // }

    // -----------------------------------------------
    // MAIN LOOP
    // -----------------------------------------------
    while (!glfwWindowShouldClose(window))
    {
        // input
        processInput(window);

        // -----------------------------------------------
        // RENDER
        // -----------------------------------------------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        // Render rectangle
        ourShader.use();
        rectangle.renderShape(rectangleIndex, 6, 6, GL_TRIANGLES);
        // Render quad with texture
        pieceShader.use();
        bPawnTexture.bind(0, GL_TEXTURE_2D);
        // Setting up black pawn positions
        for (int i = -7; i < 8; i++)
        {
            // glm::vec3(-7.0f, 5.0f, 0.0f) -> glm::vec3(7.0f, 5.0f, 0.0f) BLACK PAWN POSITION
            if (i % 2 != 0)
            {
                glm::mat4 transform = glm::mat4(1.0f);
                transform = glm::scale(transform, glm::vec3(0.125f, 0.125f, 1.0f));
                float pawnX = static_cast<float>(i);
                transform = glm::translate(transform, glm::vec3(pawnX, 5.0f, 0.0f)); // 1,3,5,7
                pieceShader.setMat4("transform", transform);
                quad.renderShape(quadIndex, 6, 6, GL_TRIANGLES);
            }
        }
        pieceShader.use();
        wPawnTexture.bind(0, GL_TEXTURE_2D);
        // Setting up black pawn positions
        for (int i = -7; i < 8; i++)
        {
            // glm::vec3(-7.0f, 5.0f, 0.0f) -> glm::vec3(7.0f, 5.0f, 0.0f) WHITE PAWN POSITION
            if (i % 2 != 0)
            {
                glm::mat4 transform = glm::mat4(1.0f);
                transform = glm::scale(transform, glm::vec3(0.125f, 0.125f, 1.0f));
                float pawnX = static_cast<float>(i);
                transform = glm::translate(transform, glm::vec3(pawnX, -5.0f, 0.0f)); // 1,3,5,7
                pieceShader.setMat4("transform", transform);
                quad.renderShape(quadIndex, 6, 6, GL_TRIANGLES);
            }
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

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