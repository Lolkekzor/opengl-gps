#version 410 core

in vec3 normal;
in vec4 fragPosEye;
in vec4 fragPosLightSpace;
in vec2 fragTexCoords;

out vec4 fColor;

//lighting
uniform	mat3 normalMatrix; 
uniform mat3 lightDirMatrix;
uniform	vec3 lightColor;
uniform	vec3 lightDir;
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform sampler2D shadowMap;

vec3 ambient;
float ambientStrength = 0.1;
vec3 diffuse;
vec3 specular;
float specularStrength = 1.5f;
float shininess = 64.0f;

vec3 o;

//FOG
in float visibility;
vec3 skyColor = vec3(0.44,0.53,0.6); // +0.0,0.10,0.15

//POINT LIGHT
in vec4 worldPosition;
const vec3 pointLightColor = vec3(0.0f, 0.7f, 0.7f);
in vec3 toLightVector;
in vec3 modelNormal;

const float att_1 = 0.01f;
const float att_2 = 0.001f;
const float att_3 = 0.0002f;
const float lightPower = 0.5f;

void computeLightComponents()
{	
	vec3 cameraPosEye = vec3(0.0f);//in eye coordinates, the viewer is situated at the origin
	
	//transform normal
	vec3 normalEye = normalize(normalMatrix * normal);	
	
	//compute light direction
	vec3 lightDirN = normalize(lightDirMatrix * lightDir);	

	//compute view direction 
	vec3 viewDirN = normalize(cameraPosEye - fragPosEye.xyz);
	
	//compute half vector
	vec3 halfVector = normalize(lightDirN + viewDirN);
		
	//compute ambient light
	ambient = ambientStrength * lightColor;
	
	//compute diffuse light
	diffuse = max(dot(normalEye, lightDirN), 0.0f) * lightColor;
	
	//compute specular light
	float specCoeff = pow(max(dot(halfVector, normalEye), 0.0f), shininess);
	specular = specularStrength * specCoeff * lightColor;

	float distance = length(toLightVector);
	float attFactor = att_1 + att_2 * distance + att_3 * distance * distance;

	diffuse += (max(dot(normalize(modelNormal), normalize(toLightVector)), 0.0f) * pointLightColor / attFactor) * lightPower;
}

float computeShadow()
{	
	// perform perspective divide
    vec3 normalizedCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    if(normalizedCoords.z > 1.0f)
        return 0.0f;
    // Transform to [0,1] range
    normalizedCoords = normalizedCoords * 0.5f + 0.5f;
    // Get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, normalizedCoords.xy).r;    
    // Get depth of current fragment from light's perspective
    float currentDepth = normalizedCoords.z;
    // Check whether current frag pos is in shadow
    float bias = 0.001f;
    float shadow = currentDepth - bias> closestDepth  ? 1.0f : 0.0f;

    return shadow;	
}

const float shadowStrength = 0.85f;
void main() 
{

	computeLightComponents();
	
	float shadow = computeShadow() * shadowStrength;
	
	//modulate with diffuse map
	ambient *= vec3(texture(diffuseTexture, fragTexCoords));
	diffuse *= vec3(texture(diffuseTexture, fragTexCoords));
	//modulate woth specular map - DIFFUSE FOR NOW
	specular *= vec3(texture(diffuseTexture, fragTexCoords));
	
	//modulate with shadow
	vec3 color = min((ambient + (1.0f - shadow)*diffuse) + (1.0f - shadow)*specular, 1.0f);
    
    fColor = vec4(color, 1.0f);
	fColor = mix(vec4(skyColor, 1.0), fColor, visibility);
    //fColor = vec4(o, 1.0f);
}
