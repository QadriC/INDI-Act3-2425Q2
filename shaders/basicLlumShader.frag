#version 330 core

in vec3 vertexSCO;
in vec3 normalSCO;
in vec3 matamb_interp;
in vec3 matdiff_interp;
in vec3 matspec_interp;
in float matshin_interp;

in vec3 posFocusSubSCO[4];
in vec3 dirFocusSubSCO[4];

uniform vec3 llumAmbient;
uniform vec3 colorFocus;
uniform vec3 posFocus;
uniform vec3 colorFocusSub[4];
uniform bool enabledFocus[4];
uniform int pintarVidres, pintarMar;

uniform mat4 proj;

out vec4 FragColor;


// 2D Random
float random (in vec2 st) {
    return fract(sin(dot(st.xy,
                         vec2(12.9898,78.233)))
                 * 43758.5453123);
}

// 2D Noise based on Morgan McGuire @morgan3d
// https://www.shadertoy.com/view/4dS3Wd
float noise (in vec2 st) {
    vec2 i = floor(st);
    vec2 f = fract(st);

    // Four corners in 2D of a tile
    float a = random(i);
    float b = random(i + vec2(1.0, 0.0));
    float c = random(i + vec2(0.0, 1.0));
    float d = random(i + vec2(1.0, 1.0));

    // Smooth Interpolation

    // Cubic Hermine Curve.  Same as SmoothStep()
    vec2 u = f*f*(3.0-2.0*f);
    // u = smoothstep(0.,1.,f);

    // Mix 4 coorners percentages
    return mix(a, b, u.x) +
            (c - a)* u.y * (1.0 - u.x) +
            (d - b) * u.x * u.y;
}

void main()
{
    vec3 N = normalize(normalSCO);
    vec3 L = normalize(posFocus - vertexSCO);
    vec3 V = normalize(-vertexSCO);
    vec3 R = reflect(-L, N);
	
	// Model Phong
	
    vec3 ambient = llumAmbient * matamb_interp;
    float diff = max(dot(N, L), 0.0);
    vec3 diffuse = colorFocus * matdiff_interp * diff;

    float spec = 0.0;
    if (diff > 0.0 && matshin_interp > 0.0)
        spec = pow(max(dot(R, V), 0.0), matshin_interp);
    vec3 specular = colorFocus * matspec_interp * spec;
	
	// Llums submarí
	
	vec3 subLightsColor = vec3(0.0);

	float theta = radians(8.0);
	float betaMax = 0.0;
	for (int i = 0; i < 4; ++i) {
		vec3 Ls = normalize(posFocusSubSCO[i] - vertexSCO);
		float d = distance(posFocusSubSCO[i], vertexSCO);
		if (d < 0.5) {
		    float diff = max(dot(N, Ls), 0.0);
		    vec3 Rs = reflect(-Ls, N);
		    float spec = pow(max(dot(Rs, V), 0.0), matshin_interp);
		    vec3 diffuse = vec3(1.0) * matdiff_interp * diff;
		    vec3 specular = vec3(1.0) * matspec_interp * spec;
		    subLightsColor += diffuse + specular;
		}
		if(enabledFocus[i]) {
			vec3 a = dirFocusSubSCO[i];
			vec3 b = normalize(vertexSCO - posFocusSubSCO[i]);
			float alfa = acos(dot(a, b));
			if(alfa < theta) {
				float beta = ( (cos(alfa) - cos(theta)) / (1.0 - cos(theta)) );
				
				float diff = max(dot(N, Ls), 0.0);
				vec3 Rs = reflect(-Ls, N);
				float spec = pow(max(dot(Rs, V), 0.0), matshin_interp);
				vec3 diffuse = colorFocusSub[i] * matdiff_interp * diff;
				vec3 specular = colorFocusSub[i] * matspec_interp * spec;

				subLightsColor += beta * (diffuse + specular);
			}
		}
	}

	
    vec3 colorFinal = ambient;
    colorFinal += (diffuse + specular);
    colorFinal += (subLightsColor);
    
    float distMin = 8.0;
	float distMax = 15.0;
	float d = length(vertexSCO);
	float f = smoothstep(distMin, distMax, d);
	colorFinal = mix(colorFinal, vec3(0.0), f);
    
    bool esVidre = matdiff_interp == vec3(0.0, 0.0, 1.0);
    float alpha = 1.0;
    if(pintarMar == 1) {
    	// Coordenadas del ruido
        float n = noise(vertexSCO.xy * 1.0);
        vec3 aigua = vec3(0.1, 0.4, 0.6);
        colorFinal = mix(aigua, vec3(1.0), n);
    	alpha = 0.2;
    	
    	float theta = radians(8.0);
		for (int i = 0; i < 4; ++i) {
			if (!enabledFocus[i]) continue;
			
			vec4 posClip = proj * vec4(posFocusSubSCO[i], 1.0);
		    vec4 targetClip = proj * vec4(posFocusSubSCO[i] + dirFocusSubSCO[i], 1.0);
		    vec4 fragClip = proj * vec4(vertexSCO, 1.0);
			
			vec2 posNDC = posClip.xy / posClip.w;
		    vec2 targetNDC = targetClip.xy / targetClip.w;
		    vec2 fragNDC = fragClip.xy / fragClip.w;
			
			vec2 a = normalize(targetNDC - posNDC);
			vec2 b = normalize(fragNDC - posNDC);
			float alfa = acos(dot(a, b));

			if (alfa < theta) {
				float beta = (cos(alfa) - cos(theta)) / (1.0 - cos(theta));
				colorFinal += beta * colorFocusSub[i];
			}
		}
	} else {
		if (pintarVidres == 0 && esVidre)
		    discard;
		if (pintarVidres == 1 && !esVidre)
		    discard;
		if(esVidre)
			alpha = 0.7;
    }
    FragColor = vec4(colorFinal, alpha);
}

