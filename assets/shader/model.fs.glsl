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

// Lighting stuff
uniform int lightMode;
uniform vec3 lightPos;
uniform vec3 lightDir;
uniform float lightCutoff;	// in radians
uniform float lightOuterCutoff;

// Camera stuff
uniform vec3 viewPos;

// Material stuff
uniform struct Material {

	vec3 ambientAlbedo;

	bool has_diff_map;
	sampler2D diffuseMap;

	bool has_spec_map;
	sampler2D specularMap;

	bool has_norm_map;
	sampler2D normalMap;

} material;


uniform float ambientStrength = 0.2;
uniform vec3 default_diffuseAlbedo = vec3(0.2, 0.2, 0.2);
uniform vec3 default_specularAlbedo = vec3(0.5, 0.5, 0.5);
uniform int specularPower = 200;

// others
uniform samplerCube skybox;
uniform sampler2D shadowMap;

layout(location = 0) out vec4 fragColor;


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

// from light source to object, in world space
vec3 light_dir_unit(bool isDirectional)
{
	if (isDirectional)
		return normalize(lightDir);
	else
		return -normalize(lightPos - blinnPhongData.fragPos);
}

// from camera to object, in world space
vec3 view_dir_unit()
{
	return normalize(viewPos - blinnPhongData.fragPos);
}

// from object surface, in world space
vec3 halfway_unit(bool isDirectional)
{
	return normalize(-light_dir_unit(isDirectional) + view_dir_unit());
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

float diffuseValue(int type)
{
	vec3 light_dir_unit = type == 0 ? light_dir_unit(true) : light_dir_unit(false);

	return max(dot(normal_unit(), -light_dir_unit), 0.0);
}

vec3 specularAlbedo()
{
	if (material.has_spec_map)
		return vec3(texture(material.specularMap, vertexData.texCoord));
	else
		return default_specularAlbedo;
}

float specularValue(int type)
{
	vec3 halfway_unit = type == 0 ? halfway_unit(true) : halfway_unit(false);

	return pow(
		max(dot(normal_unit(), halfway_unit), 0.0), specularPower);
}


/*-------------------------------------*/
/*	 Calculate Blinn-Phong lightings   */
/*-------------------------------------*/

vec3 directionalLight()                                  
{                     
    vec3 ambient = ambientStrength * ambientAlbedo();
    vec3 diffuse = diffuseValue(0) * diffuseAlbedo();
    vec3 specular = specularValue(0) * specularAlbedo();

    return ambient + diffuse + specular;                        
}                                                                              
                                                                               
vec3 pointLight()                                        
{                
	float d = length(lightPos - blinnPhongData.fragPos);
	float fa = min(1 / (1.0 + 0.75 * d + 0.75 * d * d), 1.0); 

    vec3 ambient = ambientStrength * ambientAlbedo();
    vec3 diffuse = diffuseValue(1) * diffuseAlbedo();
    vec3 specular = specularValue(1) * specularAlbedo();

    return ambient + fa * (diffuse + specular);
} 

vec3 spotLight()
{
	float theta = dot(light_dir_unit(false), normalize(lightDir));

    vec3 ambient = ambientStrength * ambientAlbedo();

	if (theta <= lightOuterCutoff) {
		return ambient;
	}
	else {

		float intensity = 0.0;
		if (theta <= lightCutoff)
			intensity = clamp((theta - lightOuterCutoff) / (lightCutoff - lightOuterCutoff), 0.0, 1.0);
		else
			intensity = 1.0;
		
		vec3 diffuse = diffuseValue(2) * diffuseAlbedo();
		vec3 specular = specularValue(2) * specularAlbedo();

		return ambient + intensity * (diffuse + specular);
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