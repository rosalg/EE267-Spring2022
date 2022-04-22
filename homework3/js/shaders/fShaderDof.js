/**
 * @file Fragment shader for DoF rendering
 *
 * @copyright The Board of Trustees of the Leland Stanford Junior University
 * @version 2022/04/14
 */

/* TODO (2.3) DoF Rendering */

var shaderID = "fShaderDof";

var shader = document.createTextNode( `
/**
 * WebGL doesn't set any default precision for fragment shaders.
 * Precision for vertex shader is set to "highp" as default.
 * Do not use "lowp". Some mobile browsers don't support it.
 */
precision mediump float;

// uv coordinates after interpolation
varying vec2 textureCoords;

// texture map from the first rendering
uniform sampler2D textureMap;

// depth map from the first rendering
uniform sampler2D depthMap;

// Projection matrix used for the first pass
uniform mat4 projectionMat;

// Inverse of projectionMat
uniform mat4 invProjectionMat;

// resolution of the window in [pixels]
uniform vec2 windowSize;

// Gaze position in [pixels]
uniform vec2 gazePosition;

// Diameter of pupil in [mm]
uniform float pupilDiameter;

// pixel pitch in [mm]
uniform float pixelPitch;

const float searchRad = 11.0;


// Compute the distance to fragment in [mm]
// p: texture coordinate of a fragment / a gaze position
//
// Note: GLSL is column major
float distToFrag( vec2 p ) {

	/* TODO (2.3.1) Distance to Fragment */
	vec3 ndc = vec3(2.0 * texture2D(textureMap, p).x - 1.0, 
		2.0 * texture2D(textureMap, p).y - 1.0, 
		2.0 * texture2D(depthMap, p) - 1.0);

	float w_clip = projectionMat[3][2]/(ndc.z + projectionMat[2][2]);
    vec4 clip = vec4(ndc, 1) * w_clip; //ndc
    vec4 view = invProjectionMat * clip;
    return length(view);
}


// compute the circle of confusion in [mm]
// fragDist: distance to current fragment in [mm]
// focusDist: distance to focus plane in [mm]
float computeCoC( float fragDist, float focusDist ) {

	/* TODO (2.3.2) Circle of Confusion Computation */

	return pupilDiameter * abs(fragDist - focusDist)/fragDist;
}


// compute depth of field blur and return color at current fragment
vec3 computeBlur() {

	/* TODO (2.3.3) Retinal Blur */
	float fragDist = distToFrag(textureCoords);
    float focusDist = distToFrag(gazePosition/windowSize);
    float blur = computeCoC(fragDist, focusDist);
    float blurRad = 0.5*blur/pixelPitch;
	vec4 color = vec4(0.0);
	float numAdded = 0.0;
	
	for(int i = -int(searchRad); i <= int(searchRad); i++ ){
		for(int j = -int(searchRad); j <= int(searchRad); j++ ){
			if (blurRad * blurRad > float(i*i + j*j)) {
				vec2 offset = vec2(float(i)/windowSize.x, float(j)/windowSize.y);
				color += texture2D(textureMap, textureCoords + offset);
				numAdded += 1.0; 
			}
		}
	}
	return color.xyz / numAdded;
}


void main() {
	vec4 color = vec4(computeBlur(), 1.0);
	gl_FragColor = color;

}
` );


var shaderNode = document.createElement( "script" );

shaderNode.id = shaderID;

shaderNode.setAttribute( "type", "x-shader/x-fragment" );

shaderNode.appendChild( shader );

document.body.appendChild( shaderNode );
