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

uniform sampler2D Noise3D;

void main() {
    vec4 texcolor = texture2D(Noise3D, gl_TexCoord[0].xy);
    float light = length(blinn());
    texcolor[2] = light * pow(texcolor[2], 2) / 2;
    texcolor[1] *= light / 2;
    texcolor[1] = mix(texcolor[2], texcolor[1], pow(texcolor[1], 2));
    gl_FragColor = texcolor;
}