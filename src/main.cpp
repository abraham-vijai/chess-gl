#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Texture.h"
#include "ShapeManager.h"

// -----------------------------------------------
// FUNCTION PROTOTYPES 
// -----------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
void renderPiece(Shader &shader, ShapeManager &quad, int quadIndex, Texture &texture, const glm::vec3 &position, float scale = 0.125f);
void setupPieces(Shader &pieceShader, ShapeManager &quad, int quadIndex, Texture textures[], bool isWhite);

// -----------------------------------------------
// GLOBAL VARIABLES 
// -----------------------------------------------
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;

int main()
{
    // -----------------------------------------------
    // INITIALIZE GLFW
    // -----------------------------------------------
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // Create window
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
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
    // Create quad for texture
    ShapeManager quad;
    int quadIndex = quad.createShape(quadVertices, sizeof(quadVertices), GL_STATIC_DRAW, quadIndices, sizeof(quadIndices));
    quad.addAttribute(quadIndex, 0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
    quad.addAttribute(quadIndex, 1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));

    // -----------------------------------------------
    // SETUP TEXTURES 
    // -----------------------------------------------
    // Enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // Load textures for pieces
    Texture textures[] = {
        Texture("../resources/w_pawn.png", GL_TEXTURE_2D, GL_RGBA, GL_REPEAT, GL_REPEAT),   // 0
        Texture("../resources/b_pawn.png", GL_TEXTURE_2D, GL_RGBA, GL_REPEAT, GL_REPEAT),   // 1
        Texture("../resources/w_rook.png", GL_TEXTURE_2D, GL_RGBA, GL_REPEAT, GL_REPEAT),   // 2
        Texture("../resources/b_rook.png", GL_TEXTURE_2D, GL_RGBA, GL_REPEAT, GL_REPEAT),   // 3
        Texture("../resources/w_knight.png", GL_TEXTURE_2D, GL_RGBA, GL_REPEAT, GL_REPEAT), // 4
        Texture("../resources/b_knight.png", GL_TEXTURE_2D, GL_RGBA, GL_REPEAT, GL_REPEAT), // 5
        Texture("../resources/w_bishop.png", GL_TEXTURE_2D, GL_RGBA, GL_REPEAT, GL_REPEAT), // 6
        Texture("../resources/b_bishop.png", GL_TEXTURE_2D, GL_RGBA, GL_REPEAT, GL_REPEAT), // 7
        Texture("../resources/w_queen.png", GL_TEXTURE_2D, GL_RGBA, GL_REPEAT, GL_REPEAT),  // 8
        Texture("../resources/b_queen.png", GL_TEXTURE_2D, GL_RGBA, GL_REPEAT, GL_REPEAT),  // 9
        Texture("../resources/w_king.png", GL_TEXTURE_2D, GL_RGBA, GL_REPEAT, GL_REPEAT),   // 10
        Texture("../resources/b_king.png", GL_TEXTURE_2D, GL_RGBA, GL_REPEAT, GL_REPEAT)    // 11
    };

    // Setup shader uniform
    pieceShader.use();
    pieceShader.setInt("pieceTexture", 0);

    // -----------------------------------------------
    // MAIN LOOP 
    // -----------------------------------------------
    while (!glfwWindowShouldClose(window))
    {
        // Input
        processInput(window);

        // -----------------------------------------------
        // RENDER 
        // -----------------------------------------------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        // Render rectangle
        ourShader.use();
        rectangle.renderShape(rectangleIndex, 6, 6, GL_TRIANGLES);
        // Render pieces
        setupPieces(pieceShader, quad, quadIndex, textures, true);
        setupPieces(pieceShader, quad, quadIndex, textures, false);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void renderPiece(Shader &shader, ShapeManager &quad, int quadIndex, Texture &texture, const glm::vec3 &position, float scale)
{
    shader.use();
    texture.bind(0, GL_TEXTURE_2D);
    glm::mat4 transform = glm::mat4(1.0f);
    transform = glm::scale(transform, glm::vec3(scale, scale, 1.0f));
    transform = glm::translate(transform, position);
    shader.setMat4("transform", transform);
    quad.renderShape(quadIndex, 6, 6, GL_TRIANGLES);
}

void setupPieces(Shader &pieceShader, ShapeManager &quad, int quadIndex, Texture textures[], bool isWhite)
{
    float multiplier = 0.0f;
    int j = 0;
    
    // White
    if (isWhite)
    {
        multiplier = 1.0f;
    }
    // Black
    else
    {
        multiplier = -1.0f;
        j = 1;
    }

    for (int i = -7; i < 8; i += 2)
    {
        renderPiece(pieceShader, quad, quadIndex, textures[j], glm::vec3(i, -5.0f * multiplier, 0.0f));
    }

    // White Rooks
    renderPiece(pieceShader, quad, quadIndex, textures[j + 2], glm::vec3(-7.0f, -7.0f * multiplier, 0.0f));
    renderPiece(pieceShader, quad, quadIndex, textures[j + 2], glm::vec3(7.0f, -7.0f * multiplier, 0.0f));

    // White Knights
    renderPiece(pieceShader, quad, quadIndex, textures[j + 4], glm::vec3(-5.0f, -7.0f * multiplier, 0.0f));
    renderPiece(pieceShader, quad, quadIndex, textures[j + 4], glm::vec3(5.0f, -7.0f * multiplier, 0.0f));

    // White Bishops
    renderPiece(pieceShader, quad, quadIndex, textures[j + 6], glm::vec3(-3.0f, -7.0f * multiplier, 0.0f));
    renderPiece(pieceShader, quad, quadIndex, textures[j + 6], glm::vec3(3.0f, -7.0f * multiplier, 0.0f));

    // White King and Queen
    renderPiece(pieceShader, quad, quadIndex, textures[j + 8], glm::vec3(-1.0f, -7.0f * multiplier, 0.0f));
    renderPiece(pieceShader, quad, quadIndex, textures[j + 10], glm::vec3(1.0f, -7.0f * multiplier, 0.0f));
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}