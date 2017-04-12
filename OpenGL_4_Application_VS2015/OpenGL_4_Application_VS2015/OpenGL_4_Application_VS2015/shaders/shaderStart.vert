#version 400

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 textcoord;

out vec3 Normal;
out vec2 passTexture;
out vec3 FragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {

	passTexture = textcoord;
    FragPos = vec3(model * vec4(vertexPosition, 1.0f));
	Normal = mat3(transpose(inverse(model))) * vertexNormal;
	gl_Position = projection * view * model * vec4(vertexPosition, 1.0f);
}
