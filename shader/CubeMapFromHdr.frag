#version 330 core
out vec4 FragColor;
in vec3 localPos;


uniform sampler2D HDRMap;


vec2 SampleSphericalMap(vec3 V);
const vec2 invAtan = vec2(0.1591, 0.3183);


void main(){
	vec2 uv = SampleSphericalMap(normalize(-localPos));
	vec3 color = texture(HDRMap,uv).rgb;
	FragColor = vec4(color,1.0);
}


vec2 SampleSphericalMap(vec3 v){
	// u is phi and v is theta
	 vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
	  uv *= invAtan;
    uv += 0.5;
    return uv;
}
