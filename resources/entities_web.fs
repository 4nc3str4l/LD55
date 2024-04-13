precision mediump float;

varying vec2 fragTexCoord;
uniform sampler2D texture0;
uniform float time;
uniform vec2 resolution;
uniform vec4 tint;

void main() {
    float wave = sin(fragTexCoord.y * 5.0 + time * 2.0) * 0.05;
    vec2 distortedCoord = vec2(fragTexCoord.x + wave, fragTexCoord.y);
    vec4 color = texture2D(texture0, distortedCoord);

    gl_FragColor = color * tint;
}
