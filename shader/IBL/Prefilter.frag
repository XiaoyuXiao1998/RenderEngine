
#version 330 core

out vec4 FragColor;
in vec3 localPos;

uniform samplerCube environmentMap;
uniform float roughness;
uniform float resolution;
vec2 Hammersley(uint i,uint N);
float RadicalInverse( uint bits );
vec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float roughness);
 vec3 Quat_Rotate(vec4 q, vec3 p);
 vec4 Quat_ZTo(vec3 to);
  vec4 Quat_Inverse(vec4 q);


const float PI = 3.14159265359;

const float TWO_PI= 6.28318530718;
void main(){

    vec3 N = normalize(localPos);
    vec3 R = N;
    // vi the output vector
    vec3 V = R;


    uint SAMPLE_COUNT = 1024u;

    vec3 radiance = vec3(0.0);
    float total_weight =0.0;
    for(uint i = 0u ;i <SAMPLE_COUNT; i++){

        //importance sampling
        vec2 hammersleyVec = Hammersley(i,SAMPLE_COUNT);
        

        vec3 H = ImportanceSampleGGX(hammersleyVec,N,roughness);
        // note that H is just a normalized vector
        vec3 L = vec3( 2.0 * H * dot(V , H)- V );

        float NdotL = max(dot(N,L),0.0);
        if(NdotL > 0.0){
            
            unsigned int  MipMapLevel = unsigned int(roughness  * 4);
            radiance += textureLod(environmentMap,L,MipMapLevel).rgb;
            total_weight +=  NdotL;
        }


    }

    radiance /= total_weight;
    FragColor = vec4(radiance,1.0);





     

}


vec2 Hammersley(uint i,uint N){
     return vec2(float(i) / float(N), RadicalInverse(i));
}


float RadicalInverse( uint bits ){
          //reverse bit
          //高低16位换位置
          bits = (bits << 16u) | (bits >> 16u); 
          //A是5的按位取反
          bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
          //C是3的按位取反
          bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
          bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
          bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
          return  float(bits) * 2.3283064365386963e-10;
}


vec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float roughness){
    //xi is a uniform random vector
    float alpha = roughness * resolution;

    float cosTheta = sqrt((1.0 - Xi.r)/ (alpha * alpha * Xi.r + 1 - Xi.r));
    float sinTheta = sqrt(1.0 - cosTheta * cosTheta);
    float phi  = TWO_PI * Xi.g;

    vec3 HOnHemisphere;
    HOnHemisphere.x = sinTheta * cos(phi);
    HOnHemisphere.y = sinTheta * sin(phi);
    HOnHemisphere.z = cosTheta ;

    vec4 rot = Quat_ZTo(N);
	return Quat_Rotate(rot, HOnHemisphere);

    //form tangent-space H to world space H






    return vec3(0.0);
}

vec4 Quat_Inverse(vec4 q) {
	return vec4(-q.xyz, q.w);
}

vec4 Quat_ZTo(vec3 to) {
	// from = (0, 0, 1)
	//float cosTheta = dot(from, to);
	//float cosHalfTheta = sqrt(max(0, (cosTheta + 1) * 0.5));
	float cosHalfTheta = sqrt(max(0, (to.z + 1) * 0.5));
	//vec3 axisSinTheta = cross(from, to);
	//    0    0    1
	// to.x to.y to.z
	//vec3 axisSinTheta = vec3(-to.y, to.x, 0);
	float twoCosHalfTheta = 2 * cosHalfTheta;
	return vec4(-to.y / twoCosHalfTheta, to.x / twoCosHalfTheta, 0, cosHalfTheta);
}

vec3 Quat_Rotate(vec4 q, vec3 p) {
	// Quat_Mul(Quat_Mul(q, vec4(p, 0)), Quat_Inverse(q)).xyz;

	vec4 qp = vec4(q.w * p + cross(q.xyz, p), - dot(q.xyz, p));
	vec4 invQ = Quat_Inverse(q);
	vec3 qpInvQ = qp.w * invQ.xyz + invQ.w * qp.xyz + cross(qp.xyz, invQ.xyz);
	return qpInvQ;
}