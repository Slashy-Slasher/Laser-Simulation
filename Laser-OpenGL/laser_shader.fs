#version 330

uniform vec2 laserPositions[100];
uniform int laserCount;

out vec4 fragColor;

void main()
{
    vec2 frag = gl_FragCoord.xy;
    float brightness = 0.0;

    for (int i = 0; i < laserCount; i++) {
        float dist = length(frag - laserPositions[i]);
        brightness += 10.0 / (dist + 10.0); // falloff
    }

    brightness = clamp(brightness, 0.0, 1.0);
    fragColor = vec4(vec3(brightness), 1.0);
}
