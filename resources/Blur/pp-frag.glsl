#version 120

varying vec2 TexCoords;

uniform sampler2D screen;
uniform vec2 screenSize;
uniform bool fast;
uniform bool first;
uniform float radius;

void main() {
    float scaledRadius = radius * screenSize.y * 0.5;
    vec2 texOffset = 1.0 / screenSize; // gets size of single texel
    vec2 direction = first ? vec2(texOffset.x, 0.0) : vec2(0.0, texOffset.y); // blur direction

    vec3 result = texture2D(screen, TexCoords).rgb;
    float weightSum = 1.0;
    float weight = 1.0;

    if (fast) {
        float fastScale = radius * 10.0 / ((radius * 10.0 + 1.0) * (radius * 10.0 + 1.0) - 1.0);
        scaledRadius *= fastScale;

        for (int i = 1; i < 64; i++) {
           if (float(i) >= scaledRadius) break;

           weight -= 1.0 / scaledRadius;
           if (weight <= 0.0) break;

           vec2 offset = direction * float(i);
           result += texture2D(screen, TexCoords + offset).rgb * weight;
           result += texture2D(screen, TexCoords - offset).rgb * weight;
           weightSum += 2.0 * weight;
        }
    } else {
        float firstWeight = 0.84089642 / pow(scaledRadius, 0.96);
        result *= firstWeight;
        weightSum = firstWeight;

        for (int i = 1; i < 64; i++) {
           float dist = float(i);
           if (dist > scaledRadius) break;

           float weight = firstWeight * exp(-dist * dist / (2.0 * scaledRadius));
           vec2 offset = direction * dist;

           result += texture2D(screen, TexCoords + offset).rgb * weight;
           result += texture2D(screen, TexCoords - offset).rgb * weight;
           weightSum += 2.0 * weight;
        }
    }

    result /= weightSum;
    gl_FragColor = vec4(result, 1.0);
}