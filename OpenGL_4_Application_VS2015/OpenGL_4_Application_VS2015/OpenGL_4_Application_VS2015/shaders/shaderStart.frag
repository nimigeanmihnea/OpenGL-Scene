#version 400

struct DirLight {
    vec3 direction;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 FragPos;
in vec3 Normal;
in vec2 passTexture;

out vec4 fragmentColor;

uniform vec3 viewPos;
uniform DirLight dirLight;
uniform PointLight pointLight;
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform int switchOnOf;
uniform int switchPoint;

// Function prototypes
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{    
    // Properties
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    
	if(switchOnOf == 1)
	{
		vec3 result = CalcDirLight(dirLight, norm, viewDir); 
		
		if(switchPoint == 1)
		{
			result += CalcPointLight(pointLight, norm, FragPos, viewDir); 
		}
		fragmentColor = vec4(result, 1.0);
	}
	else
		fragmentColor = texture(diffuseTexture, passTexture);
		
}

// Calculates the color when using a directional light.
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
	
    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
	
    // Specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
	
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0f);
	
    // Combine results
    vec3 ambient = light.ambient * vec3(texture(diffuseTexture, passTexture));
    vec3 diffuse = light.diffuse * diff * vec3(texture(diffuseTexture, passTexture));
    vec3 specular = light.specular * spec * vec3(texture(specularTexture, passTexture));
	
    return (ambient + diffuse + specular);
}

// Calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
	
    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
	
    // Specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0f);
	
    // Attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
	
    // Combine results
    vec3 ambient = light.ambient * vec3(texture(diffuseTexture, passTexture));
    vec3 diffuse = light.diffuse * diff * vec3(texture(diffuseTexture, passTexture));
    vec3 specular = light.specular * spec * vec3(texture(specularTexture, passTexture));
	
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
	
    return (ambient + diffuse + specular);
}