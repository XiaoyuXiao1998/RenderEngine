#version 330 core
out vec4 FragColor;

//variables of object materials
in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;


//***************************maps and modes************************************************************
uniform sampler2D baseColorMap;
uniform sampler2D normalMap;
uniform sampler2D metallicRoughnessMap;
uniform sampler2D BRDFLUT;



uniform vec4 baseColorFactor;
uniform float metallicFactor;
uniform float roughnessFactor;


uniform int baseColorMode;
uniform int metallicMode;
uniform int alphaMode ;
uniform float alphaCutOff;
uniform int roughnessMode;
uniform bool useNormalMap;


//********************************IBL related variables*******************************************************
uniform samplerCube DiffuseIrradianceMap;
uniform samplerCube PrefilteredMap;




uniform vec3 viewPos;

const float PI = 3.14159265359;

//dir light realted properties
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

//uniform float uRoughness;

//************************ functions ****************************************************
vec3 fresnelSchlick(vec3 F0, vec3 V, vec3 H);
float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float k);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
vec4 getBaseColor();
float getMetallic();
float getRoughness();

void main(){
    //******************************matalic material part***********************************************
    float metallic = getMetallic();
    vec4 baseColor = getBaseColor();
    vec3 albedo = baseColor.rgb;

	//ensure normal vector are normalized
	vec3 N = normalize(Normal);
	vec3 V = normalize(viewPos - FragPos);
	float NdotV = max(dot(N, V), 0.0);

	//vec3 L =  normalize(-dirLight.direction);
    vec3 L =  normalize(LightPos - FragPos);
	vec3 H = normalize(V+L);
	float NdotL = max(dot(N, L), 0.0); 
	vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);


	float uRoughness = getRoughness();
	float NDF = DistributionGGX(N, H, uRoughness);   
	float G   = GeometrySmith(N, V, L, uRoughness); 
	vec3 F = fresnelSchlick(F0, V, H);

	vec3 numerator    = NDF * G * F; 
	float denominator = max((4.0 * NdotL * NdotV), 0.001);
	vec3 BRDF = numerator / denominator;

	 

     //******************************IBL  part***********************************************
     vec3 IBL_Color = vec3(0);
     //KS part is fresnel part
     vec3 KS  = fresnelSchlick(F0, V, H);
     vec3 KD =(vec3(1.0) -KS)* (1.f-metallic );
     IBL_Color+= texture(DiffuseIrradianceMap,L).rgb*albedo * KD;
     
     vec2 scale_bias =  texture(BRDFLUT, vec2(max(dot(N,V),0.0),uRoughness)).rg;
      vec3 R= reflect(-V,N);
     vec3 prefilterdColor = textureLod(PrefilteredMap,R,uint(4*uRoughness)).rgb;
     IBL_Color += prefilterdColor* albedo * (scale_bias.x * F0 + scale_bias.g);
     
     //******************************sum  part***********************************************
     vec3 Lo = vec3(0.0);
	 Lo +=  (albedo * BRDF+  (1.f-F ) * albedo/PI) * NdotL;
     //+ IBL_Color;
	 vec3 color = IBL_Color ;

      // HDR tonemapping
	color = color / (color + vec3(1.0));
        // gamma correct
	color = pow(color, vec3(1.0/4.2)); 

 ;


    gl_FragColor = vec4(color, 1.0);
      


}



//**************************************************get color  function*******************************************************************
vec4 getBaseColor(){
    vec4 color = vec4(1);
    if(baseColorMode==2){
        color =  texture(baseColorMap,TexCoords).rgba;
    }else if(baseColorMode == 4){
        color = baseColorFactor;
    }else if(baseColorMode ==6){
        color = baseColorFactor*texture(baseColorMap,TexCoords).rgba;
    }
    return vec4(pow(color.rgb,vec3(2.2)),color.a);
}

float getMetallic(){
    float m = 0;
    if(metallicMode==2){
        m =  texture(metallicRoughnessMap,TexCoords).b;
    }else if(metallicMode == 4){
        m = metallicFactor;
    }else if(metallicMode ==6){
        m = metallicFactor*texture(baseColorMap,TexCoords).b;
    }
    return m;
}
float getRoughness(){    
float m = 0;
    if(roughnessMode==2){
        m =  texture(metallicRoughnessMap,TexCoords).g;
    }else if(roughnessMode == 4){
        m = roughnessFactor;
    }else if(roughnessMode ==6){
        m = roughnessFactor*texture(metallicRoughnessMap,TexCoords).g;
    }
    return m;

}


//**************************************************F G D function*******************************************************************


vec3 fresnelSchlick(vec3 F0, vec3 V, vec3 H){
   float cos_theta = dot(V,H);
   return F0 + (1.0- F0)*pow(1.0- cos_theta,5.0);

};

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
   // TODO: To calculate GGX NDF here
    float alpha = roughness * roughness;
	float alpha_2 = alpha * alpha;
	float NdotH = dot(N, H);
	return alpha_2 /PI/pow(NdotH * NdotH * (alpha_2 -1.0 ) + 1.0,2.0);
}

float GeometrySchlickGGX(float NdotV, float k)
{
    // TODO: To calculate Schlick G1 here
    
    return NdotV / ( NdotV * (1.0- k) + k); 
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    // TODO: To calculate Smith G here
	float k = pow(roughness + 1.0,2.0)/8.0;
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);

    return GeometrySchlickGGX(NdotV,k) * GeometrySchlickGGX(NdotL,k);
}