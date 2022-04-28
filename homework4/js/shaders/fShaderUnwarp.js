/**
 * @file Unwarp fragment shader
 *
 * @copyright The Board of Trustees of the Leland Stanford Junior University
 * @version 2022/04/21
 */

/* TODO (2.2.2) Fragment shader implementation */

var shaderID = "fShaderUnwarp";

var shader = document.createTextNode( `
/**
 * WebGL doesn't set any default precision for fragment shaders.
 * Precision for vertex shader is set to "highp" as default.
 * Do not use "lowp". Some mobile browsers don't support it.
 */

precision mediump float;

varying vec2 textureCoords;

// texture rendered in the first rendering pass
uniform sampler2D map;

// center of lens for un-distortion
// in normalized coordinates between 0 and 1
uniform vec2 centerCoordinate;

// [width, height] size of viewport in [mm]
// viewport is the left/right half of the browser window
uniform vec2 viewportSize;

// lens distortion parameters [K_1, K_2]
uniform vec2 K;

// distance between lens and screen in [mm]
uniform float distLensScreen;

void main() {

	float r_tilde = distance(viewportSize * textureCoords, viewportSize * centerCoordinate);
    float r = r_tilde / distLensScreen;

    float distortion = 1.0 + K[0] * pow(r, 2.0) + K[1] * pow(r, 4.0);
	// Start from center coord, shift in respect to center, distort, and add center coord back
	vec2 d_offset = (textureCoords - centerCoordinate) * distortion;
    vec2 xy = centerCoordinate + d_offset;

	gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
    if (xy[0] >= 0.0 && xy[0] <= 1.0 && xy[1] >= 0.0 && xy[1] <= 1.0) {
		gl_FragColor = texture2D(map, xy);
    }

}
` );


var shaderNode = document.createElement( "script" );

shaderNode.id = shaderID;

shaderNode.setAttribute( "type", "x-shader/x-fragment" );

shaderNode.appendChild( shader );

document.body.appendChild( shaderNode );
