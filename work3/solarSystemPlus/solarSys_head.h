// @time   : 2022.11.11
// @func   : ̫��ϵͷ�ļ�
// @author : gonzalez 

#ifndef SOLARSYS_HEAD
#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cstring>
#include <iostream>
#include <math.h>
#include <vector>
#include <math.h>
#include <string.h>
#include <unordered_map>

#include "..\\includes\glm\glm\glm.hpp"
#include "..\\includes\glm\glm\gtc\matrix_transform.hpp"
#include "..\\includes\glm\glm\gtc\type_ptr.hpp"
#include "..\\includes\headers\stb_image.h"
#include "..\\includes\headers\shader_m.h"
#include "..\\includes\headers\camera.h"

/**************************** �ṹ��/�궨������ ****************************/
struct planetInfo {
	std::string name;		 // ��������
	glm::vec3 site;			 // ��ʼλ�á�x y[0] z��
	glm::vec3 color;		 // ��ɫ��RGB��
	std::string momPlanet;	 // ����ĸ����
	float size;				 // ���ǳߴ�
	float speed;			 // �����ٶ�
	float radius;			 // ���ư뾶
	glm::vec3 rotationAngle; // ��ת��Ƕ�
	float rotationSpeed;     // ��ת�ٶ�
	unsigned int diffuseMap; // ����ͼƬ
};
#define PLANET_NUM 10

/**************************** ȫ�ֱ������� ****************************/
extern const std::string texturePath;
extern const unsigned int solarSystem_SCREEN_WIDTH;
extern const unsigned int solarSystem_SCREEN_HEIGHT;
extern const GLfloat solarSystem_PI;
extern const int solarSystem_Y_SEGMENTS;						
extern const int solarSystem_X_SEGMENTS;						
extern const int solarSystem_Y_SEGMENTS_CURVE;                 
extern const int solarSystem_X_SEGMENTS_CURVE;                 
extern Camera solarSystem_camera;
extern float solarSystem_lastX;
extern float solarSystem_lastY;
extern bool solarSystem_firstMouse;
extern float solarSystem_deltaTime;
extern float solarSystem_lastFrame;
extern planetInfo solarPlanets[PLANET_NUM];
extern std::unordered_map<std::string, int> solarPlanetsMap;
extern const float solarConstant;
extern const float solarLinear;
extern const float solarQuadratic;
extern const glm::vec3 solarLightColor;
extern const glm::vec3 dirLightDirection;

/**************************** �������� ****************************/
void planets_ini(void);
unsigned int solarSystem_loadCubemap(std::vector<std::string> faces);
unsigned int solarSystem_loadTexture(char const* path);
void solarRotateUpdate(std::string name);
void solarSystem_scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void solarSystem_mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
void solarSystem_framebuffer_size_callback(GLFWwindow* window, int width, int height);
void solarSystem_processInput(GLFWwindow* window);

#endif // !SOLARSYS_HEAD