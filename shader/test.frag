#version 330 core
out vec4 FragColor;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;    
    float shininess;
}; 

struct DirLight {
    vec3 direction;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

//light variables
uniform DirLight dirLight;

//phong related variables
uniform vec3 LightPos;
uniform vec3 LightIntensity;
uniform vec3 viewPos;

//variables of object materials
in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;
uniform Material material;
uniform sampler2D texture_diffuse1;
uniform int TextureSamples;


//function prototypes
vec3 CalcDirLight(DirLight dir_light,vec3 normal, vec3 view_dir);

void main()
{    

    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 radiance = CalcDirLight(dirLight,norm,viewDir);
    FragColor = vec4(radiance, 1.0);
}


//use Blinn-phong model to calculate
vec3 CalcDirLight(DirLight dir_light,vec3 normal, vec3 view_dir){

    vec3 color;
    if(TextureSamples == 1){
        color = pow(texture(texture_diffuse1, TexCoords).rgb,vec3(2.2));
    }
    else{
        color = material.diffuse;
    }
    vec3 lightDir = normalize(-dir_light.direction);
    vec3 ambient = dir_light.ambient * color;
    
    //diffuse light 
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = dir_light.diffuse * diff * color;
    

    //specular light
    vec3 halfVec = normalize(lightDir + view_dir);
    float spec = pow(max(dot(halfVec,normal),0.0),32.0);
    vec3 specular = dir_light.specular * spec * material.specular;

    return ambient + diffuse + specular;
}
