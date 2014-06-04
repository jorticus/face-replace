//
// This shader is applied to the face overlay texture
//

uniform vec2 iResolution;

uniform vec2 lumaCorrect; // (min, max) luminance

uniform sampler2D overlayTexture;
uniform sampler2D backgroundTexture;


// http://www.poynton.com/ColorFAQ.html
// RGB <> YUV conversion matrices (http://en.wikipedia.org/wiki/YUV)

// YCbCr
mat3 rgb2yuv = mat3(
     0.299,   0.587,    0.144,
    -0.1687, -0.3313,   0.5,
     0.5,    -0.4187,  -0.0813
);
mat3 yuv2rgb = mat3(
     1.0,     0.0,      1.402,
     1.0,    -0.34414, -0.71414,
     1.0,     1.772,    0.0
);

// YUV
/*mat3 rgb2yuv = mat3(
    0.299,    0.587,   0.114,
   -0.14713, -0.28886, 0.436,
    0.615,   -0.515,  -0.1
);
mat3 yuv2rgb = mat3(
    1.0,  0.0,      1.13983,
    1.0, -0.39465, -0.58060,
    1.0,  2.03211,  0.0
);*/


void main()
{
    //// YUV Luminance Blend ////

    // Overlay texture colour (RGBA)
    vec2 fg_uv = gl_TexCoord[0].xy;
	vec4 fg_color = texture2D(overlayTexture, fg_uv);

    // Video/Background texture colour (RGBA)
    vec2 bg_uv = (gl_FragCoord.xy / iResolution) * vec2(1.0, -1.0) + vec2(0.0, 1.0);
	vec4 bg_color = texture2D(backgroundTexture, bg_uv);

    // Convert RGBA to YUV colour space
    vec3 fg_yuv = fg_color.rgb * rgb2yuv;
    vec3 bg_yuv = bg_color.rgb * rgb2yuv;

    // Blend luminance of overlay with chrominance of background
    // and apply level corrections
    float Y = fg_yuv[0];
    Y = Y / (lumaCorrect[1] - lumaCorrect[0]) + lumaCorrect[0];
    vec3 blend_yuv = vec3(Y, bg_yuv.yz);

    // Convert back to RGBA colour space (Alpha channel from the overlay texture)
    vec3 blend_rgb = blend_yuv * yuv2rgb;
	//gl_FragColor = vec4(blend_rgb, fg_color.a);

    // Work-around for an alpha-blending bug in SFML when drawing to a texture instead of the screen
    gl_FragColor = vec4((blend_rgb * fg_color.a) + (bg_color.rgb * (1.0-fg_color.a)), 1.0);
}


