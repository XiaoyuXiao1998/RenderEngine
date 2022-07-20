#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
//layout (location = 2) in vec2 aTexCoords;

out vec3 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


void main()
{

    TexCoords = aPos;
    mat4 view_without_translation   = mat4(mat3(view));
    vec4 pos= projection * view_without_translation * model * vec4(aPos, 1.0);
     gl_Position = pos.xyww;
}