#version 330
//Fragment Shader - Shader language 3.30, for OpenGL 3.3

//INPUTS:
in vec4 screenPos;
in vec4 worldPos;
in vec4 surfaceColors;
in vec2 textureCoords;

uniform sampler2D u_texture;	//= currently active texture


//OUTPUTS:
out vec4 o_pixelColor; //= final color (RGBA) to report to the framebuffer (to be blended)

//-----------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------
void main()
{
	vec4 diffuseTexel = texture2D( u_texture, textureCoords );

	o_pixelColor = surfaceColors * diffuseTexel;
}