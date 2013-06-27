uniform sampler2D tex; 
uniform int textureUsed;

void main()
{
	vec4 color = texture2D(tex,gl_TexCoord[0].xy ); 
	if (textureUsed != 1)
		gl_FragColor = gl_Color;
	else 
		gl_FragColor = gl_Color * color;
	gl_FragColor.a = gl_FrontMaterial.diffuse.a;
}	
