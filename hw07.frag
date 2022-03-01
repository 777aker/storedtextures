#version 120

// model and light from vertex shader 
varying vec3 View;
varying vec3 Light;
varying vec3 Normal;
varying vec4 Ambient;

varying vec3 ModelPos;

uniform float time;

uniform sampler2D tex;

vec4 blinn() {
    //  N is the object normal
    vec3 N = normalize(Normal);
    //  L is the light vector
    vec3 L = normalize(Light);
    //  V is the view vector
    vec3 V = normalize(View);

    //  Emission and ambient color
    vec4 color = Ambient;

    //  Diffuse light is cosine of light and normal vectors
    float Id = dot(L,N);
    if (Id>0.0) {
        //  Add diffuse
        color += Id*gl_FrontLightProduct[0].diffuse;
        //  The half vectors
        vec3 H = normalize(V+L);
        //  Specular is cosine of reflected and view vectors
        float Is = dot(H,N);
        if (Is>0.0) color += pow(Is,gl_FrontMaterial.shininess)*gl_FrontLightProduct[0].specular;
    }

    //  Return sum of color components
    return color;
}

const float Scale = 1.2;
const vec4 Vein   = vec4(0.0,0.0,0.5,1.0);
const vec4 Marble = vec4(0.8,0.8,0.8,1.0);
uniform sampler2D Noise3D;

void main() {
    /*
    vec4 nv = texture3D(Noise3D,Scale*gl_TexCoord[0].xyz);
    float n = abs(nv[0] - 0.25)
            + abs(nv[1] - 0.125)
            + abs(nv[2] - 0.0625)
            + abs(nv[3] - 0.03125);
    n = 0.5*sin(12.0*n)+0.5;*/
    //gl_FragColor = blinn() * mix(Vein, Marble, n);
    gl_FragColor = texture2D(Noise3D, gl_TexCoord[0].xy);
}