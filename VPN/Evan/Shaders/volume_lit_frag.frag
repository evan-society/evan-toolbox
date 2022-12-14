uniform sampler3D baseTexture;
uniform float SampleDensityValue;
uniform float TransparencyValue;
uniform float AlphaFuncValue;
uniform vec3 clipabc;
uniform vec3 clipSup;
uniform float clipd;
uniform int clipStyle;

varying vec4 cameraPos;
varying vec4 vertexPos;
varying mat4 texgen;

void main(void)
{ 
	 vec3 t0 = (texgen * vertexPos).xyz;
	 vec3 te = (texgen * cameraPos).xyz;
	 
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

	 vec3 deltaTexCoord=(te-t0)/float(num_iterations-1.0);
	 vec3 texcoord = t0;

	 float normalSampleDistance = 1.0/512.0;
	 vec3 deltaX = vec3(normalSampleDistance, 0.0, 0.0);
	 vec3 deltaY = vec3(0.0, normalSampleDistance, 0.0);
	 vec3 deltaZ = vec3(0.0, 0.0, normalSampleDistance);

	 vec4 fragColor = vec4(0.0, 0.0, 0.0, 0.0); 
	 while(num_iterations>0.0)
	 {
		 vec4 color = texture3D( baseTexture, texcoord);

		 float a = color.a;
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

		 float r = color[3]*TransparencyValue;
		 if (r>AlphaFuncValue)
		 {
			 fragColor.xyz = fragColor.xyz*(1.0-r)+color.xyz*r;
			 fragColor.w += r;
		 }

		 if (fragColor.w<color.w)
		 {
			 fragColor = color;
		 }
		 texcoord += deltaTexCoord; 

		 --num_iterations;
	 }

	 fragColor.w *= TransparencyValue;

	 if (fragColor.w>1.0) fragColor.w = 1.0; 
	 if (fragColor.w<AlphaFuncValue) discard;
	 
	 gl_FragColor = fragColor;
}
