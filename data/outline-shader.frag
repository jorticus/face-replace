uniform sampler2D tex;
//uniform vec2 texSize;
uniform vec4 outlineColor;
uniform float outlineWidth;

void main(void)
{
	vec2 off = vec2(outlineWidth, outlineWidth);
	vec2 tc = gl_TexCoord[0].st;

	vec4 c = texture2D(tex, tc);
	vec4 n = texture2D(tex, vec2(tc.x, tc.y - off.y));
	vec4 e = texture2D(tex, vec2(tc.x + off.x, tc.y));
	vec4 s = texture2D(tex, vec2(tc.x, tc.y + off.y));
	vec4 w = texture2D(tex, vec2(tc.x - off.x, tc.y));
	
	vec4 origColor = c * gl_Color;

	float ua = 0.0;
	ua = mix(ua, 1.0, c.a);
	ua = mix(ua, 1.0, n.a);
	ua = mix(ua, 1.0, e.a);
	ua = mix(ua, 1.0, s.a);
	ua = mix(ua, 1.0, w.a);

	vec4 underColor = outlineColor * vec4(ua);

	gl_FragColor = underColor;
	gl_FragColor = mix(gl_FragColor, origColor, origColor.a);

	//if (gl_FragColor.a > 0.0)
	//	gl_FragColor.a = 1.0;
}
