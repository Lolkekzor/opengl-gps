//
//  main.cpp
//  OpenGL Shadows
//
//  Created by CGIS on 05/12/16.
//  Copyright � 2016 CGIS. All rights reserved.
//

#define GLEW_STATIC
#define TINYOBJLOADER_IMPLEMENTATION

#include <iostream>
#include "glm/glm.hpp"//core glm functionality
#include "glm/gtc/matrix_transform.hpp"//glm extension for generating common transformation matrices
#include "glm/gtc/matrix_inverse.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include <string>
#include "Shader.hpp"
#include "Camera.hpp"
#include "SkyBox.hpp"
#include "Transform.hpp"
#include "WaterFrameBuffers.hpp"
#include "ShadowFrameBuffer.hpp"

#include "Model3D.hpp"
#include "Object.hpp"
#include "Mesh.hpp"

extern "C" { _declspec(dllexport) int NvOptimusEnablement = 0x00000001; }

int glWindowWidth = 1366;
int glWindowHeight = 768;
int retina_width, retina_height;
GLFWwindow* glWindow = NULL;

glm::mat4 model;
GLuint modelLoc;
glm::mat4 view;
GLuint viewLoc;
glm::mat4 projection;
GLuint projectionLoc;
glm::mat3 normalMatrix;
GLuint normalMatrixLoc;
glm::mat3 lightDirMatrix;
GLuint lightDirMatrixLoc;

glm::vec3 lightDir;
GLuint lightDirLoc;
glm::vec3 lightColor;
GLuint lightColorLoc;

gps::Camera myCamera(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f));

bool pressedKeys[1024];
GLfloat angle;
GLfloat lightAngle;

gps::Object* myModel;
gps::Object* plane;
gps::Object* lightCube;
gps::Object* mercury, * venus, * moon, * earth, * sun, * terrain, * island;

std::vector<gps::Object*> gameObjects;

gps::Shader myCustomShader;
gps::Shader lightShader;
gps::Shader depthMapShader;
gps::Shader waterShader;

gps::SkyBox mySkyBox;
gps::Shader skyboxShader;

GLuint shadowMapFBO;
GLuint depthMapTexture;

GLenum glCheckError_(const char* file, int line)
{
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR)
	{
		std::string error;
		switch (errorCode)
		{
		case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
		case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
		case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
		case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
		case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
		case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void windowResizeCallback(GLFWwindow* window, int width, int height)
{
	fprintf(stdout, "window resized to width: %d , and height: %d\n", width, height);
	//TODO
	//for RETINA display
	glfwGetFramebufferSize(glWindow, &retina_width, &retina_height);

	myCustomShader.useShaderProgram();

	//set projection matrix
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
	//send matrix data to shader
	GLint projLoc = glGetUniformLocation(myCustomShader.shaderProgram, "projection");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

	lightShader.useShaderProgram();

	glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	//set Viewport transform
	glViewport(0, 0, retina_width, retina_height);
}

bool toggleBufferDisplay = true;
void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS) {
			pressedKeys[key] = true;
			if (key == GLFW_KEY_R) {
				toggleBufferDisplay = !toggleBufferDisplay;
			}
			if (key == GLFW_KEY_P) {
				myCamera.switchMode();
			}
		}
		else if (action == GLFW_RELEASE)
			pressedKeys[key] = false;
	}
}

double last_xpos = 0, last_ypos = 0;
void mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	double delta_xpos = 0, delta_ypos = 0;
	if (last_xpos != 0 && last_ypos != 0) {
		delta_xpos = xpos - last_xpos;
		delta_ypos = ypos - last_ypos;
	}
	last_xpos = xpos;
	last_ypos = ypos;

	myCamera.rotate(delta_ypos, delta_xpos);
}

GLfloat cameraSpeed = 5.0f;
void processMovement(double deltaTime)
{

	if (pressedKeys[GLFW_KEY_Q]) {
		angle += 0.1f;
		if (angle > 360.0f)
			angle -= 360.0f;
	}

	if (pressedKeys[GLFW_KEY_E]) {
		angle -= 0.1f;
		if (angle < 0.0f)
			angle += 360.0f;
	}

	if (pressedKeys[GLFW_KEY_T]) {
		cameraSpeed += cameraSpeed * deltaTime;
	}

	if (pressedKeys[GLFW_KEY_Y]) {
		cameraSpeed -= cameraSpeed * deltaTime;
		if (cameraSpeed < 1.0f)
			cameraSpeed = 1.0f;
	}

	double moveStep = cameraSpeed * deltaTime;
	if (pressedKeys[GLFW_KEY_W]) {
		myCamera.move(gps::MOVE_FORWARD, moveStep);
	}

	if (pressedKeys[GLFW_KEY_S]) {
		myCamera.move(gps::MOVE_BACKWARD, moveStep);
	}

	if (pressedKeys[GLFW_KEY_SPACE]) {
		myCamera.move(gps::MOVE_UP, moveStep);
	}

	if (pressedKeys[GLFW_KEY_LEFT_SHIFT]) {
		myCamera.move(gps::MOVE_DOWN, moveStep);
	}

	if (pressedKeys[GLFW_KEY_A]) {
		myCamera.move(gps::MOVE_LEFT, moveStep);
	}

	if (pressedKeys[GLFW_KEY_D]) {
		myCamera.move(gps::MOVE_RIGHT, moveStep);
	}

	if (pressedKeys[GLFW_KEY_B]) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	}

	if (pressedKeys[GLFW_KEY_N]) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	if (pressedKeys[GLFW_KEY_M]) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	

	if (pressedKeys[GLFW_KEY_J]) {

		lightAngle += 0.3f;
		if (lightAngle > 360.0f)
			lightAngle -= 360.0f;
		glm::vec3 lightDirTr = glm::vec3(glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(lightDir, 1.0f));
		myCustomShader.useShaderProgram();
		glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDirTr));
	}

	if (pressedKeys[GLFW_KEY_L]) {
		lightAngle -= 0.3f;
		if (lightAngle < 0.0f)
			lightAngle += 360.0f;
		glm::vec3 lightDirTr = glm::vec3(glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(lightDir, 1.0f));
		myCustomShader.useShaderProgram();
		glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDirTr));
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

	glfwWindowHint(GLFW_SAMPLES, 4);

	glWindow = glfwCreateWindow(glWindowWidth, glWindowHeight, "OpenGL Shader Example", NULL, NULL);
	if (!glWindow) {
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
		return false;
	}

	glfwSetWindowSizeCallback(glWindow, windowResizeCallback);
	glfwMakeContextCurrent(glWindow);

	glfwSetInputMode(glWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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

	return true;
}

void initOpenGLState()
{
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glViewport(0, 0, retina_width, retina_height);
	glEnable(GL_FRAMEBUFFER_SRGB);
	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glEnable(GL_CULL_FACE); // cull face
	glCullFace(GL_BACK); // cull back face
	glFrontFace(GL_CCW); // GL_CCW for counter clock-wise
}

gps::WaterFrameBuffers* WFB;
gps::ShadowFrameBuffer* SFB;
void initFBOs()
{
	WFB = new gps::WaterFrameBuffers();
	SFB = new gps::ShadowFrameBuffer();
}

glm::mat4 computeLightView()
{
	const GLfloat near_plane = 1.0f, far_plane = 500.0f;
	glm::mat4 lightProjection = glm::ortho(-100.0f, 100.0f, -100.0f, 100.0f, near_plane, far_plane);

	glm::vec3 lightDirTr = glm::vec3(glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(lightDir, 1.0f));
	glm::mat4 lightView = glm::lookAt(100.0f * lightDirTr, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	return lightProjection * lightView;
}

void initModels()
{
	gameObjects.push_back(myModel = new gps::Object("objects/nanosuit/nanosuit.obj", "objects/nanosuit/"));
	gameObjects.push_back(moon = new gps::Object("objects/moon/moon.obj", "objects/moon/"));
	gameObjects.push_back(earth = new gps::Object("objects/earth/earth.obj", "objects/earth/"));
	gameObjects.push_back(venus = new gps::Object("objects/venus/venus.obj", "objects/venus/"));
	gameObjects.push_back(mercury = new gps::Object("objects/mercury/mercury.obj", "objects/mercury/"));
	gameObjects.push_back(sun = new gps::Object("objects/goodsun/sun.obj", "objects/goodsun/"));
	gameObjects.push_back(island = new gps::Object("objects/island/island.obj", "objects/island/"));

	lightCube = new gps::Object("objects/cube/cube.obj", "objects/cube/");
	plane = new gps::Object("objects/ground/ground.obj", "objects/ground/");
	terrain = new gps::Object("objects/lake/lake.obj", "objects/lake/");
}

void initShaders()
{
	myCustomShader.loadShader("shaders/shaderStart.vert", "shaders/shaderStart.frag");
	lightShader.loadShader("shaders/lightCube.vert", "shaders/lightCube.frag");
	depthMapShader.loadShader("shaders/simpleDepthMap.vert", "shaders/simpleDepthMap.frag");
	waterShader.loadShader("shaders/waterShader.vert", "shaders/waterShader.frag");
}

void initUniforms()
{
	myCustomShader.useShaderProgram();

	modelLoc = glGetUniformLocation(myCustomShader.shaderProgram, "model");

	viewLoc = glGetUniformLocation(myCustomShader.shaderProgram, "view");

	normalMatrixLoc = glGetUniformLocation(myCustomShader.shaderProgram, "normalMatrix");

	lightDirMatrixLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightDirMatrix");

	projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
	projectionLoc = glGetUniformLocation(myCustomShader.shaderProgram, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	//set the light direction (direction towards the light)
	lightDir = glm::normalize(glm::vec3(-3.7f, 9.0f, 14.0f));
	lightDirLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightDir");
	glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDir));

	//set light color
	lightColor = glm::vec3(1.0f, 0.6f, 0.2f); //white light
	lightColorLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightColor");
	glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

	lightShader.useShaderProgram();
	glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
}

void initSkybox() {
	std::vector<const GLchar*> faces;
	faces.push_back("textures/skybox/right.tga");
	faces.push_back("textures/skybox/left.tga");
	faces.push_back("textures/skybox/top.tga");
	faces.push_back("textures/skybox/bottom.tga");
	faces.push_back("textures/skybox/back.tga");
	faces.push_back("textures/skybox/front.tga");

	mySkyBox.Load(faces);
	skyboxShader.loadShader("shaders/skyboxShader.vert", "shaders/skyboxShader.frag");
	skyboxShader.useShaderProgram();

	view = myCamera.getViewMatrix();
	glUniformMatrix4fv(glGetUniformLocation(skyboxShader.shaderProgram, "view"), 1, GL_FALSE,
		glm::value_ptr(view));

	projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
	glUniformMatrix4fv(glGetUniformLocation(skyboxShader.shaderProgram, "projection"), 1, GL_FALSE,
		glm::value_ptr(projection));

}

double oldTimeSinceStart = 0;
double timestep = 0;
void updateObjects() {
	double timeSinceStart = glfwGetTime();
	double deltaTime = timeSinceStart - oldTimeSinceStart;
	oldTimeSinceStart = timeSinceStart;
	processMovement(deltaTime);

	timestep += deltaTime;

	myCamera.updateCinematic(deltaTime);

	/// ADD TRANSFORMS
	for (auto it : gameObjects) it->clearTransforms();
	
	sun->addTransform(gps::Transform(gps::ROTATE, glm::vec3(0.0f, 1.0f, 0.0f), timestep / 12));

	mercury->addTransform(gps::Transform(gps::ROTATE, glm::vec3(0.0f, 1.0f, 0.0f), timestep));
	mercury->addTransform(gps::Transform(gps::TRANSLATE, glm::vec3(10.0f, 0.0f, 0.0f)));

	venus->addTransform(gps::Transform(gps::ROTATE, glm::vec3(0.0f, 1.0f, 0.0f), timestep / 2));
	venus->addTransform(gps::Transform(gps::TRANSLATE, glm::vec3(20.0f, 0.0f, 0.0f)));

	earth->addTransform(gps::Transform(gps::ROTATE, glm::vec3(0.0f, 1.0f, 0.0f), timestep / 3));
	earth->addTransform(gps::Transform(gps::TRANSLATE, glm::vec3(30.0f, 0.0f, 0.0f)));
	earth->addTransform(gps::Transform(gps::ROTATE, glm::vec3(0.1f, 1.0f, 0.1f), timestep / 4));

	moon->addTransform(gps::Transform(gps::ROTATE, glm::vec3(0.0f, 1.0f, 0.0f), timestep / 3));
	moon->addTransform(gps::Transform(gps::TRANSLATE, glm::vec3(30.0f, 0.0f, 0.0f)));
	moon->addTransform(gps::Transform(gps::ROTATE, glm::vec3(0.0f, 1.0f, 0.0f), timestep / 2));
	moon->addTransform(gps::Transform(gps::TRANSLATE, glm::vec3(8.0f, 0.0f, 0.0f)));

	for (auto it : gameObjects) it->addTransform(gps::Transform(gps::TRANSLATE, glm::vec3(0.0f, 15.0f, 0.0f)));

	terrain->clearTransforms();
	terrain->addTransform(gps::Transform(gps::TRANSLATE, glm::vec3(0.0f, -20.0f, 0.0f)));
	terrain->addTransform(gps::Transform(gps::SCALE, glm::vec3(100.0f, 100.0f, 100.0f)));

	island->clearTransforms();
	island->addTransform(gps::Transform(gps::TRANSLATE, glm::vec3(0.0f, -10.0f, 0.0f)));
	island->addTransform(gps::Transform(gps::SCALE, glm::vec3(10.0f, 10.0f, 10.0f)));

	sun->addTransform(gps::Transform(gps::SCALE, glm::vec3(6.0f, 6.0f, 6.0f)));
}

void renderObjects(std::vector<gps::Object*> objects, gps::Shader shader)
{
	glViewport(0, 0, retina_width, retina_height);
	shader.useShaderProgram();

	//send lightSpace matrix to shader
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "lightView"),
		1,
		GL_FALSE,
		glm::value_ptr(computeLightView()));

	//send view matrix to shader
	view = myCamera.getViewMatrix();
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "view"),
		1,
		GL_FALSE,
		glm::value_ptr(view));

	//compute light direction transformation matrix
	lightDirMatrix = glm::mat3(glm::inverseTranspose(view));
	//send lightDir matrix data to shader
	glUniformMatrix3fv(lightDirMatrixLoc, 1, GL_FALSE, glm::value_ptr(lightDirMatrix));

	//bind the depth map
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, SFB->shadowTexture);
	glUniform1i(glGetUniformLocation(shader.shaderProgram, "shadowMap"), 3);

	

	for (auto it : gameObjects) {
		it->Draw(myCustomShader, view);
	}

}

void renderToShadowFBO(std::vector<gps::Object*> objects, gps::Shader shader)
{
	shader.useShaderProgram();
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "lightView"),
		1,
		GL_FALSE,
		glm::value_ptr(computeLightView()));

	glViewport(0, 0, 8192, 8192);
	glBindFramebuffer(GL_FRAMEBUFFER, SFB->shadowBuffer);
	glClear(GL_DEPTH_BUFFER_BIT);
	//glViewport(0, 0, retina_width, retina_height);

	for (auto it : objects) {
		it->Draw(shader, view);
	}
}

gps::Shader textureShader;
void renderToTexture() {
	//Draw to refraction
	myCustomShader.useShaderProgram();

	glBindFramebuffer(GL_FRAMEBUFFER, WFB->refractionBuffer);
	glEnable(GL_CLIP_DISTANCE0);
	glClear(GL_DEPTH_BUFFER_BIT);

	glm::vec4 plane(0, -1, 0, 0);
	glUniform4fv(glGetUniformLocation(myCustomShader.shaderProgram, "plane"), 1, glm::value_ptr(plane));

	renderObjects(gameObjects, myCustomShader);
	glDisable(GL_CLIP_DISTANCE0);

	skyboxShader.useShaderProgram();
	mySkyBox.Draw(skyboxShader, view, projection);

	// Draw to reflection
	myCustomShader.useShaderProgram();

	glBindFramebuffer(GL_FRAMEBUFFER, WFB->reflectionBuffer);
	glEnable(GL_CLIP_DISTANCE0);
	glClear(GL_DEPTH_BUFFER_BIT);

	glm::vec4 plane2(0, 1, 0, 1);
	glUniform4fv(glGetUniformLocation(myCustomShader.shaderProgram, "plane"), 1, glm::value_ptr(plane2));

	myCamera.move(gps::MOVE_DOWN, myCamera.getCameraPosition().y * 2);
	myCamera.flip();
	renderObjects(gameObjects, myCustomShader);
	myCamera.move(gps::MOVE_UP, myCamera.getCameraPosition().y * -2);
	myCamera.flip();
	glDisable(GL_CLIP_DISTANCE0);

	skyboxShader.useShaderProgram();
	mySkyBox.Draw(skyboxShader, view, projection);

}

gps::Model3D texLoader;
GLuint DuDvID, normalMap;
GLuint quad_waterID;
GLuint quad_watervertexbuffer;
void initWaterQuad() {
	glGenVertexArrays(1, &quad_waterID);
	glBindVertexArray(quad_waterID);

	static const GLfloat g_quad_water_vertex_buffer_data[] = {
		-1.0f,  0.0f, 1.0f,
		1.0f, 0.0f, -1.0f,
		-1.0f, 0.0f, -1.0f,

		1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, -1.0f,
		-1.0f,  0.0f, 1.0f,
	};

	glGenBuffers(1, &quad_watervertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, quad_watervertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_water_vertex_buffer_data), g_quad_water_vertex_buffer_data, GL_STATIC_DRAW);

	DuDvID = texLoader.ReadTextureFromFile("textures/waterDUDV.png");
	normalMap = texLoader.ReadTextureFromFile("textures/matchingNormalMap.png");
}

void renderWater() {
	waterShader.useShaderProgram();

	//set the light direction (direction towards the light)
	glUniform3fv(glGetUniformLocation(waterShader.shaderProgram, "lightDir"), 1, glm::value_ptr(lightDir));

	//set light color
	glUniform3fv(glGetUniformLocation(waterShader.shaderProgram, "lightColor"), 1, glm::value_ptr(lightColor));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, WFB->reflectionTexture);
	glUniform1i(glGetUniformLocation(waterShader.shaderProgram, "reflectionTexture"), 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, WFB->refractionTexture);
	glUniform1i(glGetUniformLocation(waterShader.shaderProgram, "refractionTexture"), 1);
	
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, DuDvID);
	glUniform1i(glGetUniformLocation(waterShader.shaderProgram, "dudvMap"), 2);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, normalMap);
	glUniform1i(glGetUniformLocation(waterShader.shaderProgram, "normalMap"), 3);

	glUniform1f(glGetUniformLocation(waterShader.shaderProgram, "moveFactor"), timestep);

	glm::vec3 camPos = myCamera.getCameraPosition();
	glUniform3fv(glGetUniformLocation(waterShader.shaderProgram, "cameraPos"), 1, glm::value_ptr(camPos));

	gps::Transform translate(gps::TRANSLATE, glm::vec3(0.0f, 0.0f, 5.0f));
	gps::Transform scale(gps::SCALE, glm::vec3(50.0f, 1.0f, 55.0f));
	glm::mat4 transform = translate.getMatrix() * scale.getMatrix();
	glUniformMatrix4fv(glGetUniformLocation(waterShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(transform));

	int projectionLoc = glGetUniformLocation(waterShader.shaderProgram, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	view = myCamera.getViewMatrix();
	int viewLoc = glGetUniformLocation(waterShader.shaderProgram, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	//glActiveTexture(GL_TEXTURE3);
	//glBindTexture(GL_TEXTURE_2D, WFB->refractionTexture);
	//glUniform1i(glGetUniformLocation(textureShader.shaderProgram, "renderedTexture"), 3);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glBindVertexArray(quad_waterID);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, quad_watervertexbuffer);
	glVertexAttribPointer(
		0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);
	// Draw the triangle !
	glDrawArrays(GL_TRIANGLES, 0, 6); // Starting from vertex 0; 3 vertices total -> 1 triangle
	glDisableVertexAttribArray(0);
}

void renderScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//render the scene to the depth buffer (first pass)
	renderToShadowFBO(gameObjects, depthMapShader);

	//render scene to refraction FBO
	renderToTexture();

	//render the scene (second pass)
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	renderObjects(gameObjects, myCustomShader);

	renderWater();
	
	skyboxShader.useShaderProgram();
	mySkyBox.Draw(skyboxShader, view, projection);
}

GLuint quad_VertexArrayID;
GLuint quad_vertexbuffer;
GLuint texID;
void initQuad() {
	
	glGenVertexArrays(1, &quad_VertexArrayID);
	glBindVertexArray(quad_VertexArrayID);

	static const GLfloat g_quad_vertex_buffer_data[] = {
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		1.0f,  1.0f, 0.0f,
	};
	
	glGenBuffers(1, &quad_vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data), g_quad_vertex_buffer_data, GL_STATIC_DRAW);

	textureShader.loadShader("shaders/passThrough.vert", "shaders/simpleTexture.frag");
	textureShader.useShaderProgram();
	
	texID = glGetUniformLocation(textureShader.shaderProgram, "renderedTexture");
}

void drawQuad() {
	textureShader.useShaderProgram();

	glm::mat4 GUItransform(1.0f);
	gps::Transform translate(gps::TRANSLATE, glm::vec3(-0.5f, 0.5f, 0.0f));
	gps::Transform scale(gps::SCALE, glm::vec3(0.25f, 0.25f, 1.0f));
	GUItransform = translate.getMatrix() * scale.getMatrix();
	glUniformMatrix4fv(glGetUniformLocation(textureShader.shaderProgram, "transform"), 1, GL_FALSE, glm::value_ptr(GUItransform));

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, WFB->refractionTexture);
	glUniform1i(glGetUniformLocation(textureShader.shaderProgram, "renderedTexture"), 3);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glBindVertexArray(quad_VertexArrayID);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
	glVertexAttribPointer(
		0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);
	// Draw the triangle !
	glDrawArrays(GL_TRIANGLES, 0, 6); // Starting from vertex 0; 3 vertices total -> 1 triangle
	glDisableVertexAttribArray(0);
}

void drawQuad2() {
	textureShader.useShaderProgram();

	glm::mat4 GUItransform(1.0f);
	gps::Transform translate(gps::TRANSLATE, glm::vec3(0.5f, 0.5f, 0.0f));
	gps::Transform scale(gps::SCALE, glm::vec3(0.25f, 0.25f, 1.0f));
	GUItransform = translate.getMatrix() * scale.getMatrix();
	glUniformMatrix4fv(glGetUniformLocation(textureShader.shaderProgram, "transform"), 1, GL_FALSE, glm::value_ptr(GUItransform));

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, WFB->reflectionTexture);
	glUniform1i(glGetUniformLocation(textureShader.shaderProgram, "renderedTexture"), 3);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glBindVertexArray(quad_VertexArrayID);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
	glVertexAttribPointer(
		0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);
	// Draw the triangle !
	glDrawArrays(GL_TRIANGLES, 0, 6); // Starting from vertex 0; 3 vertices total -> 1 triangle
	glDisableVertexAttribArray(0);
}

int main(int argc, const char* argv[]) {
	initOpenGLWindow();
	initOpenGLState();
	initFBOs();
	initModels();
	initSkybox();
	initShaders();
	initUniforms();

	/// TODOTODOTODOTODOTO ereroeroeroer
	std::vector<glm::vec3> points;
	const float scaled = 10.0f;
	
	/*
	points.push_back(glm::vec3(40.0f * scaled, 5.0f * scaled, 0.0f * scaled));
	points.push_back(glm::vec3(-20.0f * scaled, 4.0f * scaled, 60.0f * scaled));
	points.push_back(glm::vec3(-35.0f * scaled, 3.0f * scaled, -60.0f * scaled));
	points.push_back(glm::vec3(-3.5f * scaled, 2.0f * scaled, 0.0f * scaled));
	*/
	points.push_back(glm::vec3(-40.0f * scaled, -5.0f * scaled, -60.0f * scaled));
	points.push_back(glm::vec3(80.0f * scaled, 6.0f * scaled, 40.0f * scaled));
	//points.push_back(glm::vec3(-16.0f * scaled, 4.5f * scaled, 5.0f * scaled));
	//points.push_back(glm::vec3(-3.5f * scaled, 0.5f * scaled, 0.0f * scaled));
	points.push_back(glm::vec3(-26.0f * scaled, 4.5f * scaled, 0.0f * scaled));
	points.push_back(glm::vec3(2.5f * scaled, 0.5f * scaled, -3.5f * scaled));
	myCamera.setCinematic(points, glm::vec3(0.0f));

	//TESTING
	initQuad();
	initWaterQuad();
	//TESTING

	glCheckError();
	while (!glfwWindowShouldClose(glWindow)) {
		updateObjects();
		renderScene();
		
		if (toggleBufferDisplay) {
			drawQuad();
			drawQuad2();
		}
		

		glfwPollEvents();
		glfwSwapBuffers(glWindow);
	}

	//close GL context and any other GLFW resources
	glfwTerminate();

	return 0;
}
