uniform sampler3D baseTexture;
uniform float SampleDensityValue;
uniform float TransparencyValue;
uniform float AlphaFuncValue;
uniform vec3 clipabc;
uniform vec3 clipSup;
uniform float clipd;
uniform int clipStyle;
uniform int isOrtho;

varying vec4 cameraPos;
varying vec4 vertexPos;
varying mat4 texgen;

void main(void)
{ 
	vec3 t0 = (texgen * vertexPos).xyz;
	vec3 te = (texgen * cameraPos).xyz;
	
	//correcting the direction of rays if in orthographic views
	if (isOrtho == 1)
		te = te + t0 - vec3(0.5,0.5,0.5); //all rays are now parallel to the ray that goes from the camera to the centre of the volume

	if (te.x>=0.0 && te.x<=1.0 &&
			te.y>=0.0 && te.y<=1.0 &&
			te.z>=0.0 && te.z<=1.0)
	{
		// do nothing... te inside volume
	}
	else
	{
		if (te.x<0.0)
		{
			float r = -te.x / (t0.x-te.x);
			te = te + (t0-te)*r;
		}

		if (te.x>1.0)
		{
			float r = (1.0-te.x) / (t0.x-te.x);
			te = te + (t0-te)*r;
		}

		if (te.y<0.0)
		{
			float r = -te.y / (t0.y-te.y);
			te = te + (t0-te)*r;
		}

		if (te.y>1.0)
		{
			float r = (1.0-te.y) / (t0.y-te.y);
			te = te + (t0-te)*r;
		}

		if (te.z<0.0)
		{
			float r = -te.z / (t0.z-te.z);
			te = te + (t0-te)*r;
		}

		if (te.z>1.0)
		{
			float r = (1.0-te.z) / (t0.z-te.z);
			te = te + (t0-te)*r;
		}
	}

	const float max_iteratrions = 2048.0;
	float num_iterations = ceil(length(te-t0)/SampleDensityValue);
	if (num_iterations<2.0) num_iterations = 2.0;

	if (num_iterations>max_iteratrions) 
	{
		num_iterations = max_iteratrions;
	}

	vec3 deltaTexCoord=(te-t0)/float(num_iterations-1.0);
	vec3 texcoord = t0;

	vec4 fragColor = vec4(0.0, 0.0, 0.0, 0.0); 
	
	if (clipStyle == 0) //noclip
	{
		while(num_iterations>0.0)
		{
			
				vec4 color = texture3D( baseTexture, texcoord);
				if (fragColor.w<color.w)
				{
					fragColor = color;
				}
			
			texcoord += deltaTexCoord;
			--num_iterations;
		}
	}
	else if (clipStyle == 1 || clipStyle == 2)
	{
		float sign;
		if (clipStyle==1)
			sign=1.0;
		else
			sign=-1.0;
		while(num_iterations>0.0)
		{
			if (sign*(dot(texcoord,clipabc)) >= sign*clipd)
			{
				vec4 color = texture3D( baseTexture, texcoord);
				if (fragColor.w<color.w)
				{
					fragColor = color;
				}
			}
			texcoord += deltaTexCoord;

			--num_iterations;
		}
	}
	else if (clipStyle == 3)
	{
		while(num_iterations>0.0)
		{
			if (abs(dot(texcoord,clipabc)-clipd) <= 0.02)
			{
				vec4 color = texture3D( baseTexture, texcoord);
				if (fragColor.w<color.w)
				{
					fragColor = color;
				}
			}
			texcoord += deltaTexCoord;

			--num_iterations;
		}
	}
	else if(clipStyle == 4)
	{
		while(num_iterations>0.0)
		{
			if (texcoord.x>=clipabc.x && texcoord.y>=clipabc.y && texcoord.z>=clipabc.z
					&& texcoord.x<=clipSup.x && texcoord.y<=clipSup.y && texcoord.z<=clipSup.z)
			{
				vec4 color = texture3D( baseTexture, texcoord);
				if (fragColor.w<color.w)
				{
					fragColor = color;
				}
			}
			texcoord += deltaTexCoord;

			--num_iterations;
		}
	}

	fragColor.w *= TransparencyValue;

	if (fragColor.w>1.0) fragColor.w = 1.0; 
	if (fragColor.w<AlphaFuncValue) discard;
	
	gl_FragColor = fragColor;
}
