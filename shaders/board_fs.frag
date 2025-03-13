#version 330 core
out vec4 FragColor;

// Input the fragment's screen-space coordinates
in vec2 FragCoord;

// Size of the chessboard squares
const float squareSize = 1.0 / 16.0; // For a NxN square, squareSize = 1.0 / (N * 2)

void main()
{
    // Calculate the position of the fragment in the chessboard grid
    vec2 gridPos = floor(FragCoord / squareSize);

    // Determine if the square should be black or white
    float isBlack = mod(gridPos.x + gridPos.y, 2.0);

    // Set the color based on the square
    if (isBlack < 1.0)
    {
        FragColor = vec4(0.6, 0.3, 0.1, 1.0); // Black square
    }
    else
    {
        FragColor = vec4(1.0, 0.8, 0.4, 1.0); // White square
    }
}