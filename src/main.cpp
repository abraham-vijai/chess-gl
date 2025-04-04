#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <unordered_map>

#include "Shader.h"
#include "Texture.h"
#include "ShapeManager.h"
#include "Board.h"
#include "Piece.h"

// -----------------------------------------------
// STRUCTS
// -----------------------------------------------
struct PieceStruct
{
    int pieceType = 0;
    int pieceColor = pieceType & (8 | 16);
    glm::vec2 piecePos = glm::vec2(0.0f, 0.0f);
    Texture *pieceTexture;

    // Constructor
    PieceStruct(int type, const glm::vec2 &pos, Texture *texture)
        : pieceType(type), piecePos(pos), pieceTexture(texture) {}
};

// -----------------------------------------------
// FUNCTION PROTOTYPES
// -----------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_button_callback(GLFWwindow *window, int button, int action, int mods);
void processInput(GLFWwindow *window);
void renderPieces(Shader &shader, ShapeManager &quad, int quadIndex);
void initializePieces(Texture textures[]);
void parseFenString(const std::string &fenString, Board &board, Texture textures[]);
Texture *getTexture(int pieceType, Texture textures[]);
void printPieceData();

// -----------------------------------------------
// GLOBAL VARIABLES
// -----------------------------------------------
#define SCR_WIDTH 800
#define SCR_HEIGHT 800
#define FEN_STRING "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR"
PieceStruct *selectedPiece = nullptr;
std::vector<PieceStruct> pieces;
glm::vec2 selectedCell = glm::vec2(1.0f, 1.0f);
std::vector<glm::vec2> validMoves; // Set of valid cells to highlight for a selected piece
bool isCellSelected = false;

void checkValidMoves()
{
    if (selectedPiece->pieceType == Piece::Pawn)
    {
        // Move only forward
    }
}
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
    glfwSetMouseButtonCallback(window, mouse_button_callback);

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
    Shader boardShader("../shaders/board_vs.vert", "../shaders/board_fs.frag");
    Shader pieceShader("../shaders/piece_vs.vert", "../shaders/piece_fs.frag");

    // -----------------------------------------------
    // SETUP VERTEX DATA
    // -----------------------------------------------
    // Board vertices
    float rectangleVertices[] = {
        // positions         // colors
        1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f,  // bottom right
        -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // bottom left
        -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,  // top left
        1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f    // top right
    };
    // Board indices
    unsigned int rectangleIndices[] = {
        0, 1, 2, // first triangle
        0, 2, 3  // second triangle
    };
    // Quad vertices
    float quadVertices[] = {
        // positions   // texture coords
        1.0f, 1.0f, 1.0f, 1.0f,   // top right
        1.0f, -1.0f, 1.0f, 0.0f,  // bottom right
        -1.0f, -1.0f, 0.0f, 0.0f, // bottom left
        -1.0f, 1.0f, 0.0f, 1.0f   // top left
    };
    // Quad indices
    unsigned int quadIndices[] = {
        0, 1, 2, // first triangle
        0, 2, 3  // second triangle
    };

    // -----------------------------------------------
    // CREATE SHAPES
    // -----------------------------------------------
    // Create board
    ShapeManager board;
    int boardIndex = board.createShape(rectangleVertices, sizeof(rectangleVertices), GL_STATIC_DRAW, rectangleIndices, sizeof(rectangleIndices));
    board.addAttribute(boardIndex, 0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    board.addAttribute(boardIndex, 1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
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

    // Initialize pieces
    initializePieces(textures);

    // -----------------------------------------------
    // MAIN LOOP
    // -----------------------------------------------
    while (!glfwWindowShouldClose(window))
    {
        // Input
        processInput(window);

        // Hightlight the cells in validMoves

        // -----------------------------------------------
        // RENDER
        // -----------------------------------------------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        // Render board
        boardShader.use();
        boardShader.setVec2("selectedCell", selectedCell);
        boardShader.setBool("isCellSelected", isCellSelected);
        board.renderShape(boardIndex, 6, 6, GL_TRIANGLES);
        // Render pieces
        renderPieces(pieceShader, quad, quadIndex);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void parseFenString(const std::string &fenString, Board &board, Texture textures[])
{
    // Map FEN characters to piece types
    static const std::unordered_map<char, int> fenToPiece = {
        {'r', Piece::Rook | Piece::Black}, {'n', Piece::Knight | Piece::Black}, {'b', Piece::Bishop | Piece::Black}, {'q', Piece::Queen | Piece::Black}, {'k', Piece::King | Piece::Black}, {'p', Piece::Pawn | Piece::Black}, {'R', Piece::Rook | Piece::White}, {'N', Piece::Knight | Piece::White}, {'B', Piece::Bishop | Piece::White}, {'Q', Piece::Queen | Piece::White}, {'K', Piece::King | Piece::White}, {'P', Piece::Pawn | Piece::White}};

    int squareIndex = 0; // Tracks the current square on the board

    for (char fenChar : fenString)
    {
        if (fenChar == '/')
        {
            // Skip slashes (they separate ranks in FEN)
            continue;
        }
        else if (std::isdigit(fenChar))
        {
            // Handle numbers (represent empty squares)
            int emptySquares = fenChar - '0';
            for (int i = 0; i < emptySquares; i++)
            {
                if (squareIndex >= 64)
                {
                    // Prevent out-of-bounds access
                    break;
                }
                board.Square[squareIndex++] = Piece::None;
            }
        }
        else
        {
            // Handle piece characters
            auto it = fenToPiece.find(fenChar);
            if (it != fenToPiece.end())
            {
                if (squareIndex >= 64)
                {
                    // Prevent out-of-bounds access
                    break;
                }
                board.Square[squareIndex++] = it->second;
            }
            else
            {
                std::cerr << "Invalid FEN character: " << fenChar << std::endl;
            }
        }
    }
}

Texture *getTexture(int pieceType, Texture textures[])
{
    // Check if the piece is black or white
    bool isBlack = (pieceType & Piece::Black) == Piece::Black;

    // Mask out the color bits to get the piece type
    int type = pieceType & 0b111; // 0b111 is the mask for the piece type

    // Determine the texture index based on the piece type and color
    if (isBlack)
    {
        switch (type)
        {
        case Piece::Rook:
            return &textures[3]; // Black rook
        case Piece::Knight:
            return &textures[5]; // Black knight
        case Piece::Bishop:
            return &textures[7]; // Black bishop
        case Piece::Queen:
            return &textures[9]; // Black queen
        case Piece::King:
            return &textures[11]; // Black king
        case Piece::Pawn:
            return &textures[1]; // Black pawn
        default:
            return nullptr; // Unknown piece type
        }
    }
    else
    {
        switch (type)
        {
        case Piece::Rook:
            return &textures[2]; // White rook
        case Piece::Knight:
            return &textures[4]; // White knight
        case Piece::Bishop:
            return &textures[6]; // White bishop
        case Piece::Queen:
            return &textures[8]; // White queen
        case Piece::King:
            return &textures[10]; // White king
        case Piece::Pawn:
            return &textures[0]; // White pawn
        default:
            return nullptr; // Unknown piece type
        }
    }

    // If the piece has no color (invalid), return nullptr
    return nullptr;
}

void initializePieces(Texture textures[])
{
    pieces.clear();

    Board chessBoard;
    parseFenString(FEN_STRING, chessBoard, textures);

    for (int i = 0; i < 64; i++)
    {
        int row = i / 8;
        int col = i % 8;

        // Calculate the position of the cell in OpenGL coordinates
        float x = -7.0f + 2.0f * (static_cast<float>(col));
        float y = 7.0f - 2.0f * (static_cast<float>(row));

        // Get piece type
        int pieceType = chessBoard.Square[i];
        // Get texture from piece type
        Texture *pieceTexture = getTexture(pieceType, textures);

        // Store the piece in the vector
        if (pieceTexture != nullptr)
            pieces.emplace_back(pieceType, glm::vec2(x, y), pieceTexture);
        else
            pieces.emplace_back(Piece::None, glm::vec2(x, y), nullptr);
    }
}

void renderPieces(Shader &shader, ShapeManager &quad, int quadIndex)
{
    for (const auto &piece : pieces)
    {
        // Skip rendering if the piece type is None (empty cell)
        if ((piece.pieceType & 7) == Piece::None)
        {
            continue;
        }

        shader.use();

        // Bind the texture if it exists
        if (piece.pieceTexture != nullptr)
        {
            (*piece.pieceTexture).bind(0, GL_TEXTURE_2D);
        }
        else
        {
            // No texture (this should not happen for valid pieces)
            shader.setInt("pieceTexture", 0);
        }

        // Apply transformations
        glm::mat4 transform = glm::mat4(1.0f);
        transform = glm::scale(transform, glm::vec3(0.125f, 0.125f, 1.0f));
        transform = glm::translate(transform, glm::vec3(piece.piecePos.x, piece.piecePos.y, 0.0f));
        shader.setMat4("transform", transform);

        // Render the quad
        quad.renderShape(quadIndex, 6, 6, GL_TRIANGLES);
    }
}

void printPieceData()
{
    std::cout << "\n============= Piece Data =============" << std::endl;

    if (selectedPiece->pieceColor == Piece::White)
    {
        std::cout << "Piece Color: White" << std::endl;
    }
    else if (selectedPiece->pieceColor == Piece::Black)
    {
        std::cout << "Piece Color: Black" << std::endl;
    }
    else
    {
        std::cout << "Piece Color: Unknown" << std::endl;
    }

    // Extract the piece type bits (last 3 bits)
    unsigned int type = selectedPiece->pieceType & 7; // Mask with 7 to get the last 3 bits

    std::cout << "Piece Type: ";
    switch (type)
    {
    case Piece::King:
        std::cout << "King" << std::endl;
        break;
    case Piece::Queen:
        std::cout << "Queen" << std::endl;
        break;
    case Piece::Bishop:
        std::cout << "Bishop" << std::endl;
        break;
    case Piece::Rook:
        std::cout << "Rook" << std::endl;
        break;
    case Piece::Pawn:
        std::cout << "Pawn" << std::endl;
        break;
    case Piece::Knight:
        std::cout << "Knight" << std::endl;
        break;
    case Piece::None:
        std::cout << "None" << std::endl;
        break;
    default:
        std::cout << "Unknown" << std::endl;
        break;
    }

    std::cout << "Piece Position: " << selectedPiece->piecePos.x << ", " << selectedPiece->piecePos.y << std::endl;
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

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        selectedPiece = nullptr;

        double xPos, yPos;
        int x, y;

        // Get the mouse position
        glfwGetCursorPos(window, &xPos, &yPos);

        // Get the x and y coordinates
        x = xPos / (SCR_WIDTH / 8);
        y = yPos / (SCR_HEIGHT / 8);

        // Get the index of the selected square
        int selectedIndex = y * 8 + x;
        // Set the selected piece
        selectedPiece = &pieces[selectedIndex];
        // If empty cells are selected
        if ((selectedPiece->pieceType & 7) == Piece::None)
        {
            selectedPiece = nullptr;
            isCellSelected = false;
        }
        else
        {
            // Print piece data
            printPieceData();

            // Update the selected cell coordinates
            selectedCell = glm::vec2(x, 7 - y);
            isCellSelected = true;

            // Check the valid moves for the selected piece
            if (selectedPiece != nullptr)
            {
                validMoves.clear();
                float multiplier = (selectedPiece->pieceColor == Piece::White) ? 1.0f : -1.0f;

                // If selected piece is pawn
                if ((selectedPiece->pieceType & 7) == Piece::Pawn)
                {
                    selectedCell.y += 1.0f * multiplier;
                    validMoves.emplace_back(selectedCell);
                }

                // If selected piece is rook
                else if ((selectedPiece->pieceType & 7) == Piece::Rook)
                {
                    for (int i = (int)selectedCell.y + 1; i < 7; i++)
                    {
                        selectedCell.y += 1.0f * multiplier;
                        validMoves.emplace_back(selectedCell);
                    }
                }
            }
        }
    }
}
