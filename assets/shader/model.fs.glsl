#version 420 core

in VertexData
{
    vec3 position;
    vec3 normal;
    vec2 texCoord;

} vertexData;

in BlinnPhongData
{
    vec3 fragPos;
    vec3 normal;    // in world space

} blinnPhongData;

in ShadowData
{
    vec4 fragPos;   // in light space

} shadowData;

uniform int lightMode;
uniform vec3 lightPos;
uniform vec3 lightDir;
uniform float lightCutoff;	// in radians
uniform float lightOuterCutoff;

uniform vec3 viewPos;
uniform float ambientStrength;
uniform vec3 ambientAlbedo;
//uniform vec3 diffuseAlbedo;
uniform vec3 specularAlbedo;
uniform int specularPower;

uniform samplerCube skybox;
uniform sampler2D shadowMap;
uniform sampler2D tex;

layout(location = 0) out vec4 fragColor;


vec4 environment_map()
{
    // calculate sample direction for skybox
    vec3 view_dir_unit = normalize(viewPos - blinnPhongData.fragPos);
    vec3 normal_unit = normalize(blinnPhongData.normal);
    vec3 sample_dir = reflect(-view_dir_unit, normal_unit);

    // sample color from skybox
    vec4 col = texture(skybox, sample_dir)/* * vec4(0.95, 0.80, 0.45, 1.0)*/;

    return col;
}

float shadow_factor(vec4 fragPos)   // light space
{
    // perspective divide (in case the light camera is perspective)
    // [-x, x] -> [-1, 1]
    vec3 proj_coord = fragPos.xyz / fragPos.w;

    // transform to the range 0~1
    // [-1, 1] -> [0, 1]
    proj_coord = proj_coord * 0.5 + 0.5;

    // calculate shadow factor
    float current_depth = proj_coord.z;

    vec3 normal_unit = normalize(blinnPhongData.normal);
    vec3 light_dir_unit = normalize(lightPos - blinnPhongData.fragPos);
    // we should bias less if the face is parallel to the light
    float bias = (1 - dot(normal_unit, light_dir_unit)) * 0.0005 + 0.00005;

    float shadow_factor = 0;
    vec2 texel_size = 1.0 / textureSize(shadowMap, 0);
    int sample_texel_len = 1;

    for (int i = -sample_texel_len; i <= sample_texel_len; ++i) {
        for (int j = -sample_texel_len; j <= sample_texel_len; ++j) {
            float closest_depth = texture(shadowMap, 
                proj_coord.xy + vec2(i, j) * texel_size).r;
            shadow_factor += (current_depth - bias > closest_depth ? 0.8 : 0.0);  
        }
    }
    int sample_num = 2 * sample_texel_len + 1;
    shadow_factor /= pow(sample_num, 2);

    return shadow_factor;
}

vec3 directionalLight()                                  
{                     
	// Ambient
    vec3 ambient = ambientStrength * ambientAlbedo;

    // Diffuse
    vec3 normal_unit = normalize(blinnPhongData.normal);
    vec3 light_dir_unit = normalize(lightDir);
    float diffuse_value = max(dot(normal_unit, -light_dir_unit), 0.0);
    vec3 diffuseAlbedo = vec3(texture(tex, vertexData.texCoord));
    vec3 diffuse = diffuse_value * diffuseAlbedo;

    // Specular
    vec3 view_dir_unit = normalize(viewPos - blinnPhongData.fragPos);
    vec3 halfway_unit = normalize(-light_dir_unit + view_dir_unit);
    float specular_value = pow(
        max(dot(normal_unit, halfway_unit), 0.0), specularPower);
    vec3 specular = specular_value * specularAlbedo;

    return ambient + diffuse + specular;                        
}                                                                              
                                                                               
vec3 pointLight()                                        
{                
	float d = length(lightPos - blinnPhongData.fragPos);
	float fa = min(1 / (1.0 + 0.75 * d + 0.75 * d * d), 1.0); 

	// Ambient
    vec3 ambient = ambientStrength * ambientAlbedo;

    // Diffuse
    vec3 normal_unit = normalize(blinnPhongData.normal);
    vec3 light_dir_unit = -normalize(lightPos - blinnPhongData.fragPos);
    float diffuse_value = max(dot(normal_unit, -light_dir_unit), 0.0);
    vec3 diffuseAlbedo = vec3(texture(tex, vertexData.texCoord));
    vec3 diffuse = diffuse_value * diffuseAlbedo;

    // Specular
    vec3 view_dir_unit = normalize(viewPos - blinnPhongData.fragPos);
    vec3 halfway_unit = normalize(-light_dir_unit + view_dir_unit);
    float specular_value = pow(
        max(dot(normal_unit, halfway_unit), 0.0), specularPower);
    vec3 specular = specular_value * specularAlbedo;

    return ambient + fa * (diffuse + specular);
} 

vec3 spotLight()
{
	float theta = dot(normalize(lightPos - blinnPhongData.fragPos), normalize(-lightDir));

	// Ambient
    vec3 ambient = ambientStrength * ambientAlbedo;

	if (theta <= lightOuterCutoff) {
		return ambient;
	}
	else {

		//float d = length(lightPos - blinnPhongData.fragPos);
		//float fa = min(1 / (1.0 + 0.75 * d + 0.75 * d * d), 1.0); 

		float intensity = 0.0;
		if (theta <= lightCutoff)
			intensity = clamp((theta - lightOuterCutoff) / (lightCutoff - lightOuterCutoff), 0.0, 1.0);
		else
			intensity = 1.0;
		
		// Diffuse
		vec3 normal_unit = normalize(blinnPhongData.normal);
		vec3 light_dir_unit = -normalize(lightPos - blinnPhongData.fragPos);
		float diffuse_value = max(dot(normal_unit, -light_dir_unit), 0.0);
		vec3 diffuseAlbedo = vec3(texture(tex, vertexData.texCoord));
		vec3 diffuse = diffuse_value * diffuseAlbedo;

		// Specular
		vec3 view_dir_unit = normalize(viewPos - blinnPhongData.fragPos);
		vec3 halfway_unit = normalize(-light_dir_unit + view_dir_unit);
		float specular_value = pow(
			max(dot(normal_unit, halfway_unit), 0.0), specularPower);
		vec3 specular = specular_value * specularAlbedo;

		return ambient + /* fa **/ intensity * (diffuse + specular);
	}

}

vec3 blinn_phong()
{
	// directional light
	if (lightMode == 0) {
		return directionalLight();
	}
	// point light
	else if (lightMode == 1) {
		return pointLight();
	}
	// spot light
	else if (lightMode == 2) {
		return spotLight();
	}
}

void main()
{
    vec3 blinn_phong_col = blinn_phong();
    vec3 add_shadow = blinn_phong_col * (1 - shadow_factor(shadowData.fragPos));
    fragColor = vec4(add_shadow, 1.0);
}