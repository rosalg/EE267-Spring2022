/**
 * @file Fragment shader for foveated rendering
 *
 * @copyright The Board of Trustees of the Leland Stanford Junior University
 * @version 2022/04/14
 */

/* TODO (2.2.4) Fragment Shader Foveation Blur */

var shaderID = "fShaderFoveated";

var shader = document.createTextNode( `
/***
 * WebGL doesn't set any default precision for fragment shaders.
 * Precision for vertex shader is set to "highp" as default.
 * Do not use "lowp". Some mobile browsers don't support it.
 */
precision mediump float;

// texture or uv coordinates of current fragment in normalized coordinates [0,1]
varying vec2 textureCoords;

// texture map from the first rendering pass
uniform sampler2D textureMap;

// resolution of the window in [pixels]
uniform vec2 windowSize;

// window space coordinates of gaze position in [pixels]
uniform vec2 gazePosition;

// eccentricity angle at boundary of foveal and middle layers
uniform float e1;

// eccentricity angle at boundary of middle and outer layers
uniform float e2;

// visual angle of one pixel
uniform float pixelVA;

// radius of middle layer blur kernel [in pixels]
const float middleKernelRad = 2.0;

// radius of outer layer blur kernel [in pixels]
const float outerKernelRad = 4.0;

// gaussian blur kernel for middle layer (5x5)
uniform float middleBlurKernel[int(middleKernelRad)*2+1];

// gaussian blur kernel for outer layer (9x9)
uniform float outerBlurKernel[int(outerKernelRad)*2+1];


void main() {
	float currentEcccentricity = length(textureCoords * windowSize - gazePosition) * pixelVA;

	vec4 color = vec4(0.0);
	if (currentEcccentricity <= e1){
		gl_FragColor = texture2D( textureMap,  textureCoords );
		return;
	} 
	else if(currentEcccentricity <= e2){
		for (int i = -int(middleKernelRad); i <= int(middleKernelRad); i++){
			for (int j = -int(middleKernelRad); j <= int(middleKernelRad); j++){
				vec2 offset = vec2(float(i)/windowSize.x, float(j)/windowSize.y);
				color += middleBlurKernel[int(middleKernelRad) + i] * middleBlurKernel[int(middleKernelRad) + j] * texture2D( textureMap,  textureCoords + offset);
			}
		}
	} else {
		for (int i = -int(outerKernelRad); i <= int(outerKernelRad); i++){
			for (int j = -int(outerKernelRad); j <= int(outerKernelRad); j++){
				vec2 offset = vec2(float(i)/windowSize.x, float(j)/windowSize.y);
				color += outerBlurKernel[int(outerKernelRad) + i] * outerBlurKernel[int(outerKernelRad) + j] * texture2D( textureMap,  textureCoords + offset);
			}
		}
	}

	color.w = 1.0;
	gl_FragColor = color;

}
` );

var shaderNode = document.createElement( "script" );

shaderNode.id = shaderID;

shaderNode.setAttribute( "type", "x-shader/x-fragment" );

shaderNode.appendChild( shader );

document.body.appendChild( shaderNode );
