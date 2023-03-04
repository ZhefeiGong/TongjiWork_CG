// @time   : 2022.10.27
// @func   : ����̫��ϵ
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


/************************* ���ݶ��� *****************************/
struct planetInfo {
	std::string name;       // ��������
	glm::vec3 site;         // ��ʼλ�á�x y[0] z��
	glm::vec3 color;        // ��ɫ��RGB��
	std::string momPlanet;  // ����ĸ����
	float size;             // ���ǳߴ�
	float speed;            // �����ٶ�
	float radius;           // ���ư뾶
};
#define PLANET_NUM 10

const unsigned int solarSys_SCREEN_WIDTH = 780;
const unsigned int solarSys_SCREEN_HEIGHT = 780;
const GLfloat solarSys_PI = 3.14159265358979323846f;

const int solarSys_Y_SEGMENTS = 100;                      // �򻮷ֿ���
const int solarSys_X_SEGMENTS = 100;                      // �򻮷ֿ���

const int solarSys_Y_SEGMENTS_CURVE = 1000;                      // �򻮷ֿ���
const int solarSys_X_SEGMENTS_CURVE = 1000;                      // �򻮷ֿ���

Camera solarSys_camera(glm::vec3(20.0, 20.0, 3.0f));
float solarSys_lastX = solarSys_SCREEN_WIDTH / 0.1f;
float solarSys_lastY = solarSys_SCREEN_HEIGHT / 0.1f;
bool solarSys_firstMouse = true;

float solarSys_deltaTime = 0.0f;
float solarSys_lastFrame = 0.0f;

planetInfo planets[PLANET_NUM];                            // �洢���ǻ�����Ϣ
std::unordered_map<std::string, int> planetsMap;           // �洢����������֮��Ĺ�ϵ

const float constant = 1.0f;
const float linear = 0.045f;
const float quadratic = 0.0075f;
const glm::vec3 lightColor = glm::vec3(20.0, 20.0, 3.0f);


/************************* �������� *****************************/
// @func : ����������
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
// @func : ����ı�ص�����
void solarSys_framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}
// @func : ����ƶ��ص�
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
// @func : �����ֻص�
void solarSys_scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	solarSys_camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
// @func : ��ʼ��������Ϣ
void planet_ini(void)
{
	// sun
	planets[0].name = "sun";
	planets[0].site = glm::vec3(0.0f, 0.0f, 0.0f);     // λ��
	planets[0].color = glm::vec3(0.85f, 0.0f, 0.0f);   // ��ɫ
	planets[0].momPlanet = "NULL";
	planets[0].size = 48.74f;
	planets[0].speed = 0.0f;
	planets[0].radius = 0.0f;
	planetsMap.insert(std::make_pair("sun", 0));
	// mercury
	planets[1].name = "mercury";
	planets[1].site = glm::vec3(62.06f, 0.0f, 0.0f);    // λ��
	planets[1].color = glm::vec3(.36, .13, 0.07);       // ��ɫ
	planets[1].momPlanet = "sun";
	planets[1].size = 7.32f;
	planets[1].speed = 0.4f;
	planets[1].radius = 62.06f;
	planetsMap.insert(std::make_pair("mercury", 1));
	// venus
	planets[2].name = "venus";
	planets[2].site = glm::vec3(115.56f, 0.0f, 0.0f);   // λ��
	planets[2].color = glm::vec3(0.4, 0.35, 0);         // ��ɫ
	planets[2].momPlanet = "sun";
	planets[2].size = 18.15f;
	planets[2].speed = 0.15f;
	planets[2].radius = 115.56f;
	planetsMap.insert(std::make_pair("venus", 2));
	// earth
	planets[3].name = "earth";
	planets[3].site = glm::vec3(168.00f, 0.0f, 0.0f);   // λ��
	planets[3].color = glm::vec3(0.25, 0.88, 0.81);     // ��ɫ
	planets[3].momPlanet = "sun";
	planets[3].size = 19.13f;
	planets[3].speed = 0.1f;
	planets[3].radius = 168.00f;
	planetsMap.insert(std::make_pair("earth", 3));
	// mars
	planets[4].name = "mars";
	planets[4].site = glm::vec3(228.00f, 0.0f, 0.0f);   // λ��
	planets[4].color = glm::vec3(1, .5, .5);            // ��ɫ
	planets[4].momPlanet = "sun";
	planets[4].size = 10.19f;
	planets[4].speed = 0.08f;
	planets[4].radius = 228.00f;
	planetsMap.insert(std::make_pair("mars", 4));
	// jupiter
	planets[5].name = "jupiter";
	planets[5].site = glm::vec3(333.40f, 0.0f, 0.0f);   // λ��
	planets[5].color = glm::vec3(1, 1, .5);             // ��ɫ
	planets[5].momPlanet = "sun";
	planets[5].size = 42.90f;
	planets[5].speed = 0.07f;
	planets[5].radius = 333.40f;
	planetsMap.insert(std::make_pair("jupiter", 5));
	// saturn
	planets[6].name = "saturn";
	planets[6].site = glm::vec3(428.10f, 0.0f, 0.0f);   // λ��
	planets[6].color = glm::vec3(.52, 0.2, .98);        // ��ɫ
	planets[6].momPlanet = "sun";
	planets[6].size = 36.16f;
	planets[6].speed = 0.06f;
	planets[6].radius = 428.10f;
	planetsMap.insert(std::make_pair("saturn", 6));
	// uranus
	planets[7].name = "uranus";
	planets[7].site = glm::vec3(520.00f, 0.0f, 0.0f);   // λ��
	planets[7].color = glm::vec3(.4, .4, .4);           // ��ɫ
	planets[7].momPlanet = "sun";
	planets[7].size = 25.56f;
	planets[7].speed = 0.04f;
	planets[7].radius = 520.00f;
	planetsMap.insert(std::make_pair("uranus", 7));
	// neptune
	planets[8].name = "neptune";
	planets[8].site = glm::vec3(575.10f, 0.0f, 0.0f);   // λ��
	planets[8].color = glm::vec3(1, 0, 1);            // ��ɫ
	planets[8].momPlanet = "sun";
	planets[8].size = 24.78f;
	planets[8].speed = 0.03f;
	planets[8].radius = 575.10f;
	planetsMap.insert(std::make_pair("neptune", 8));
	// moon
	planets[9].name = "moon";
	planets[9].site = glm::vec3(168.00f+26.01f, 0.0f, 0.0f);      // λ��
	planets[9].color = glm::vec3(0.03, 0.17, 0.32);               // ��ɫ
	planets[9].momPlanet = "earth";
	planets[9].size = 5.15f;
	planets[9].speed = 0.9f;
	planets[9].radius = 26.01f;
	planetsMap.insert(std::make_pair("moon", 9));

	// ��Сʮ��
	for (int i = 0; i < PLANET_NUM; i++) {
		planets[i].radius = planets[i].radius / 10;
		planets[i].site[0] = planets[i].site[0] / 10;
		planets[i].size = planets[i].size / 10;
	}

}
// @func : ת��
void rotateUpdate(std::string name)
{
	// ׼���׶�
	float time = (float)glfwGetTime();
	float angle = fmod((time * planets[planetsMap.find(name)->second].speed), (2.0f * solarSys_PI));
	std::string momName = planets[planetsMap.find(name)->second].momPlanet;
	glm::vec3 pole = planets[planetsMap.find(momName)->second].site;
	// ����
	planets[planetsMap.find(name)->second].site.x = pole.x + planets[planetsMap.find(name)->second].radius * cos(angle);
	planets[planetsMap.find(name)->second].site.y = 0.0f;
	planets[planetsMap.find(name)->second].site.z = pole.z + planets[planetsMap.find(name)->second].radius * sin(angle);
	return ;
}


/************************* ������ *****************************/
int main()
{
	//------------------- PART 1 ��ʼ�� -------------------
	
	//��ʼ��GLFW
	glfwInit();                                                     // ��ʼ��GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);                  // opengl�汾��3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);                  // �ΰ汾��3
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // ʹ�ú���ģʽ(������������)
	// ʹ��macϵͳ
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	//�������ڣ����ߡ��������ƣ�
	GLFWwindow* window = glfwCreateWindow(solarSys_SCREEN_WIDTH, solarSys_SCREEN_HEIGHT, "solar system", nullptr, nullptr);
	//��ⴰ���Ƿ񴴽��ɹ�
	if (window == nullptr)	{
		std::cout << "Failed to Create OpenGL Context" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);                                              //�����ڵ�����������Ϊ��ǰ���̵���������
	glfwSetFramebufferSizeCallback(window, solarSys_framebuffer_size_callback);  //���ڴ�С�ص�
	glfwSetCursorPosCallback(window, solarSys_mouse_callback);                   //���ص�
	glfwSetScrollCallback(window, solarSys_scroll_callback);                     //���ֻص�
	// GLFW��׽mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	//��ʼ��GLAD������OpenGLָ���ַ�ĺ���
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	//ָ����ǰ�ӿڳߴ磨ǰ��������Ϊ���½�λ�ã���������������Ⱦ���ڿ��ߣ�
	glViewport(0, 0, solarSys_SCREEN_WIDTH, solarSys_SCREEN_HEIGHT);
	glEnable(GL_DEPTH_TEST);

	//------------------- PART 2 ���ʼ�� -------------------
	planet_ini();                        // ����Ϣ��ʼ��
	std::vector<float> sphereVertices;   // ����
	std::vector<int> sphereIndices;      // ����˳��

	std::vector<float> curveVertices;    // ����
	std::vector<int> curveIndices;       // ����˳��


	// ========== �� ==========
	// ������Ķ���ͷ�����
	for (int y = 0; y <= solarSys_Y_SEGMENTS; y++){
		for (int x = 0; x <= solarSys_X_SEGMENTS; x++){
			float xSegment = (float)x / (float)(solarSys_X_SEGMENTS);
			float ySegment = (float)y / (float)(solarSys_Y_SEGMENTS);

			float xPos = std::cos(xSegment * 2.0f * solarSys_PI) * std::sin(ySegment * solarSys_PI);
			float yPos = std::cos(ySegment * solarSys_PI);
			float zPos = std::sin(xSegment * 2.0f * solarSys_PI) * std::sin(ySegment * solarSys_PI);

			// ��������
			sphereVertices.push_back(xPos);
			sphereVertices.push_back(yPos);
			sphereVertices.push_back(zPos);

			// ������
			sphereVertices.push_back(xPos);
			sphereVertices.push_back(yPos);
			sphereVertices.push_back(zPos);

		}
	}
	// �������Indices
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
	// ���ݰ�
	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	// ���ɲ��������VAO��VBO
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// ���������ݰ�����ǰĬ�ϵĻ�����
	glBufferData(GL_ARRAY_BUFFER, sphereVertices.size() * sizeof(float), &sphereVertices[0], GL_STATIC_DRAW);
	// ����vec
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// ������vec
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// EBO���ݰ�
	GLuint element_buffer_object;
	glGenBuffers(1, &element_buffer_object);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_object);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphereIndices.size() * sizeof(int), &sphereIndices[0], GL_STATIC_DRAW);
	// ���VAO��VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// ========== ���� ==========
	// �������ߵĶ���ͷ�����
	for (int y = solarSys_Y_SEGMENTS_CURVE / 2; y <= solarSys_Y_SEGMENTS_CURVE / 2 + 1; y++) {
		for (int x = 0; x <= solarSys_X_SEGMENTS_CURVE; x++) {
			float xSegment = (float)x / (float)(solarSys_X_SEGMENTS_CURVE);
			float ySegment = (float)y / (float)(solarSys_Y_SEGMENTS_CURVE);

			float xPos = std::cos(xSegment * 2.0f * solarSys_PI) * std::sin(ySegment * solarSys_PI);
			float yPos = std::cos(ySegment * solarSys_PI);
			float zPos = std::sin(xSegment * 2.0f * solarSys_PI) * std::sin(ySegment * solarSys_PI);

			// ��������
			curveVertices.push_back(xPos);
			curveVertices.push_back(yPos);
			curveVertices.push_back(zPos);

			// ������
			curveVertices.push_back(xPos);
			curveVertices.push_back(yPos);
			curveVertices.push_back(zPos);

			// 

		}
	}
	// �������ߵ�Indices
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
	// ���ݰ�
	unsigned int curveVBO, curveVAO;
	glGenVertexArrays(1, &curveVAO);
	glGenBuffers(1, &curveVBO);
	// ���ɲ��������VAO��VBO
	glBindVertexArray(curveVAO);
	glBindBuffer(GL_ARRAY_BUFFER, curveVBO);
	// ���������ݰ�����ǰĬ�ϵĻ�����
	glBufferData(GL_ARRAY_BUFFER, curveVertices.size() * sizeof(float), &curveVertices[0], GL_STATIC_DRAW);
	// ����vec
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// ������vec
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// EBO���ݰ�
	GLuint curve_element_buffer_object;
	glGenBuffers(1, &curve_element_buffer_object);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, curve_element_buffer_object);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, curveIndices.size() * sizeof(int), &curveIndices[0], GL_STATIC_DRAW);
	// ���VAO��VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//��ɫ��
	Shader PLANET_shader("shader\\PLANET.vs", "shader\\PLANET.fs");
	Shader SUN_shader("shader\\SUN.vs", "shader\\SUN.fs");
	
	//------------------- PART 3 ��Ⱦ -------------------

	while (!glfwWindowShouldClose(window)) {
		// timing
		float currentFrame = static_cast<float>(glfwGetTime());
		solarSys_deltaTime = currentFrame - solarSys_lastFrame;
		solarSys_lastFrame = currentFrame;
		// �������
		solarSys_processInput(window);
		// �����ɫ����
		glClearColor(0.0f, 0.0f, 0.12f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// ���ݶ���
		glm::mat4 projection, view, model;
		glm::vec3 diffuseColor, ambientColor;


		// ============================ ����̫�� ============================
		SUN_shader.use();
		// view and projection����
		projection = glm::perspective(glm::radians(solarSys_camera.Zoom), (float)solarSys_SCREEN_WIDTH / (float)solarSys_SCREEN_HEIGHT, 0.1f, 100.0f);
		view = solarSys_camera.GetViewMatrix();
		SUN_shader.setMat4("projection", projection);
		SUN_shader.setMat4("view", view);
		// model����[��������λ������]
		SUN_shader.setFloat("size", planets[planetsMap.find("sun")->second].size);
		model = glm::mat4(1.0f);
		model = glm::translate(model, planets[planetsMap.find("sun")->second].site);
		SUN_shader.setMat4("model", model);
		SUN_shader.setVec3("color", planets[planetsMap.find("sun")->second].color);
		//���л���
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, solarSys_X_SEGMENTS * solarSys_Y_SEGMENTS * 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);


		// ============================ �������� ============================
		// ��ʼ��
		PLANET_shader.use();
		PLANET_shader.setVec3("light.position", planets[planetsMap.find("sun")->second].site);
		PLANET_shader.setVec3("viewPos", solarSys_camera.Position);
		// ���߲���
		diffuseColor = lightColor * glm::vec3(0.5f);    // decrease the influence
		ambientColor = diffuseColor * glm::vec3(0.2f); // low influence
		PLANET_shader.setVec3("light.ambient", ambientColor);
		PLANET_shader.setVec3("light.diffuse", diffuseColor);
		PLANET_shader.setVec3("light.specular", 0.5f, 0.5f, 0.5f);
		// ����˥��
		PLANET_shader.setFloat("light.constant", constant);
		PLANET_shader.setFloat("light.linear", linear);
		PLANET_shader.setFloat("light.quadratic", quadratic);
		// ��������
		projection = glm::perspective(glm::radians(solarSys_camera.Zoom), (float)solarSys_SCREEN_WIDTH / (float)solarSys_SCREEN_HEIGHT, 0.1f, 100.0f);
		view = solarSys_camera.GetViewMatrix();
		PLANET_shader.setMat4("projection", projection);
		PLANET_shader.setMat4("view", view);
		glm::vec3 specular = glm::vec3(0.5f, 0.5f, 0.5f);
		float shininess = 32.0f;
		for (int i = 1; i < PLANET_NUM; i++) {
			std::string Pname = planets[i].name;

			// ���ʲ���
			PLANET_shader.setVec3("material.ambient", planets[planetsMap.find(Pname)->second].color);
			PLANET_shader.setVec3("material.diffuse", planets[planetsMap.find(Pname)->second].color);
			PLANET_shader.setVec3("material.specular",specular);
			PLANET_shader.setFloat("material.shininess", shininess);

			// model����[��������λ������]
			PLANET_shader.setFloat("size", planets[planetsMap.find(Pname)->second].size);
			rotateUpdate(Pname);                                                          //��������
			model = glm::mat4(1.0f);
			model = glm::translate(model, planets[planetsMap.find(Pname)->second].site);
			PLANET_shader.setMat4("model", model);

			// ���л���
			glBindVertexArray(VAO);
			glDrawElements(GL_TRIANGLES, solarSys_X_SEGMENTS * solarSys_Y_SEGMENTS * 6, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);

		}

		// ============================ ���ƹ�� ============================
		SUN_shader.use();
		// ����view��projection����
		projection = glm::perspective(glm::radians(solarSys_camera.Zoom), (float)solarSys_SCREEN_WIDTH / (float)solarSys_SCREEN_HEIGHT, 0.1f, 100.0f);
		view = solarSys_camera.GetViewMatrix();
		SUN_shader.setMat4("projection", projection);
		SUN_shader.setMat4("view", view);
		// ���ǻ���
		for (int i = 1; i < PLANET_NUM; i++) {
			std::string name = planets[i].name;
			std::string momName = planets[planetsMap.find(name)->second].momPlanet;

			// model����[��������λ������]
			SUN_shader.setFloat("size", planets[planetsMap.find(name)->second].radius);
			model = glm::mat4(1.0f);
			model = glm::translate(model, planets[planetsMap.find(momName)->second].site);
			SUN_shader.setMat4("model", model);
			SUN_shader.setVec3("color", planets[planetsMap.find(name)->second].color);
			//���л���
			glBindVertexArray(curveVAO);
			glDrawElements(GL_TRIANGLES, solarSys_X_SEGMENTS_CURVE * 2 * 6, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		}

		//�������岢�Ҽ���Ƿ��д����¼�(����������롢����ƶ�)
		glfwSwapBuffers(window);
		glfwPollEvents();

	}


	//------------------- PART 4 �ͷ� -------------------
	
	//�ͷ�VAO��VBO��EBO
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &element_buffer_object);
	//�������е���Դ����ȷ������
	glfwTerminate();
	return 0;
}
