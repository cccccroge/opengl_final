#version 420 core

in VertexData
{
    vec3 position;
    vec3 normal;
    vec2 texCoord;
	mat3 TBN;

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

// Lights
struct DirectionalLight {
	vec3 position;
	vec3 direction;

	vec3 color;
	float intensity;
};

struct PointLight {
	vec3 position;	// in world space

	vec3 color;
	float intensity;

	float att_constant;
	float att_linear;
	float att_quadratic;
};

struct SpotLight {
	vec3 position;		// in world space
	vec3 direction;
	float cutoff;		// cos of radian, not radian!
	float outerCutoff;	// cos of radian, not radian!

	vec3 color;
	float intensity;

	float att_constant;
	float att_linear;
	float att_quadratic;
};

uniform DirectionalLight direcitonalLights[3];
uniform PointLight pointLights[50];
uniform SpotLight spotLights[10];

uniform int NUM_OF_DIRECTIONAL_LIGHT;
uniform int NUM_OF_POINT_LIGHT;
uniform int NUM_OF_SPOT_LIGHT;

uniform double BIAS_BASE;
uniform double BIAS_FACTOR;

// Camera
uniform vec3 viewPos;

// Material
uniform struct Material {

	vec3 ambientAlbedo;

	bool has_diff_map;
	sampler2D diffuseMap;

	bool has_spec_map;
	sampler2D specularMap;

	bool has_norm_map;
	sampler2D normalMap;

} material;


uniform float ambientStrength = 0.08;
uniform vec3 default_diffuseAlbedo = vec3(0.2, 0.2, 0.2);
uniform vec3 default_specularAlbedo = vec3(0.5, 0.5, 0.5);
uniform int specularPower = 200;

// others
uniform samplerCube skybox;
uniform sampler2D shadowMap;

layout(location = 0) out vec4 fragColor;


/*-------------------------------------*/
/*	   Helper unit vector functions	   */
/*-------------------------------------*/

// from object surface, in world space
vec3 normal_unit()
{
	if (material.has_norm_map) {
		vec3 normal = texture(material.normalMap, vertexData.texCoord).xyz;
		normal = normalize(normal * 2.0 - 1.0);	// color -> spatial
		normal = normalize(vertexData.TBN * normal);	// tangent space -> world space
		return normal;
	}
	else
		return normalize(blinnPhongData.normal);
}

// from camera to object, in world space
vec3 view_dir_unit()
{
	return normalize(viewPos - blinnPhongData.fragPos);
}


/*-------------------------------------*/
/*	   Shadow Calculation functions	   */
/*-------------------------------------*/

float shadow_factor(vec4 fragPos, vec3 light_dir_unit)   // light space
{
	// perspective divide (in case the light camera is perspective)
	// [-x, x] -> [-1, 1]
	vec3 proj_coord = fragPos.xyz / fragPos.w;

	// transform to the range 0~1
	// [-1, 1] -> [0, 1]
	proj_coord = proj_coord * 0.5 + 0.5;

	// calculate shadow factor
	float current_depth = proj_coord.z;

	// outside light spectrum
	if (proj_coord.z > 1.0)
		return 0.0;

	// we should bias less if the face is parallel to the light
	double bias = (1 - dot(normal_unit(), -light_dir_unit)) * BIAS_FACTOR + BIAS_BASE;

	float shadow_factor = 0;
	vec2 texel_size = 1.0 / textureSize(shadowMap, 0);
	int sample_texel_len = 1;

	for (int i = -sample_texel_len; i <= sample_texel_len; ++i) {
		for (int j = -sample_texel_len; j <= sample_texel_len; ++j) {
			float closest_depth = texture(shadowMap,
				proj_coord.xy + vec2(i, j) * texel_size).r;
			shadow_factor += (current_depth - bias > closest_depth ? 1.0 : 0.0);
		}
	}
	int sample_num = 2 * sample_texel_len + 1;
	shadow_factor /= pow(sample_num, 2);

	return shadow_factor;
}


/*-------------------------------------*/
/*	   Helper Blinn-Phong functions	   */
/*-------------------------------------*/

vec3 ambientAlbedo()
{
	return vec3(texture(material.diffuseMap, vertexData.texCoord));
}

vec3 diffuseAlbedo()
{
	if (material.has_diff_map)
		return vec3(texture(material.diffuseMap, vertexData.texCoord));
	else
		return default_diffuseAlbedo;
}

vec3 specularAlbedo()
{
	if (material.has_spec_map)
		return vec3(texture(material.specularMap, vertexData.texCoord));
	else
		return default_specularAlbedo;
}


/*-------------------------------------*/
/*	 Calculate Blinn-Phong lightings   */
/*-------------------------------------*/

vec3 oneDirectionalLight(DirectionalLight light)
{                
	// diffuse
	vec3 light_dir_unit = normalize(light.direction);
	float diffuse_value = max(dot(normal_unit(), -light_dir_unit), 0.0);
    vec3 diffuse = diffuse_value * diffuseAlbedo();

	// specular
	vec3 halfway_unit = normalize(-light_dir_unit + view_dir_unit());
	float specular_value = pow(
		max(dot(normal_unit(), halfway_unit), 0.0), specularPower);
    vec3 specular = specular_value * specularAlbedo();


    vec3 mix = (light.color * light.intensity) * (diffuse + specular);
	// add shadow
	//vec3 shadow_light_unit_dir = normalize(blinnPhongData.fragPos - light.position);
	vec3 add_shadow = mix * (1 - shadow_factor(shadowData.fragPos, light_dir_unit));

	return add_shadow;
}                                                                              
                                  
vec3 onePointLight(PointLight light)
{        
	// diffuse
	vec3 light_dir_unit = normalize(blinnPhongData.fragPos - light.position);
	float diffuse_value = max(dot(normal_unit(), -light_dir_unit), 0.0);
    vec3 diffuse = diffuse_value * diffuseAlbedo();

	// specular
	vec3 halfway_unit = normalize(-light_dir_unit + view_dir_unit());
	float specular_value = pow(
		max(dot(normal_unit(), halfway_unit), 0.0), specularPower);
    vec3 specular = specular_value * specularAlbedo();

	// diffuse and specular factor
	float d = length(light.position - blinnPhongData.fragPos);
	float factor = min(
		1 / (light.att_constant + light.att_linear * d + light.att_quadratic * d * d),
		1.0);

    return (light.color * light.intensity) * factor * (diffuse + specular);
} 

vec3 oneSpotLight(SpotLight light)
{
	vec3 light_dir_unit = normalize(blinnPhongData.fragPos - light.position);
	vec3 head_to_unit = normalize(light.direction);
	float theta = dot(light_dir_unit, head_to_unit);


	if (theta <= light.outerCutoff) {
		return vec3(0, 0, 0);
	}
	// diffuse & specular
	else {

		float stand = 0.0;
		if (theta <= light.cutoff)
			stand = clamp(
				(theta - light.outerCutoff) / (light.cutoff - light.outerCutoff),
				0.0, 1.0);
		else
			stand = 1.0;

		float diffuse_value = max(dot(normal_unit(), -light_dir_unit), 0.0);
		vec3 diffuse = diffuse_value * diffuseAlbedo();

		vec3 halfway_unit = normalize(-light_dir_unit + view_dir_unit());
		float specular_value = pow(
			max(dot(normal_unit(), halfway_unit), 0.0), specularPower);
		vec3 specular = specular_value * specularAlbedo();

		float d = length(light.position - blinnPhongData.fragPos);
		float factor = min(
			1 / (light.att_constant + light.att_linear * d + light.att_quadratic * d * d),
			1.0);

		return (light.color * light.intensity) * stand * factor * (diffuse + specular);
	}
}

vec3 blinn_phong()
{
	vec3 accum = vec3(0.0, 0.0, 0.0);

	// default ambient
	vec3 ambient = ambientStrength * ambientAlbedo();
	accum += ambient;

	// directional lights
	for (int i = 0; i < NUM_OF_DIRECTIONAL_LIGHT; ++i) {
		accum += oneDirectionalLight(direcitonalLights[i]);
	}

	// point lights
	for (int i = 0; i < NUM_OF_POINT_LIGHT; ++i) {
		accum += onePointLight(pointLights[i]);
	}

	// spot lights
	for (int i = 0; i < NUM_OF_SPOT_LIGHT; ++i) {
		accum += oneSpotLight(spotLights[i]);
	}

	return accum;
}

void main()
{
    vec3 blinn_phong_col = blinn_phong();
	fragColor = vec4(blinn_phong_col, 1.0);
}