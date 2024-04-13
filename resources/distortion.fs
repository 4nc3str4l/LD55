#version 330

in vec2 fragTexCoord;
out vec4 finalColor;

uniform sampler2D texture0;
uniform float time;
uniform vec2 resolution;

void main()
{
    float wave = sin(fragTexCoord.y * 10.0 + time * 2.0) * 0.005;
    vec2 distortedCoord = vec2(fragTexCoord.x + wave, fragTexCoord.y);
    vec4 color = texture(texture0, distortedCoord);

    finalColor = color;
}

