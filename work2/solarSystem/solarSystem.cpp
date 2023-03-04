// @time   : 2022.10.27
// @func   : 绘制太阳系
// @notice : 

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "..\\includes\headers\stb_image.h"
#include "..\\includes\glm\glm\glm.hpp"
#include "..\\includes\glm\glm\gtc\matrix_transform.hpp"
#include "..\\includes\glm\glm\gtc\type_ptr.hpp"
#include "..\\includes\headers\shader_m.h"
#include "..\\includes\headers\camera.h"
#include <cstring>
#include <iostream>
#include <math.h>
#include <vector>
#include <math.h>
#include <string.h>
#include <unordered_map>


/************************* 数据定义 *****************************/
struct planetInfo {
	std::string name;       // 行星名字
	glm::vec3 site;         // 初始位置【x y[0] z】
	glm::vec3 color;        // 颜色【RGB】
	std::string momPlanet;  // 环绕母星名
	float size;             // 行星尺寸
	float speed;            // 环绕速度
	float radius;           // 环绕半径
};
#define PLANET_NUM 10

const unsigned int solarSys_SCREEN_WIDTH = 780;
const unsigned int solarSys_SCREEN_HEIGHT = 780;
const GLfloat solarSys_PI = 3.14159265358979323846f;

const int solarSys_Y_SEGMENTS = 100;                      // 球划分块数
const int solarSys_X_SEGMENTS = 100;                      // 球划分块数

const int solarSys_Y_SEGMENTS_CURVE = 1000;                      // 球划分块数
const int solarSys_X_SEGMENTS_CURVE = 1000;                      // 球划分块数

Camera solarSys_camera(glm::vec3(20.0, 20.0, 3.0f));
float solarSys_lastX = solarSys_SCREEN_WIDTH / 0.1f;
float solarSys_lastY = solarSys_SCREEN_HEIGHT / 0.1f;
bool solarSys_firstMouse = true;

float solarSys_deltaTime = 0.0f;
float solarSys_lastFrame = 0.0f;

planetInfo planets[PLANET_NUM];                            // 存储行星基本信息
std::unordered_map<std::string, int> planetsMap;           // 存储名字与索引之间的关系

const float constant = 1.0f;
const float linear = 0.045f;
const float quadratic = 0.0075f;
const glm::vec3 lightColor = glm::vec3(20.0, 20.0, 3.0f);


/************************* 函数定义 *****************************/
// @func : 按键处理函数
void solarSys_processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		solarSys_camera.ProcessKeyboard(FORWARD, solarSys_deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		solarSys_camera.ProcessKeyboard(BACKWARD, solarSys_deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		solarSys_camera.ProcessKeyboard(LEFT, solarSys_deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		solarSys_camera.ProcessKeyboard(RIGHT, solarSys_deltaTime);
}
// @func : 界面改变回调函数
void solarSys_framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}
// @func : 鼠标移动回调
void solarSys_mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);
	if (solarSys_firstMouse)	{
		solarSys_lastX = xpos;
		solarSys_lastY = ypos;
		solarSys_firstMouse = false;
	}
	float xoffset = xpos - solarSys_lastX;
	float yoffset = solarSys_lastY - ypos; 
	solarSys_lastX = xpos;
	solarSys_lastY = ypos;
	solarSys_camera.ProcessMouseMovement(xoffset, yoffset);
}
// @func : 鼠标滚轮回调
void solarSys_scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	solarSys_camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
// @func : 初始化行星信息
void planet_ini(void)
{
	// sun
	planets[0].name = "sun";
	planets[0].site = glm::vec3(0.0f, 0.0f, 0.0f);     // 位置
	planets[0].color = glm::vec3(0.85f, 0.0f, 0.0f);   // 颜色
	planets[0].momPlanet = "NULL";
	planets[0].size = 48.74f;
	planets[0].speed = 0.0f;
	planets[0].radius = 0.0f;
	planetsMap.insert(std::make_pair("sun", 0));
	// mercury
	planets[1].name = "mercury";
	planets[1].site = glm::vec3(62.06f, 0.0f, 0.0f);    // 位置
	planets[1].color = glm::vec3(.36, .13, 0.07);       // 颜色
	planets[1].momPlanet = "sun";
	planets[1].size = 7.32f;
	planets[1].speed = 0.4f;
	planets[1].radius = 62.06f;
	planetsMap.insert(std::make_pair("mercury", 1));
	// venus
	planets[2].name = "venus";
	planets[2].site = glm::vec3(115.56f, 0.0f, 0.0f);   // 位置
	planets[2].color = glm::vec3(0.4, 0.35, 0);         // 颜色
	planets[2].momPlanet = "sun";
	planets[2].size = 18.15f;
	planets[2].speed = 0.15f;
	planets[2].radius = 115.56f;
	planetsMap.insert(std::make_pair("venus", 2));
	// earth
	planets[3].name = "earth";
	planets[3].site = glm::vec3(168.00f, 0.0f, 0.0f);   // 位置
	planets[3].color = glm::vec3(0.25, 0.88, 0.81);     // 颜色
	planets[3].momPlanet = "sun";
	planets[3].size = 19.13f;
	planets[3].speed = 0.1f;
	planets[3].radius = 168.00f;
	planetsMap.insert(std::make_pair("earth", 3));
	// mars
	planets[4].name = "mars";
	planets[4].site = glm::vec3(228.00f, 0.0f, 0.0f);   // 位置
	planets[4].color = glm::vec3(1, .5, .5);            // 颜色
	planets[4].momPlanet = "sun";
	planets[4].size = 10.19f;
	planets[4].speed = 0.08f;
	planets[4].radius = 228.00f;
	planetsMap.insert(std::make_pair("mars", 4));
	// jupiter
	planets[5].name = "jupiter";
	planets[5].site = glm::vec3(333.40f, 0.0f, 0.0f);   // 位置
	planets[5].color = glm::vec3(1, 1, .5);             // 颜色
	planets[5].momPlanet = "sun";
	planets[5].size = 42.90f;
	planets[5].speed = 0.07f;
	planets[5].radius = 333.40f;
	planetsMap.insert(std::make_pair("jupiter", 5));
	// saturn
	planets[6].name = "saturn";
	planets[6].site = glm::vec3(428.10f, 0.0f, 0.0f);   // 位置
	planets[6].color = glm::vec3(.52, 0.2, .98);        // 颜色
	planets[6].momPlanet = "sun";
	planets[6].size = 36.16f;
	planets[6].speed = 0.06f;
	planets[6].radius = 428.10f;
	planetsMap.insert(std::make_pair("saturn", 6));
	// uranus
	planets[7].name = "uranus";
	planets[7].site = glm::vec3(520.00f, 0.0f, 0.0f);   // 位置
	planets[7].color = glm::vec3(.4, .4, .4);           // 颜色
	planets[7].momPlanet = "sun";
	planets[7].size = 25.56f;
	planets[7].speed = 0.04f;
	planets[7].radius = 520.00f;
	planetsMap.insert(std::make_pair("uranus", 7));
	// neptune
	planets[8].name = "neptune";
	planets[8].site = glm::vec3(575.10f, 0.0f, 0.0f);   // 位置
	planets[8].color = glm::vec3(1, 0, 1);            // 颜色
	planets[8].momPlanet = "sun";
	planets[8].size = 24.78f;
	planets[8].speed = 0.03f;
	planets[8].radius = 575.10f;
	planetsMap.insert(std::make_pair("neptune", 8));
	// moon
	planets[9].name = "moon";
	planets[9].site = glm::vec3(168.00f+26.01f, 0.0f, 0.0f);      // 位置
	planets[9].color = glm::vec3(0.03, 0.17, 0.32);               // 颜色
	planets[9].momPlanet = "earth";
	planets[9].size = 5.15f;
	planets[9].speed = 0.9f;
	planets[9].radius = 26.01f;
	planetsMap.insert(std::make_pair("moon", 9));

	// 缩小十倍
	for (int i = 0; i < PLANET_NUM; i++) {
		planets[i].radius = planets[i].radius / 10;
		planets[i].site[0] = planets[i].site[0] / 10;
		planets[i].size = planets[i].size / 10;
	}

}
// @func : 转动
void rotateUpdate(std::string name)
{
	// 准备阶段
	float time = (float)glfwGetTime();
	float angle = fmod((time * planets[planetsMap.find(name)->second].speed), (2.0f * solarSys_PI));
	std::string momName = planets[planetsMap.find(name)->second].momPlanet;
	glm::vec3 pole = planets[planetsMap.find(momName)->second].site;
	// 更新
	planets[planetsMap.find(name)->second].site.x = pole.x + planets[planetsMap.find(name)->second].radius * cos(angle);
	planets[planetsMap.find(name)->second].site.y = 0.0f;
	planets[planetsMap.find(name)->second].site.z = pole.z + planets[planetsMap.find(name)->second].radius * sin(angle);
	return ;
}


/************************* 主函数 *****************************/
int main()
{
	//------------------- PART 1 初始化 -------------------
	
	//初始化GLFW
	glfwInit();                                                     // 初始化GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);                  // opengl版本号3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);                  // 次版本号3
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 使用核心模式(无序向后兼容性)
	// 使用mac系统
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	//创建窗口（宽、高、窗口名称）
	GLFWwindow* window = glfwCreateWindow(solarSys_SCREEN_WIDTH, solarSys_SCREEN_HEIGHT, "solar system", nullptr, nullptr);
	//检测窗口是否创建成功
	if (window == nullptr)	{
		std::cout << "Failed to Create OpenGL Context" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);                                              //将窗口的上下文设置为当前进程的主上下文
	glfwSetFramebufferSizeCallback(window, solarSys_framebuffer_size_callback);  //窗口大小回调
	glfwSetCursorPosCallback(window, solarSys_mouse_callback);                   //鼠标回调
	glfwSetScrollCallback(window, solarSys_scroll_callback);                     //滚轮回调
	// GLFW捕捉mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	//初始化GLAD，加载OpenGL指针地址的函数
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	//指定当前视口尺寸（前两个参数为左下角位置，后两个参数是渲染窗口宽、高）
	glViewport(0, 0, solarSys_SCREEN_WIDTH, solarSys_SCREEN_HEIGHT);
	glEnable(GL_DEPTH_TEST);

	//------------------- PART 2 球初始化 -------------------
	planet_ini();                        // 球信息初始化
	std::vector<float> sphereVertices;   // 顶点
	std::vector<int> sphereIndices;      // 绘制顺序

	std::vector<float> curveVertices;    // 曲线
	std::vector<int> curveIndices;       // 绘制顺序


	// ========== 球 ==========
	// 生成球的顶点和法向量
	for (int y = 0; y <= solarSys_Y_SEGMENTS; y++){
		for (int x = 0; x <= solarSys_X_SEGMENTS; x++){
			float xSegment = (float)x / (float)(solarSys_X_SEGMENTS);
			float ySegment = (float)y / (float)(solarSys_Y_SEGMENTS);

			float xPos = std::cos(xSegment * 2.0f * solarSys_PI) * std::sin(ySegment * solarSys_PI);
			float yPos = std::cos(ySegment * solarSys_PI);
			float zPos = std::sin(xSegment * 2.0f * solarSys_PI) * std::sin(ySegment * solarSys_PI);

			// 顶点坐标
			sphereVertices.push_back(xPos);
			sphereVertices.push_back(yPos);
			sphereVertices.push_back(zPos);

			// 法向量
			sphereVertices.push_back(xPos);
			sphereVertices.push_back(yPos);
			sphereVertices.push_back(zPos);

		}
	}
	// 生成球的Indices
	for (int i = 0; i < solarSys_Y_SEGMENTS; i++){
		for (int j = 0; j < solarSys_X_SEGMENTS; j++){
			sphereIndices.push_back(i * (solarSys_X_SEGMENTS + 1) + j);
			sphereIndices.push_back((i + 1) * (solarSys_X_SEGMENTS + 1) + j);
			sphereIndices.push_back((i + 1) * (solarSys_X_SEGMENTS + 1) + j + 1);

			sphereIndices.push_back(i * (solarSys_X_SEGMENTS + 1) + j);
			sphereIndices.push_back((i + 1) * (solarSys_X_SEGMENTS + 1) + j + 1);
			sphereIndices.push_back(i * (solarSys_X_SEGMENTS + 1) + j + 1);
		}
	}
	// 数据绑定
	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	// 生成并绑定球体的VAO和VBO
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// 将顶点数据绑定至当前默认的缓冲中
	glBufferData(GL_ARRAY_BUFFER, sphereVertices.size() * sizeof(float), &sphereVertices[0], GL_STATIC_DRAW);
	// 顶点vec
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// 法向量vec
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// EBO数据绑定
	GLuint element_buffer_object;
	glGenBuffers(1, &element_buffer_object);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_object);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphereIndices.size() * sizeof(int), &sphereIndices[0], GL_STATIC_DRAW);
	// 解绑VAO和VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// ========== 曲线 ==========
	// 生成曲线的顶点和法向量
	for (int y = solarSys_Y_SEGMENTS_CURVE / 2; y <= solarSys_Y_SEGMENTS_CURVE / 2 + 1; y++) {
		for (int x = 0; x <= solarSys_X_SEGMENTS_CURVE; x++) {
			float xSegment = (float)x / (float)(solarSys_X_SEGMENTS_CURVE);
			float ySegment = (float)y / (float)(solarSys_Y_SEGMENTS_CURVE);

			float xPos = std::cos(xSegment * 2.0f * solarSys_PI) * std::sin(ySegment * solarSys_PI);
			float yPos = std::cos(ySegment * solarSys_PI);
			float zPos = std::sin(xSegment * 2.0f * solarSys_PI) * std::sin(ySegment * solarSys_PI);

			// 顶点坐标
			curveVertices.push_back(xPos);
			curveVertices.push_back(yPos);
			curveVertices.push_back(zPos);

			// 法向量
			curveVertices.push_back(xPos);
			curveVertices.push_back(yPos);
			curveVertices.push_back(zPos);

			// 

		}
	}
	// 生成曲线的Indices
	for (int i = 0; i < 1; i++) {
		for (int j = 0; j < solarSys_X_SEGMENTS_CURVE; j++) {
			curveIndices.push_back(i * (solarSys_X_SEGMENTS_CURVE + 1) + j);
			curveIndices.push_back((i + 1) * (solarSys_X_SEGMENTS_CURVE + 1) + j);
			curveIndices.push_back((i + 1) * (solarSys_X_SEGMENTS_CURVE + 1) + j + 1);

			curveIndices.push_back(i * (solarSys_X_SEGMENTS_CURVE + 1) + j);
			curveIndices.push_back((i + 1) * (solarSys_X_SEGMENTS_CURVE + 1) + j + 1);
			curveIndices.push_back(i * (solarSys_X_SEGMENTS_CURVE + 1) + j + 1);
		}
	}
	// 数据绑定
	unsigned int curveVBO, curveVAO;
	glGenVertexArrays(1, &curveVAO);
	glGenBuffers(1, &curveVBO);
	// 生成并绑定球体的VAO和VBO
	glBindVertexArray(curveVAO);
	glBindBuffer(GL_ARRAY_BUFFER, curveVBO);
	// 将顶点数据绑定至当前默认的缓冲中
	glBufferData(GL_ARRAY_BUFFER, curveVertices.size() * sizeof(float), &curveVertices[0], GL_STATIC_DRAW);
	// 顶点vec
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// 法向量vec
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// EBO数据绑定
	GLuint curve_element_buffer_object;
	glGenBuffers(1, &curve_element_buffer_object);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, curve_element_buffer_object);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, curveIndices.size() * sizeof(int), &curveIndices[0], GL_STATIC_DRAW);
	// 解绑VAO和VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//着色器
	Shader PLANET_shader("shader\\PLANET.vs", "shader\\PLANET.fs");
	Shader SUN_shader("shader\\SUN.vs", "shader\\SUN.fs");
	
	//------------------- PART 3 渲染 -------------------

	while (!glfwWindowShouldClose(window)) {
		// timing
		float currentFrame = static_cast<float>(glfwGetTime());
		solarSys_deltaTime = currentFrame - solarSys_lastFrame;
		solarSys_lastFrame = currentFrame;
		// 检测输入
		solarSys_processInput(window);
		// 清空颜色缓冲
		glClearColor(0.0f, 0.0f, 0.12f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// 数据定义
		glm::mat4 projection, view, model;
		glm::vec3 diffuseColor, ambientColor;


		// ============================ 绘制太阳 ============================
		SUN_shader.use();
		// view and projection矩阵
		projection = glm::perspective(glm::radians(solarSys_camera.Zoom), (float)solarSys_SCREEN_WIDTH / (float)solarSys_SCREEN_HEIGHT, 0.1f, 100.0f);
		view = solarSys_camera.GetViewMatrix();
		SUN_shader.setMat4("projection", projection);
		SUN_shader.setMat4("view", view);
		// model矩阵[行星中心位置坐标]
		SUN_shader.setFloat("size", planets[planetsMap.find("sun")->second].size);
		model = glm::mat4(1.0f);
		model = glm::translate(model, planets[planetsMap.find("sun")->second].site);
		SUN_shader.setMat4("model", model);
		SUN_shader.setVec3("color", planets[planetsMap.find("sun")->second].color);
		//进行绘制
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, solarSys_X_SEGMENTS * solarSys_Y_SEGMENTS * 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);


		// ============================ 绘制行星 ============================
		// 初始化
		PLANET_shader.use();
		PLANET_shader.setVec3("light.position", planets[planetsMap.find("sun")->second].site);
		PLANET_shader.setVec3("viewPos", solarSys_camera.Position);
		// 光线参数
		diffuseColor = lightColor * glm::vec3(0.5f);    // decrease the influence
		ambientColor = diffuseColor * glm::vec3(0.2f); // low influence
		PLANET_shader.setVec3("light.ambient", ambientColor);
		PLANET_shader.setVec3("light.diffuse", diffuseColor);
		PLANET_shader.setVec3("light.specular", 0.5f, 0.5f, 0.5f);
		// 设置衰减
		PLANET_shader.setFloat("light.constant", constant);
		PLANET_shader.setFloat("light.linear", linear);
		PLANET_shader.setFloat("light.quadratic", quadratic);
		// 绘制行星
		projection = glm::perspective(glm::radians(solarSys_camera.Zoom), (float)solarSys_SCREEN_WIDTH / (float)solarSys_SCREEN_HEIGHT, 0.1f, 100.0f);
		view = solarSys_camera.GetViewMatrix();
		PLANET_shader.setMat4("projection", projection);
		PLANET_shader.setMat4("view", view);
		glm::vec3 specular = glm::vec3(0.5f, 0.5f, 0.5f);
		float shininess = 32.0f;
		for (int i = 1; i < PLANET_NUM; i++) {
			std::string Pname = planets[i].name;

			// 材质参数
			PLANET_shader.setVec3("material.ambient", planets[planetsMap.find(Pname)->second].color);
			PLANET_shader.setVec3("material.diffuse", planets[planetsMap.find(Pname)->second].color);
			PLANET_shader.setVec3("material.specular",specular);
			PLANET_shader.setFloat("material.shininess", shininess);

			// model矩阵[行星中心位置坐标]
			PLANET_shader.setFloat("size", planets[planetsMap.find(Pname)->second].size);
			rotateUpdate(Pname);                                                          //更新坐标
			model = glm::mat4(1.0f);
			model = glm::translate(model, planets[planetsMap.find(Pname)->second].site);
			PLANET_shader.setMat4("model", model);

			// 进行绘制
			glBindVertexArray(VAO);
			glDrawElements(GL_TRIANGLES, solarSys_X_SEGMENTS * solarSys_Y_SEGMENTS * 6, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);

		}

		// ============================ 绘制轨道 ============================
		SUN_shader.use();
		// 设置view和projection矩阵
		projection = glm::perspective(glm::radians(solarSys_camera.Zoom), (float)solarSys_SCREEN_WIDTH / (float)solarSys_SCREEN_HEIGHT, 0.1f, 100.0f);
		view = solarSys_camera.GetViewMatrix();
		SUN_shader.setMat4("projection", projection);
		SUN_shader.setMat4("view", view);
		// 行星绘制
		for (int i = 1; i < PLANET_NUM; i++) {
			std::string name = planets[i].name;
			std::string momName = planets[planetsMap.find(name)->second].momPlanet;

			// model矩阵[行星中心位置坐标]
			SUN_shader.setFloat("size", planets[planetsMap.find(name)->second].radius);
			model = glm::mat4(1.0f);
			model = glm::translate(model, planets[planetsMap.find(momName)->second].site);
			SUN_shader.setMat4("model", model);
			SUN_shader.setVec3("color", planets[planetsMap.find(name)->second].color);
			//进行绘制
			glBindVertexArray(curveVAO);
			glDrawElements(GL_TRIANGLES, solarSys_X_SEGMENTS_CURVE * 2 * 6, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		}

		//交换缓冲并且检查是否有触发事件(比如键盘输入、鼠标移动)
		glfwSwapBuffers(window);
		glfwPollEvents();

	}


	//------------------- PART 4 释放 -------------------
	
	//释放VAO和VBO，EBO
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &element_buffer_object);
	//清理所有的资源并正确出程序
	glfwTerminate();
	return 0;
}
