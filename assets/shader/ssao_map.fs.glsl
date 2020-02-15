#version 420 core

uniform mat4 pMatrix;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D kernelNoise;
uniform vec2 noiseScale;
uniform vec3 kernel[16];

uniform float radius;
uniform float bias;
uniform float occlusion_power;

in vec2 texCoordVec2;

out vec3 fragColor;


float ao_factor()
{
	vec3 fragPos = texture(gPosition, texCoordVec2).xyz;
	vec3 normal = normalize(texture(gNormal, texCoordVec2).xyz);
	vec3 randomVec = normalize(
		texture(kernelNoise, texCoordVec2 * noiseScale).xyz);

	// create TBN matrix: from tangent-space to view-space
	vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
	vec3 bitangent = cross(normal, tangent);
	mat3 TBN = mat3(tangent, bitangent, normal);

	// calculate occlusion factor
	float occlusion = 0.0;

	for (int i = 0; i < 16; ++i)
	{
		// get sample position
		vec3 samplePoint = TBN * kernel[i]; // from tangent to view-space
		samplePoint = fragPos + samplePoint * radius;

		// project sample position (to sample texture) (to get position on screen/texture)
		vec4 offset = vec4(samplePoint, 1.0);
		offset = pMatrix * offset; // from view to clip-space
		offset.xyz /= offset.w; // perspective divide
		offset.xyz = offset.xyz * 0.5 + 0.5; // transform to range 0.0 - 1.0
		float screenDepth = texture(gPosition, offset.xy).z; // get depth value of kernel sample

		float sampleDepth = samplePoint.z;

		// range check & accumulate
		float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - screenDepth));
		//rangeCheck = 1.0f;
		occlusion += (screenDepth >= sampleDepth + bias ? 1.0 : 0.0) * rangeCheck;
	}
	occlusion /= 16;
	occlusion = pow(occlusion, occlusion_power);

	return occlusion;
}

void main()
{
	fragColor = vec3(1.0f - ao_factor());
}