#version 430 core

uniform vec3 objectColor;
uniform vec3 lightDir;

in vec3 interpNormal;
in vec2 texcoord;
in vec3 position;

uniform sampler2D _texture;

void main()
{
	
	if (sin(position.x)>0) {
vec3 normal = normalize(interpNormal);
	float diffuse = max(dot(normal, lightDir), 0.0);

	vec4 texC = texture2D(_texture, texcoord); //fix obrot
	gl_FragColor = vec4(texC.xyz * diffuse, 1.0);
	
	}
	
	else {

	vec3 normal = normalize(interpNormal);
	float diffuse = max(dot(normal, -lightDir), 0.0);

	vec4 texC = texture2D(_texture, texcoord); //fix obrot
	
	gl_FragColor = vec4(texC.xyz * diffuse, 1.0);
	
	}
	

}
