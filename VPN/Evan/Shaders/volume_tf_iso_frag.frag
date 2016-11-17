uniform sampler3D baseTexture;
uniform sampler1D tfTexture;
uniform float SampleDensityValue;
uniform float TransparencyValue;
uniform float IsoSurfaceValue;
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
	
	vec3 eyeDirection = normalize(te-t0);
	
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
	
	vec3 deltaTexCoord=(t0-te)/float(num_iterations-1.0);
	vec3 texcoord = te;
	float previousV = texture3D( baseTexture, texcoord).a;
	
	float normalSampleDistance = 1.0/512.0;
	vec3 deltaX = vec3(normalSampleDistance, 0.0, 0.0);
	vec3 deltaY = vec3(0.0, normalSampleDistance, 0.0);
	vec3 deltaZ = vec3(0.0, 0.0, normalSampleDistance);
	
	
	if (clipStyle == 0) //noclip
	{
		while(num_iterations>0.0)
		{
			float v = texture3D( baseTexture, texcoord).a;
			float m = (previousV-IsoSurfaceValue) * (v-IsoSurfaceValue);
			
			if (m <= 0.0)
			{
				float r = (IsoSurfaceValue-v)/(previousV-v);
				texcoord = texcoord - r*deltaTexCoord;
				
				v = texture3D( baseTexture, texcoord).a;
				vec4 color = texture1D( tfTexture, v);
				
				float px = texture3D( baseTexture, texcoord + deltaX).a;
				float py = texture3D( baseTexture, texcoord + deltaY).a;
				float pz = texture3D( baseTexture, texcoord + deltaZ).a;
				
				float nx = texture3D( baseTexture, texcoord - deltaX).a;
				float ny = texture3D( baseTexture, texcoord - deltaY).a;
				float nz = texture3D( baseTexture, texcoord - deltaZ).a;
				
				vec3 grad = vec3(px-nx, py-ny, pz-nz);
				if (grad.x!=0.0 || grad.y!=0.0 || grad.z!=0.0)
				{
					vec3 normal = normalize(grad);
					float lightScale = 0.1 +  abs(dot(normal.xyz, eyeDirection))*0.9;
					
					color.x *= lightScale;
					color.y *= lightScale;
					color.z *= lightScale;
				}
				
				gl_FragColor = color;
				
				return;
			}

			previousV = v;
			
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
			float v = texture3D( baseTexture, texcoord).a;
			float m = (previousV-IsoSurfaceValue) * (v-IsoSurfaceValue);
			
			if (m <= 0.0 && sign*(dot(texcoord,clipabc)) >= sign*clipd )
			{
				float r = (IsoSurfaceValue-v)/(previousV-v);
				texcoord = texcoord - r*deltaTexCoord;
				
				v = texture3D( baseTexture, texcoord).a;
				vec4 color = texture1D( tfTexture, v);
				
				float px = texture3D( baseTexture, texcoord + deltaX).a;
				float py = texture3D( baseTexture, texcoord + deltaY).a;
				float pz = texture3D( baseTexture, texcoord + deltaZ).a;
				
				float nx = texture3D( baseTexture, texcoord - deltaX).a;
				float ny = texture3D( baseTexture, texcoord - deltaY).a;
				float nz = texture3D( baseTexture, texcoord - deltaZ).a;
				
				vec3 grad = vec3(px-nx, py-ny, pz-nz);
				if (grad.x!=0.0 || grad.y!=0.0 || grad.z!=0.0)
				{
					vec3 normal = normalize(grad);
					float lightScale = 0.1 +  abs(dot(normal.xyz, eyeDirection))*0.9;
					
					color.x *= lightScale;
					color.y *= lightScale;
					color.z *= lightScale;
				}
				
				gl_FragColor = color;
				
				return;
			}

			previousV = v;
			
			texcoord += deltaTexCoord; 
			
			--num_iterations;
		}
	}
	else if (clipStyle == 3)
	{
		while(num_iterations>0.0)
		{
			float v = texture3D( baseTexture, texcoord).a;
			float m = (previousV-IsoSurfaceValue) * (v-IsoSurfaceValue);
			
			if (abs(dot(texcoord,clipabc)-clipd) <= 0.01 && m <= 0.0)
			{
				float r = (IsoSurfaceValue-v)/(previousV-v);
				texcoord = texcoord - r*deltaTexCoord;
				
				v = texture3D( baseTexture, texcoord).a;
				vec4 color = texture1D( tfTexture, v);
				
				float px = texture3D( baseTexture, texcoord + deltaX).a;
				float py = texture3D( baseTexture, texcoord + deltaY).a;
				float pz = texture3D( baseTexture, texcoord + deltaZ).a;
				
				float nx = texture3D( baseTexture, texcoord - deltaX).a;
				float ny = texture3D( baseTexture, texcoord - deltaY).a;
				float nz = texture3D( baseTexture, texcoord - deltaZ).a;
				
				vec3 grad = vec3(px-nx, py-ny, pz-nz);
				if (grad.x!=0.0 || grad.y!=0.0 || grad.z!=0.0)
				{
					vec3 normal = normalize(grad);
					float lightScale = 0.1 +  abs(dot(normal.xyz, eyeDirection))*0.9;
					
					color.x *= lightScale;
					color.y *= lightScale;
					color.z *= lightScale;
				}
				
				gl_FragColor = color;
				
				return;
			}

			previousV = v;
			
			texcoord += deltaTexCoord; 
			
			--num_iterations;
		}
	}
	else if(clipStyle == 4)
	{
		while(num_iterations>0.0)
		{
			float v = texture3D( baseTexture, texcoord).a;
			float m = (previousV-IsoSurfaceValue) * (v-IsoSurfaceValue);
			
			if (m <= 0.0 && texcoord.x>=clipabc.x && texcoord.y>=clipabc.y && texcoord.z>=clipabc.z
					&& texcoord.x<=clipSup.x && texcoord.y<=clipSup.y && texcoord.z<=clipSup.z)
			{
				float r = (IsoSurfaceValue-v)/(previousV-v);
				texcoord = texcoord - r*deltaTexCoord;
				
				v = texture3D( baseTexture, texcoord).a;
				vec4 color = texture1D( tfTexture, v);
				
				float px = texture3D( baseTexture, texcoord + deltaX).a;
				float py = texture3D( baseTexture, texcoord + deltaY).a;
				float pz = texture3D( baseTexture, texcoord + deltaZ).a;
				
				float nx = texture3D( baseTexture, texcoord - deltaX).a;
				float ny = texture3D( baseTexture, texcoord - deltaY).a;
				float nz = texture3D( baseTexture, texcoord - deltaZ).a;
				
				vec3 grad = vec3(px-nx, py-ny, pz-nz);
				if (grad.x!=0.0 || grad.y!=0.0 || grad.z!=0.0)
				{
					vec3 normal = normalize(grad);
					float lightScale = 0.1 +  abs(dot(normal.xyz, eyeDirection))*0.9;
					
					color.x *= lightScale;
					color.y *= lightScale;
					color.z *= lightScale;
				}
				
				gl_FragColor = color;
				
				return;
			}

			previousV = v;
			
			texcoord += deltaTexCoord; 
			
			--num_iterations;
		}  
	}
	
	// we didn't find an intersection so just discard fragment
	discard;
	
}
