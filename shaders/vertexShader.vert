#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 ourColor;
out vec2 FragCoord; // Pass fragment coordinates to the fragment shader

void main()
{
    gl_Position = vec4(aPos, 1.0);
    ourColor = aColor;

    // Calculate normalized screen-space coordinates
    FragCoord = aPos.xy * 0.5 + 0.5; // Normalize to [0, 1] range
}