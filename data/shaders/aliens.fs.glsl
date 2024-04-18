#version 330 core
in vec2 TexCoord;

uniform vec4 ourColor;
uniform sampler2D ourTexture;

out vec4 fragColor;

void main()
{
   // fragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
   // fragColor = ourColor;
   fragColor = texture(ourTexture, TexCoord);
}