uniform float coefArray1;   // Amount of warping for each warps
uniform float coefArray2;
uniform float coefArray3;
uniform float coefArray4;
uniform float coefArray5;
uniform int numElements;	  // number of warps
attribute vec3 morph1;
attribute vec3 morph2;
attribute vec3 morph3;		 // one vector for each warps
attribute vec3 morph4;
attribute vec3 morph5;

void main()
{
	vec3 normalF, lightDir, morphVector;
	vec4 diffuseF, ambientF, globalAmbientF; // Front color
	vec4 diffuseB, ambientB, globalAmbientB; // Back color
	vec4 vert;
	float NdotF,NdotB;
	float alpha;
	
	gl_TexCoord[0] = gl_MultiTexCoord0;
	
	vert = gl_Vertex;
	vert.xyz += coefArray1 * morph1.xyz;
	if (numElements>=2)
		vert.xyz += coefArray2 * morph2.xyz;
	if (numElements>=3)
		vert.xyz += coefArray3 * morph3.xyz;
	if (numElements>=4)
		vert.xyz += coefArray4 * morph4.xyz;
	if (numElements>=5)
		vert.xyz += coefArray5 * morph5.xyz;
	
	gl_Position = gl_ModelViewProjectionMatrix * vert;		
	normalF = normalize(gl_NormalMatrix * gl_Normal);
	lightDir = normalize(vec3(gl_LightSource[0].position));
	NdotF = max(dot(normalF, lightDir), 0.0);
	NdotB = max(dot(-normalF, lightDir), 0.0);
	
	diffuseF = gl_FrontMaterial.diffuse * gl_LightSource[0].diffuse;
	ambientF = gl_FrontMaterial.ambient * gl_LightSource[0].ambient;
	globalAmbientF = gl_LightModel.ambient * gl_FrontMaterial.ambient;
	
	diffuseB = gl_BackMaterial.diffuse * gl_LightSource[0].diffuse;
	ambientB = gl_BackMaterial.ambient * gl_LightSource[0].ambient;
	globalAmbientB = gl_LightModel.ambient * gl_BackMaterial.ambient;
		
	gl_FrontColor =  NdotF * diffuseF + globalAmbientF + ambientF;
	gl_BackColor =  NdotB * diffuseB + globalAmbientB + ambientB;
	
	gl_ClipVertex = gl_ModelViewMatrix * vert;
} 
