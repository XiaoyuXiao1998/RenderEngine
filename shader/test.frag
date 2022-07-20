#version 330 core
out vec4 FragColor;
// Shadow map related variables
#define NUM_SAMPLES 49
#define EPS 1e-3
#define PI 3.141592653589793
#define PI2 6.283185307179586
#define LIGHT_WIDTH 4.0

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
in vec4 FragPosLightSpace;
uniform Material material;
uniform sampler2D shadowMap;
uniform sampler2D texture_diffuse1;
uniform int TextureSamples;

//variables of shadowmaps
uniform bool hasShadowMap;



//function prototypes
vec3 CalcDirLight(DirLight dir_light,vec3 normal, vec3 view_dir);
//shadow map function
float shadowCalculation(vec4 FragPosLightSpace);
//percentage closer filter
float PCF(vec4 fragPosLightSpace,float filterSize);
//percentage closer soft shadow
float PCSS(vec4 fragPosLightSpace);

//uniform sample function for PCF
void uniformDiskSamples( const in vec2 randomSeed );

//find blocker functions for PCSS
float findBlocker(vec3  projCoords, float zReceiver ) ;

vec2 uniformDisk[NUM_SAMPLES];

void main()
{    

    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 radiance = CalcDirLight(dirLight,norm,viewDir);
   

   FragColor = vec4(radiance, 1.0); 
}


//use Blinn-phong model to calculate
vec3 CalcDirLight(DirLight dir_light,vec3 normal, vec3 view_dir){

    float visibility = 0;
    if(hasShadowMap){
       // visibility = PCSS(FragPosLightSpace);
       // visibility = PCF(FragPosLightSpace,7);
        visibility = shadowCalculation(FragPosLightSpace);
    }

  

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

    return ambient + (diffuse + specular) * (1- visibility);
}

float shadowCalculation(vec4 fragPosLightSpace){
 // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
  // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
   // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
   // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
  // check whether current frag pos is in shadow
    float shadow = currentDepth -0.01> closestDepth  ? 1.0 : 0.0;

    return  shadow;

}


//persontage closer filter
float PCF(vec4 fragPosLightSpace,float filterSize){
 // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
  // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;

    float final_shadow = 0;
    uniformDiskSamples(fragPosLightSpace.xy);
    vec2 texelSize = 1.0 / textureSize(shadowMap,0);

    for(int i = 0 ;i < NUM_SAMPLES ;i++){
   // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy + filterSize * uniformDisk[i] * texelSize).r; 
   // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
  // check whether current frag pos is in shadow
    float shadow = currentDepth -0.01> closestDepth  ? 1.0 : 0.0;
    final_shadow += shadow/NUM_SAMPLES;
    }

    return  final_shadow;

}



float rand_2to1(vec2 uv ) { 
  // 0 - 1
	const  float a = 12.9898, b = 78.233, c = 43758.5453;
    float dt = dot( uv.xy, vec2( a,b ) ), sn = mod( dt, PI );
	return fract(sin(sn) * c);
}

highp float rand_1to1(highp float x ) { 
  // -1 -1
  return fract(sin(x)*10000.0);
}

void uniformDiskSamples( const in vec2 randomSeed ) {

  float randNum = rand_2to1(randomSeed);
  float sampleX = rand_1to1( randNum ) ;
  float sampleY = rand_1to1( sampleX ) ;

  float angle = sampleX * PI2;
  float radius = sqrt(sampleY);

  for( int i = 0; i < NUM_SAMPLES; i ++ ) {
    uniformDisk[i] = vec2( radius * cos(angle) , radius * sin(angle)  );

    sampleX = rand_1to1( sampleY ) ;
    sampleY = rand_1to1( sampleX ) ;

    angle = sampleX * PI2;
    radius = sqrt(sampleY);
  }
}



//percentage closer soft shadows
float PCSS(vec4 fragPosLightSpace){
 // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
  // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;

    float final_shadow = 0;
    uniformDiskSamples(fragPosLightSpace.xy);
    vec2 texelSize = 1.0 / textureSize(shadowMap,0);

  // STEP 1: avgblocker depth

  float avg_blocker_depth = findBlocker(projCoords,0.1);

  // STEP 2: penumbra size
  //   depth(blocker to light)/light_width = depth(blocker to shadow) / shadow_width 
  float penumbra_size = LIGHT_WIDTH  * (projCoords.z - avg_blocker_depth) / avg_blocker_depth;

  // STEP 3: filtering

     for(int i = 0 ;i < NUM_SAMPLES ;i++){
   // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy + penumbra_size * uniformDisk[i] * texelSize).r; 
   // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
  // check whether current frag pos is in shadow
    float shadow = currentDepth -0.01> closestDepth  ? 1.0 : 0.0;
    final_shadow += shadow/NUM_SAMPLES;
    }

    return final_shadow;
  

}


//find average blocker depth
float findBlocker( vec3  projCoords, float zReceiver ){

    float final_dep = 0;
    
    float sample_radius =  (projCoords.z- zReceiver)/projCoords.z * LIGHT_WIDTH;
    vec2 texelSize = 1.0 / textureSize(shadowMap,0);

    //now sample depth
     for(int i = 0 ;i < NUM_SAMPLES ;i++){
   // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy + sample_radius * uniformDisk[i] * texelSize).r; 

     final_dep += closestDepth;
   
    }
    final_dep  = final_dep/NUM_SAMPLES;


    return final_dep;
}