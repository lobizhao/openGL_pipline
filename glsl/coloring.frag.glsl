#version 330 core

// The width and height of your canvas in pixels
uniform ivec2 u_ScreenDimensions;
// A constantly-increasing value updated in MyGL::tick()
uniform float u_Time;
// Only necessary for extra credit; the mouse's X and Y coords
uniform ivec2 u_MousePosition;

// The output color of this shader.
out vec4 out_Col;

void main() {
    // TODO: Do something cool using mouse position

    // Example code: Sets the final fragment color
    // to white with 100% opacity.
    out_Col = vec4(1,1,1,1);
}
