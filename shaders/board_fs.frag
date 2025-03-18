#version 330 core
out vec4 FragColor;

// Input the fragment's screen-space coordinates
in vec2 FragCoord;

// Size of the chessboard squares
const float squareSize = 1.0 / 8.0; // For a NxN square, squareSize = 1.0 / (N)

// Uniforms for the selected cell
uniform vec2 selectedCell; // (x, y) coordinates of the selected cell
uniform bool isCellSelected; // Whether a cell is selected

void main()
{
    // Calculate the position of the fragment in the chessboard grid
    vec2 gridPos = floor(FragCoord / squareSize);

    // Determine if the square should be black or white
    float isBlack = mod(gridPos.x + gridPos.y, 2.0);

    // Default colors for black and white squares
    vec4 blackColor = vec4(0.6, 0.3, 0.1, 1.0); // Black square
    vec4 whiteColor = vec4(1.0, 0.8, 0.4, 1.0); // White square

    // Highlight color
    vec4 highlightColor = vec4(0.2, 0.8, 0.2, 1.0); // Green highlight

    // Check if the current fragment is in the selected cell
    if (isCellSelected && gridPos == selectedCell)
    {
        FragColor = highlightColor; // Highlight the selected cell
    }
    else
    {
        // Set the color based on the square
        if (isBlack < 1.0)
        {
            FragColor = blackColor; // Black square
        }
        else
        {
            FragColor = whiteColor; // White square
        }
    }
}