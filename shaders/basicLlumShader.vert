#version 330 core

in vec3 vertex;
in vec3 normal;

in vec3 matamb;
in vec3 matdiff;
in vec3 matspec;
in float matshin;

out vec3 vertexSCO;
out vec3 normalSCO;
out vec3 matamb_interp;
out vec3 matdiff_interp;
out vec3 matspec_interp;
out float matshin_interp;

out vec3 posFocusSubSCO[4];
out vec3 dirFocusSubSCO[4];

uniform mat4 proj;
uniform mat4 view;
uniform mat4 TG;

uniform vec3 posFocusSub[4];
uniform vec3 posAntena;

void main()
{
    mat4 MV = view * TG;
    vec4 vertSCO = MV * vec4(vertex, 1.0);
    vertexSCO = vertSCO.xyz;
    
    mat3 normalMatrix = transpose(inverse(mat3(MV)));
    normalSCO = normalize(normalMatrix * normal);

	vec3 dirFocusSub[4];
	for (int i = 0; i < 4; ++i) {
        posFocusSubSCO[i] = vec3(view * vec4(posFocusSub[i], 1.0));	
        dirFocusSub[i] = normalize(posFocusSub[i] - posAntena);
        dirFocusSubSCO[i] = normalize(mat3(view) * dirFocusSub[i]);
    }
	
    matamb_interp = matamb;
    matdiff_interp = matdiff;
    matspec_interp = matspec;
    matshin_interp = matshin;

    gl_Position = proj * vertSCO;
}

