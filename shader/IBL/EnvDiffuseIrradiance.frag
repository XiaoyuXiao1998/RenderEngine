#version 330 core
out vec4 FragColor;
in vec3 localPos;

uniform samplerCube environmentMap;

//InitialPos is the viewpoint location when using IBL in the space that are not large enough;
// the default initial pos is vec(0,0,0);
uniform vec3 InitialPos;

const float PI = 3.14159265359;

const float TWO_PI= 6.28318530718;

//sampling function
float RadicalInverse( uint bits );
vec2 Hammersley(uint i,uint N);
//u is theta (0-PI)  v is PHI(0-2PI)
vec3 hemisphereSample_uniform(float u, float v);
 vec3 hemisphereSample_cos(float u, float v);
 vec4 Quat_Inverse(vec4 q);
 vec3 Quat_Rotate(vec4 q, vec3 p);
 vec4 Quat_ZTo(vec3 to);
 vec3 CosOnHalfSphere(vec2 Xi, vec3 N);
 vec3 CosOnHalfSphere(vec2 Xi);
 
vec2 UniformOnDisk(float Xi);

void main(){


	 vec3 irradiance = vec3(0.0);
	vec3 normal  = normalize(localPos - InitialPos);
	uint sample_num = 1000u;
	for(uint i = 0u;i < sample_num;i++){
    vec2 hammersleyVec = Hammersley(i,sample_num);
    vec3 L = CosOnHalfSphere(hammersleyVec, normal);
	irradiance += texture(environmentMap, L).xyz;
	}


    irradiance /= float(sample_num);
    FragColor = vec4(irradiance, 1.0);
}




//sample functions 

vec3 hemisphereSample_uniform(float u, float v){
	float phi = v * 2 * PI;
	float cosTheta = 1.0f - u;
	float sinTheta = sqrt(1.0 - cosTheta * cosTheta);

	return vec3(sinTheta * cos(phi),sinTheta*sin(phi),cosTheta);
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

vec2 Hammersley(uint i,uint N){
          return vec2(float(i) / float(N), RadicalInverse(i));
}


vec3 Quat_Rotate(vec4 q, vec3 p) {
	// Quat_Mul(Quat_Mul(q, vec4(p, 0)), Quat_Inverse(q)).xyz;

	vec4 qp = vec4(q.w * p + cross(q.xyz, p), - dot(q.xyz, p));
	vec4 invQ = Quat_Inverse(q);
	vec3 qpInvQ = qp.w * invQ.xyz + invQ.w * qp.xyz + cross(qp.xyz, invQ.xyz);
	return qpInvQ;
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


vec3 CosOnHalfSphere(vec2 Xi, vec3 N) {
	vec3 p = CosOnHalfSphere(Xi);
	vec4 rot = Quat_ZTo(N);
	return Quat_Rotate(rot, p);

}

vec3 CosOnHalfSphere(vec2 Xi) {
	float r = sqrt(Xi.x);
	vec2 pInDisk = r * UniformOnDisk(Xi.y);
	float z = sqrt(1 - Xi.x);
	return vec3(pInDisk, z);
}

vec2 UniformOnDisk(float Xi) {
	float theta = TWO_PI * Xi;
	return vec2(cos(theta), sin(theta));
}