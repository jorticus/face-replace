//
// This shader is applied to the face overlay texture
//

uniform vec2 iResolution;

uniform sampler2D overlayTexture;
uniform sampler2D backgroundTexture;



// RGB <> YCbCr conversion matrices
// NOTE: Cb/Cr channels are in the range [-0.5, 0.5]
mat3 rgb2ycbcr = mat3(
     0.29900,   0.58700,   0.14400,
    -0.16894,  -0.33167,   0.50059,
     0.49981,  -0.41853,  -0.08128
);
mat3 ycbcr2rgb = mat3(
     0.97087,  -0.05155,   1.40252,
     0.97087,  -0.39528,  -0.71440,
     0.97089,   1.71835,  -0.00001
);


void main()
{
    //// YCbCr Luminance Blend ////

    // Overlay texture colour (RGBA)
    vec2 fg_uv = gl_TexCoord[0].xy;
	vec4 fg_rgba = texture2D(overlayTexture, fg_uv);

    // Video/Background texture colour (RGBA)
    vec2 bg_uv = (gl_FragCoord.xy / iResolution) * vec2(1.0, -1.0) + vec2(0.0, 1.0);
	vec4 bg_rgba = texture2D(backgroundTexture, bg_uv);

    // Convert RGBA to YCbCr colour space
    vec3 fg_ycbcr = fg_rgba.xyz * rgb2ycbcr;
    vec3 bg_ycbcr = bg_rgba.xyz * rgb2ycbcr;

    // Blend luminance of overlay with chrominance of background
    vec3 blend_ycbcr = vec3(fg_ycbcr[0], bg_ycbcr[1], bg_ycbcr[2]);

    // Convert back to RGBA colour space (Alpha channel from the overlay texture)
    vec4 blend_rgba = vec4(blend_ycbcr * ycbcr2rgb, fg_rgba[3]);

	gl_FragColor = blend_rgba;
}


