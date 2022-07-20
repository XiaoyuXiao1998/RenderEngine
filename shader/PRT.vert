#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout(location = 3) in mat3 aPrecomputeLT;


out vec3 Color;

uniform mat3 aPrecomputeLR;
uniform mat3 aPrecomputeLG;
uniform mat3 aPrecomputeLB;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

#define PI 3.1415926535

void main(){

    gl_Position = projection * view * model * vec4(aPos, 1.0);
     Color  =vec3( dot(matrixCompMult(aPrecomputeLR ,aPrecomputeLT) * vec3(1,1,1),vec3(1,1,1)) ,
     dot(matrixCompMult(aPrecomputeLG ,aPrecomputeLT) * vec3(1,1,1),vec3(1,1,1)) ,
     dot(matrixCompMult(aPrecomputeLB ,aPrecomputeLT) * vec3(1,1,1),vec3(1,1,1)) ) / PI * 2;
   // float  a = aPrecomputeLT[0][0];
    //if(a<0.7) a = 0.7;
   // Color= vec3(a,a,a);


}