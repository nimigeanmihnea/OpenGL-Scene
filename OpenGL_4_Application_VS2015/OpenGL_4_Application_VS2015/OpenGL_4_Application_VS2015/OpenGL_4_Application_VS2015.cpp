//
//  main.cpp
//  OpenGL_Shader_Example_step1
//
//  Created by CGIS on 02/11/16.
//  Copyright � 2016 CGIS. All rights reserved.
//

#define GLEW_STATIC

#include <iostream>
#include "glm/glm.hpp"//core glm functionality
#include "glm/gtc/matrix_transform.hpp"//glm extension for generating common transformation matrices
#include "glm/gtc/type_ptr.hpp"
#include "GLEW/glew.h"
#include "GLFW/glfw3.h"
#include <string>
#include "Shader.hpp"
#include "Camera.hpp"
#define TINYOBJLOADER_IMPLEMENTATION

#include "Model3D.hpp"
#include "Mesh.hpp"

#pragma disable warnings
int glWindowWidth = 1920;
int glWindowHeight = 1080;
int retina_width, retina_height;
GLFWwindow* glWindow = NULL;

glm::mat4 model;
GLint modelLoc;

gps::Camera myCamera(glm::vec3(0.0f, 5.0f, 15.0f), glm::vec3(0.0f, 2.0f, -10.0f));
float cameraSpeed = 1.0f;

bool pressedKeys[1024];
float angle = 0.0f;

gps::Model3D myModel;
gps::Model3D myModel1;
gps::Model3D myModel2;
gps::Model3D myModel3;
gps::Model3D myModel4;
gps::Model3D myModel5;
gps::Model3D myModel6;
gps::Model3D myModel7;
gps::Model3D myModel8;


gps::Shader myCustomShader;

void windowResizeCallback(GLFWwindow* window, int width, int height)
{
	fprintf(stdout, "window resized to width: %d , and height: %d\n", width, height);
	//TODO
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			pressedKeys[key] = true;
		else if (action == GLFW_RELEASE)
			pressedKeys[key] = false;
	}
}

// horizontal angle : toward -Z
float horizontalAngle;
// vertical angle : 0, look at the horizon
float verticalAngle;
// Initial Field of View
float initialFoV;

void mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	float centerX = 1980 / 2, centerY = 1080 / 2;
	
	// position
	glm::vec3 position = glm::vec3(0, 0, 5);

	float speed = 2.0f; // 2 units / second
	float mouseSpeed = 0.0005f;

	horizontalAngle += mouseSpeed * float(centerX - xpos);
	verticalAngle += mouseSpeed * float(centerY - ypos);

	myCamera.rotate(horizontalAngle, verticalAngle);

	glfwSetInputMode(glWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	glfwSetCursorPos(glWindow, centerX, centerY);

}
int switchOnOf = 1;
int switchPoint = 1;
void processMovement()
{
	if (pressedKeys[GLFW_KEY_Q]) {
		angle += 0.02;
	}

	if (pressedKeys[GLFW_KEY_E]) {
		angle -= 0.02;
	}

	if (pressedKeys[GLFW_KEY_W]) {
		myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_S]) {
		myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_A]) {
		myCamera.move(gps::MOVE_LEFT, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_D]) {
		myCamera.move(gps::MOVE_RIGHT, cameraSpeed);
	}
	if (pressedKeys[GLFW_KEY_1]) {
		switchOnOf = 1;
	}
	if (pressedKeys[GLFW_KEY_2]) {
		switchOnOf = 0;
	}
	if (pressedKeys[GLFW_KEY_4]) {
		switchPoint = 1;
	}
	if (pressedKeys[GLFW_KEY_5]) {
		switchPoint = 0;
	}
}

bool initOpenGLWindow()
{
	if (!glfwInit()) {
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return false;
	}

	//for Mac OS X
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glWindow = glfwCreateWindow(glWindowWidth, glWindowHeight, "OpenGL Shader Example", NULL, NULL);
	if (!glWindow) {
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
		return false;
	}

	glfwSetWindowSizeCallback(glWindow, windowResizeCallback);
	glfwMakeContextCurrent(glWindow);

	glfwWindowHint(GLFW_SAMPLES, 4);

	// start GLEW extension handler
	glewExperimental = GL_TRUE;
	glewInit();

	// get version info
	const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
	const GLubyte* version = glGetString(GL_VERSION); // version as a string
	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", version);

	//for RETINA display
	glfwGetFramebufferSize(glWindow, &retina_width, &retina_height);

	glfwSetKeyCallback(glWindow, keyboardCallback);
	glfwSetCursorPosCallback(glWindow, mouseCallback);
	//glfwSetInputMode(glWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	return true;
}

GLuint ReadTextureFromFile(const char* file_name) {
	int x, y, n;
	int force_channels = 4;
	unsigned char* image_data = stbi_load(file_name, &x, &y, &n, force_channels);
	if (!image_data) {
		fprintf(stderr, "ERROR: could not load %s\n", file_name);
		return false;
	}
	// NPOT check
	if ((x & (x - 1)) != 0 || (y & (y - 1)) != 0) {
		fprintf(
			stderr, "WARNING: texture %s is not power-of-2 dimensions\n", file_name
		);
	}

	int width_in_bytes = x * 4;
	unsigned char *top = NULL;
	unsigned char *bottom = NULL;
	unsigned char temp = 0;
	int half_height = y / 2;

	for (int row = 0; row < half_height; row++) {
		top = image_data + row * width_in_bytes;
		bottom = image_data + (y - row - 1) * width_in_bytes;
		for (int col = 0; col < width_in_bytes; col++) {
			temp = *top;
			*top = *bottom;
			*bottom = temp;
			top++;
			bottom++;
		}
	}

	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_SRGB, //GL_SRGB,//GL_RGBA,
		x,
		y,
		0,
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		image_data
	);
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	return textureID;
}

GLuint verticesVBO;
GLuint verticesEBO;
GLuint objectVAO;
GLint texture;

//vertex position and UV coordinates
GLfloat vertexData[] = {
	// first triangle
	-5.0f, 0.0f, 0.0f, 0.0f, 0.0f,
	5.0f,0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 8.0f, 0.0f, 0.5f, 1.0f,
	// second triangle
	0.1f, 8.0f, 0.0f, 1.0f, 0.0f,
	5.1f, 0.0f, 0.0f, 0.5f, 1.0f,
	10.1f, 8.0f, 0.0f, 0.0f, 0.0f
};

GLuint vertexIndices[] = {
	0,1,2,
	3,4,5
};

float delta = 0.0f;

float movementSpeed = 0.6; // units per second
void updateDelta(double elapsedSeconds) {
	delta = delta + movementSpeed * elapsedSeconds;
}
double lastTimeStamp = glfwGetTime();

void renderScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.8, 0.8, 0.8, 1.0);
	glViewport(0, 0, retina_width, retina_height);

	//initialize the model matrix
	model = glm::mat4(1.0f);
	modelLoc = glGetUniformLocation(myCustomShader.shaderProgram, "model");

	processMovement();

	myCustomShader.useShaderProgram();

	//initialize the view matrix
	glm::mat4 view = myCamera.getViewMatrix();
	//send matrix data to shader
	GLint viewLoc = glGetUniformLocation(myCustomShader.shaderProgram, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	////send matrix data to vertex shader
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "diffuseTexture"), 0);

	//sending specular texture
	glActiveTexture(GL_TEXTURE1);
	glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "specularTexture"), 1);

	//direct light
	glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "dirLight.direction"), 0.4f, 1.0f, 0.5f);
	glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "dirLight.ambient"), 0.2f, 0.3f, 0.3f);
	glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "dirLight.diffuse"), 0.3f, 0.3f, 0.4f);
	glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "dirLight.specular"), 0.4f, 0.4f, 0.4f);

	//point light
	glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "pointLight.position"), -0.529f, 64.0f, 14.235f);
	glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "pointLight.ambient"), 5.0f, 5.0f, 5.0f);
	glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "pointLight.diffuse"), 5.0f, 10.0f, 5.0f);
	glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "pointLight.specular"), 5.0f, 5.0f, 5.0f);

	glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "pointLight.constant"), 1.0f);
	glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "pointLight.linear"), 0.09f);
	glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "pointLight.quadratic"), 0.032f);

	glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "switchOnOf"), switchOnOf);
	glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "switchPoint"), switchPoint);

	myModel.Draw(myCustomShader);

//////////////////////

//////////////////////
	model = glm::mat4(1.0f);

	//send matrix data to vertex shader
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	myModel1.Draw(myCustomShader);

//////////////////////
	model = glm::mat4(1.0f);

	//send matrix data to vertex shader
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	myModel2.Draw(myCustomShader);

//////////////////////
	model = glm::mat4(1.0f);

	//send matrix data to vertex shader
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	myModel3.Draw(myCustomShader);

///////////////////////////

//////////////////////
	model = glm::mat4(1.0f);

	//send matrix data to vertex shader
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	myModel4.Draw(myCustomShader);

	///////////////////////////

	model = glm::mat4(1.0f);

	//send matrix data to vertex shader
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	myModel5.Draw(myCustomShader);

	/////////////////
	model = glm::mat4(1.0f);

	// get current time
	double currentTimeStamp = glfwGetTime();
	updateDelta(currentTimeStamp - lastTimeStamp);
	lastTimeStamp = currentTimeStamp;
	model = glm::translate(model, glm::vec3(0, 0, delta));

	//send matrix data to vertex shader
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	myModel6.Draw(myCustomShader);

	/////////////////////////////

	model = glm::mat4(1.0f);

	viewLoc = glGetUniformLocation(myCustomShader.shaderProgram, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	//send matrix data to vertex shader
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "diffuseTexture"), 0);


	myModel7.Draw(myCustomShader);

	/////////////////////////////

	model = glm::mat4(1.0f);

	viewLoc = glGetUniformLocation(myCustomShader.shaderProgram, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));


	// get current time
	currentTimeStamp = glfwGetTime();
	updateDelta(currentTimeStamp - lastTimeStamp);
	lastTimeStamp = currentTimeStamp;
	model = glm::translate(model, glm::vec3(0, 0, delta));

	//send matrix data to vertex shader
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "diffuseTexture"), 0);


	myModel8.Draw(myCustomShader);
}

void loadTriangleData() {

	glGenVertexArrays(1, &objectVAO);

	glBindVertexArray(objectVAO);

	glGenBuffers(1, &verticesVBO);
	glBindBuffer(GL_ARRAY_BUFFER, verticesVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);

	glGenBuffers(1, &verticesEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, verticesEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vertexIndices), vertexIndices, GL_STATIC_DRAW);

	//vertex position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	//vertex texture
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);
}

int main(int argc, const char * argv[]) {

	initOpenGLWindow();

	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glCullFace(GL_BACK); // cull back face
	glFrontFace(GL_CCW); // GL_CCW for counter clock-wise

	myCustomShader.loadShader("shaders/shaderStart.vert", "shaders/shaderStart.frag");
	myCustomShader.useShaderProgram();

	//initialize the projection matrix
	glm::mat4 projection = glm::perspective(45.0f, (float)glWindowWidth / (float)glWindowHeight, 0.1f, 1000.0f);

	//send matrix data to shader
	GLint projLoc = glGetUniformLocation(myCustomShader.shaderProgram, "projection");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

	
	myModel = gps::Model3D("obj\\terrain.obj");
	myModel1 = gps::Model3D("obj\\skye.obj");
	myModel2 = gps::Model3D("obj\\stormtrooper.obj");
	myModel3 = gps::Model3D("obj\\darthVader.obj");
	myModel4 = gps::Model3D("obj\\deadBodies.obj");
	myModel5 = gps::Model3D("obj\\camila.obj");
	myModel6 = gps::Model3D("obj\\transport.obj");
	myModel7 = gps::Model3D("obj\\tank.obj");
	myModel8 = gps::Model3D("obj\\battleShip.obj");

	while (!glfwWindowShouldClose(glWindow)) {
		renderScene();

		glfwPollEvents();
		glfwSwapBuffers(glWindow);
	}

	//close GL context and any other GLFW resources
	glfwTerminate();

	return 0;
}
