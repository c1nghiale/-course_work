varying vec3 Normal;
varying vec3 Position;

uniform vec3 lightPos;
uniform vec3 shipColor;

void main()
{
    vec3 lightDir = normalize(lightPos - Position);
    float diff = max(dot(Normal, lightDir), 0.0);
    vec3 ambient = shipColor * 0.2;
    vec3 diffuse = shipColor * diff * 0.8;
    gl_FragColor = vec4(ambient + diffuse, 1.0);
}