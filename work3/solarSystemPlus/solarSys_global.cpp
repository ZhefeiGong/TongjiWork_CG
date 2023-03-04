// @time   : 2022.11.11
// @func   : 太阳系全局变量
// @author : gonzalez 

#include "solarSys_head.h"

const std::string texturePath = "..\\resources\\texture\\";         // 纹理所在位置
const unsigned int solarSystem_SCREEN_WIDTH = 1387;
const unsigned int solarSystem_SCREEN_HEIGHT = 780;
const GLfloat solarSystem_PI = 3.14159265358979323846f;
const int solarSystem_Y_SEGMENTS = 100;							// 球划分块数
const int solarSystem_X_SEGMENTS = 100;							// 球划分块数
const int solarSystem_Y_SEGMENTS_CURVE = 1000;                  // 球划分块数
const int solarSystem_X_SEGMENTS_CURVE = 1000;                  // 球划分块数
Camera solarSystem_camera(glm::vec3(20.0, 20.0, 3.0f));
float solarSystem_lastX = solarSystem_SCREEN_WIDTH / 0.1f;
float solarSystem_lastY = solarSystem_SCREEN_HEIGHT / 0.1f;
bool solarSystem_firstMouse = true;
float solarSystem_deltaTime = 0.0f;
float solarSystem_lastFrame = 0.0f;
planetInfo solarPlanets[PLANET_NUM];                            // 存储行星基本信息
std::unordered_map<std::string, int> solarPlanetsMap;           // 存储名字与索引之间的关系
const float solarConstant = 1.0f;
const float solarLinear = 0.045f;
const float solarQuadratic = 0.0075f;
const glm::vec3 solarLightColor = glm::vec3(20.0, 20.0, 20.0);
const glm::vec3 dirLightDirection = glm::vec3(0.0f, 0.0f, 1.0f);