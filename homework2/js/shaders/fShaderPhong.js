/**
 * @file Phong fragment shader
 *
 * @copyright The Board of Trustees of the Leland Stanford Junior University
 * @version 2021/04/01
 */

/* TODO (2.2.2) */

var shaderID = "fShaderPhong";

var shader = document.createTextNode( `
/**
 * WebGL doesn't set any default precision for fragment shaders.
 * Precision for vertex shader is set to "highp" as default.
 * Do not use "lowp". Some mobile browsers don't support it.
 */
precision mediump float;

varying vec3 normalCam; // Normal in view coordinate
varying vec3 fragPosCam; // Fragment position in view cooridnate

uniform mat4 viewMat;

struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

uniform Material material;

uniform vec3 attenuation;

uniform vec3 ambientLightColor;


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

	vec3 fColor = ambientReflection;

	for (int i = 0; i < NUM_POINT_LIGHTS; i++) {
		PointLight light = pointLights[i];
		vec4 temp_light_pos = (viewMat * vec4(light.position, 1.0));
		vec3 light_pos = temp_light_pos.xyz;

		vec3 L = normalize(light_pos - fragPosCam);
		vec3 N = normalize(normalCam);
		
		// Attenuation
		float d = length(light_pos - fragPosCam);
		float atten = 1.0/(attenuation[0] + d*attenuation[1] + d*d*attenuation[2]);

		// Diffuse
		float max_diffuse = max(dot(L,N), 0.0);
		vec3 diffuse = max_diffuse * material.diffuse * light.color;

		// Specular 
		vec3 R = normalize(-reflect(L, N));
		vec3 V = normalize(-fragPosCam);
		float max_specular = pow(max(dot(R,V), 0.0), material.shininess);
		vec3 specular = max_specular * material.specular * light.color;

		fColor += atten * (diffuse + specular);
	}

	gl_FragColor = vec4( fColor, 1.0 );

}
` );


var shaderNode = document.createElement( "script" );

shaderNode.id = shaderID;

shaderNode.setAttribute( "type", "x-shader/x-fragment" );

shaderNode.appendChild( shader );

document.body.appendChild( shaderNode );
