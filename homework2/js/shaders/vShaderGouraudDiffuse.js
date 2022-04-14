/**
 * @file Gouraud vertex shader with diffuse and ambient light
 *
 * @copyright The Board of Trustees of the Leland Stanford Junior University
 * @version 2021/04/01
 */

/* TODO (2.1.1), (2.1.3) */

var shaderID = "vShaderGouraudDiffuse";

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
		vec3 color;
		vec3 position;
	};

	uniform PointLight pointLights[ NUM_POINT_LIGHTS ];

#endif


void main() {

	// Compute ambient reflection
	vec3 ambientReflection = material.ambient * ambientLightColor;

	vColor = ambientReflection;

	vec4 temp_vertex_pos = (modelViewMat * vec4(position, 1.0));
	vec3 vertex_pos = vec3(temp_vertex_pos) / temp_vertex_pos.w;

	for (int i = 0; i < NUM_POINT_LIGHTS; i++) {
		PointLight light = pointLights[i];
		vec4 temp_light_pos = (viewMat * vec4(light.position, 1.0));
		vec3 light_pos = vec3(temp_light_pos) / temp_light_pos.w;

		vec3 L = normalize(light_pos - vertex_pos);
		vec3 N = normalize(normalMat * normal);
		float max = max(dot(L,N), 0.0);

		float d = length(light_pos - vertex_pos);
		float atten = 1.0/(attenuation[0] + d*attenuation[1] + d*d*attenuation[2]);

		// Diffuse
		vec3 diffuse = max * material.diffuse * light.color;
		vColor += atten * diffuse;
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
