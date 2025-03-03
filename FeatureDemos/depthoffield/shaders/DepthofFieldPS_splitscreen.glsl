// simple fragment shader

// 'time' contains seconds since the program was linked.
//uniform float time;

uniform sampler2D RTexture;
uniform sampler2D depthTex;
uniform sampler2D RTexture2;
uniform sampler2D depthTex2;
uniform vec2 res;
float middle = texture2D(depthTex, res/2).r + texture2D(depthTex, res/2).g/ 256.0;
float middle2 = texture2D(depthTex2, res/2).r + texture2D(depthTex2, res/2).g/ 256.0;

float samples[] ={0.0755906,0.1309775,0.1756663,0.0755906,
0.1284325,0.0590321,0.1790372};
int j;
int i;


void main()
{
	vec4 sum = vec4(0);
	vec2 texcoord = vec2(gl_TexCoord[0].x, (res.y-gl_TexCoord[0].y)*2);
	float diff = abs((middle-texture2D(depthTex, texcoord).r - texture2D(depthTex, texcoord).g/ 256.0)*3);
	float diff2 = abs((middle2-texture2D(depthTex2, texcoord).r - texture2D(depthTex2, texcoord).g/ 256.0)*3);

	if (gl_TexCoord[0].y<0.5)
	{
		for( i= -2 ;i < 2; i++)
		{
			for( j= -2 ;j < 2; j++)
			{
				sum += texture2D(RTexture, texcoord + vec2(j, i)*0.003*diff) * samples[j+2];
			}
		}
	}
	else
	{
		for( i= -2 ;i < 2; i++)
		{
			for( j= -2 ;j < 2; j++)
			{
				sum += texture2D(RTexture2, texcoord + vec2(j, i)*0.003*diff2) * samples[j+2];
			}
		}
	}

	gl_FragColor = sum*0.55;
}
