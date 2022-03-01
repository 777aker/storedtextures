#version 120

varying vec3 View;
varying vec3 Light;
varying vec3 Normal;
varying vec4 Ambient;

varying vec3 ModelPos;

void main() {
	ModelPos = gl_Vertex.xyz;

	// lighting values for frag shader
	// vertex location in modelview coordinates
	vec3 P = vec3(gl_ModelViewMatrix * gl_Vertex);
	// light position
	Light = vec3(gl_LightSource[0].position) - P;
	// normal 
	Normal = gl_NormalMatrix * gl_Normal;
	// eye position
	View = -P;
	// ambient color 
	Ambient = gl_FrontMaterial.emission + gl_FrontLightProduct[0].ambient + gl_LightModel.ambient*gl_FrontMaterial.ambient;

	// texture coord 
	//gl_TexCoord[0] = gl_Vertex;
	gl_TexCoord[0] = gl_MultiTexCoord0;

	// set vertex position
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}