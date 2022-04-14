/**
 * @file Gouraud vertex shader
 *
 * @copyright The Board of Trustees of the Leland Stanford Junior University
 * @version 2021/04/01
 */

/* TODO (2.1.2) and (2.1.3) */

var shaderID = "vShaderGouraud";

var shader = document.createTextNode( `
/**
 * varying qualifier is used for passing variables from a vertex shader
 * to a fragment shader. In the fragment shader, these variables are
 * interpolated between neighboring vertexes.
 */
varying vec3 vColor; // Color at a vertex

uniform mat4 viewMat;
uniform mat4 projectionMat;
uniform mat4 modelViewMat;
uniform mat3 normalMat;

struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

uniform Material material;

uniform vec3 attenuation;

uniform vec3 ambientLightColor;

attribute vec3 position;
attribute vec3 normal;


/***
 * NUM_POINT_LIGHTS is replaced to the number of point lights by the
 * replaceNumLights() function in teapot.js before the shader is compiled.
 */
#if NUM_POINT_LIGHTS > 0

	struct PointLight {
		vec3 position;
		vec3 color;
	};

	uniform PointLight pointLights[ NUM_POINT_LIGHTS ];

#endif


void main() {

	// Compute ambient reflection
	vec3 ambientReflection = material.ambient * ambientLightColor;

	vColor = ambientReflection;

	vec4 temp_vertex_pos = (modelViewMat * vec4(position, 1.0));
	vec3 vertex_pos = temp_vertex_pos.xyz;

	for (int i = 0; i < NUM_POINT_LIGHTS; i++) {
		PointLight light = pointLights[i];
		vec4 temp_light_pos = (viewMat * vec4(light.position, 1.0));
		vec3 light_pos = temp_light_pos.xyz;

		vec3 L = normalize(light_pos - vertex_pos);
		vec3 N = normalize(normalMat * normal);
		
		// Attenuation
		float d = length(light_pos - vertex_pos);
		float atten = 1.0/(attenuation[0] + d*attenuation[1] + d*d*attenuation[2]);

		// Diffuse
		float max_diffuse = max(dot(L,N), 0.0);
		vec3 diffuse = max_diffuse * material.diffuse * light.color;

		// Specular 
		vec3 R = normalize(-reflect(L, N));
		vec3 V = normalize(-vertex_pos);
		float max_specular = pow(max(dot(R,V), 0.0), material.shininess);
		vec3 specular = max_specular * material.specular * light.color;

		vColor += atten * (diffuse + specular);
	}

	gl_Position =
		projectionMat * modelViewMat * vec4( position, 1.0 );

}
` );

var shaderNode = document.createElement( "script" );

shaderNode.id = shaderID;

shaderNode.setAttribute( "type", "x-shader/x-vertex" );

shaderNode.appendChild( shader );

document.body.appendChild( shaderNode );
