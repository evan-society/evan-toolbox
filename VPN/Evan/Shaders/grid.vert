uniform float coefArray1;   // Amount of warping for each warps
uniform float coefArray2;
uniform float coefArray3;
uniform float coefArray4;
uniform float coefArray5;
uniform int numElements;	  // number of warps
uniform mat4 P;
uniform mat4 P_Inverted;
attribute vec3 morph1;
attribute vec3 morph2;
attribute vec3 morph3;		 // one vector for each warps
attribute vec3 morph4;
attribute vec3 morph5;

void main()
{
	vec3 morphVector;
	vec4 ambient;
	vec4 vert;
	
	vert = P * gl_Vertex;
	vert.xyz += coefArray1 * morph1.xyz;
	if (numElements>=2)
		vert.xyz += coefArray2 * morph2.xyz;
	if (numElements>=3)
		vert.xyz += coefArray3 * morph3.xyz;
	if (numElements>=4)
		vert.xyz += coefArray4 * morph4.xyz;
	if (numElements>=5)
		vert.xyz += coefArray5 * morph5.xyz;
	
	gl_Position = gl_ModelViewProjectionMatrix * P_Inverted * vert;
	gl_FrontColor =  gl_FrontMaterial.diffuse;
} 
