// @time   : 2022.11.11
// @func   : 太阳系函数定义
// @author : gonzalez 

#include "solarSys_head.h"

/************************* 函数定义 *****************************/
void solarSystem_processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		solarSystem_camera.ProcessKeyboard(FORWARD, solarSystem_deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		solarSystem_camera.ProcessKeyboard(BACKWARD, solarSystem_deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		solarSystem_camera.ProcessKeyboard(LEFT, solarSystem_deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		solarSystem_camera.ProcessKeyboard(RIGHT, solarSystem_deltaTime);
}
// @func : 界面改变回调函数
void solarSystem_framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}
// @func : 鼠标移动回调
void solarSystem_mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);
	if (solarSystem_firstMouse) {
		solarSystem_lastX = xpos;
		solarSystem_lastY = ypos;
		solarSystem_firstMouse = false;
	}
	float xoffset = xpos - solarSystem_lastX;
	float yoffset = solarSystem_lastY - ypos;
	solarSystem_lastX = xpos;
	solarSystem_lastY = ypos;
	solarSystem_camera.ProcessMouseMovement(xoffset, yoffset);
}
// @func : 鼠标滚轮回调
void solarSystem_scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	solarSystem_camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
// @func : 转动更新
void solarRotateUpdate(std::string name)
{
	// 准备阶段
	float time = (float)glfwGetTime();
	float angle = fmod((time * solarPlanets[solarPlanetsMap.find(name)->second].speed), (2.0f * solarSystem_PI));
	std::string momName = solarPlanets[solarPlanetsMap.find(name)->second].momPlanet;
	glm::vec3 pole = solarPlanets[solarPlanetsMap.find(momName)->second].site;
	// 更新
	solarPlanets[solarPlanetsMap.find(name)->second].site.x = pole.x + solarPlanets[solarPlanetsMap.find(name)->second].radius * cos(angle);
	solarPlanets[solarPlanetsMap.find(name)->second].site.y = 0.0f;
	solarPlanets[solarPlanetsMap.find(name)->second].site.z = pole.z + solarPlanets[solarPlanetsMap.find(name)->second].radius * sin(angle);
	return;
}
// @func : 加载图象
unsigned int solarSystem_loadTexture(char const* path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}
// @func : 天空盒加载
unsigned int solarSystem_loadCubemap(std::vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrComponents;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrComponents, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}
// @func : 初始化行星信息
void planets_ini(void)
{
	std::string path;

	// sun
	solarPlanets[0].name = "sun";
	solarPlanets[0].site = glm::vec3(0.0f, 0.0f, 0.0f);     // 位置
	solarPlanets[0].color = glm::vec3(0.85f, 0.0f, 0.0f);   // 颜色
	solarPlanets[0].momPlanet = "NULL";
	solarPlanets[0].size = 48.74f;
	solarPlanets[0].speed = 0.0f;
	solarPlanets[0].radius = 0.0f;
	solarPlanets[0].rotationAngle = glm::vec3(0, 1, 0);						 // 自转角度
	solarPlanets[0].rotationSpeed = 0.02f;								     // 自转速度
	path = texturePath + solarPlanets[0].name + ".jpg";
	solarPlanets[0].diffuseMap = solarSystem_loadTexture(path.c_str());      // 纹理载入
	solarPlanetsMap.insert(std::make_pair("sun", 0));
	// mercury
	solarPlanets[1].name = "mercury";
	solarPlanets[1].site = glm::vec3(62.06f, 0.0f, 0.0f);    // 位置
	solarPlanets[1].color = glm::vec3(.2, .2, .5);           // 颜色
	solarPlanets[1].momPlanet = "sun";
	solarPlanets[1].size = 7.32f;
	solarPlanets[1].speed = 0.4f;
	solarPlanets[1].rotationAngle = glm::vec3(0, 1, 0);						 // 自转角度
	solarPlanets[1].rotationSpeed = 0.15f;								     // 自转速度
	path = texturePath + solarPlanets[1].name + ".jpg";
	solarPlanets[1].diffuseMap = solarSystem_loadTexture(path.c_str());      // 纹理载入
	solarPlanets[1].radius = 62.06f;
	solarPlanetsMap.insert(std::make_pair("mercury", 1));
	// venus
	solarPlanets[2].name = "venus";
	solarPlanets[2].site = glm::vec3(115.56f, 0.0f, 0.0f);   // 位置
	solarPlanets[2].color = glm::vec3(1, .7, 0);             // 颜色
	solarPlanets[2].momPlanet = "sun";
	solarPlanets[2].size = 18.15f;
	solarPlanets[2].speed = 0.15f;
	solarPlanets[2].radius = 115.56f;
	solarPlanets[2].rotationAngle = glm::vec3(0, 1, 0);					 // 自转角度
	solarPlanets[2].rotationSpeed = 0.20f;								     // 自转速度
	path = texturePath + solarPlanets[2].name + ".jpg";
	solarPlanets[2].diffuseMap = solarSystem_loadTexture(path.c_str());      // 纹理载入
	solarPlanetsMap.insert(std::make_pair("venus", 2));
	// earth
	solarPlanets[3].name = "earth";
	solarPlanets[3].site = glm::vec3(168.00f, 0.0f, 0.0f);   // 位置
	solarPlanets[3].color = glm::vec3(0, 1, 0);              // 颜色
	solarPlanets[3].momPlanet = "sun";
	solarPlanets[3].size = 19.13f;
	solarPlanets[3].speed = 0.1f;
	solarPlanets[3].radius = 168.00f;
	solarPlanets[3].rotationAngle = glm::vec3(0, 1, 0);						 // 自转角度
	solarPlanets[3].rotationSpeed = 0.23f;								     // 自转速度
	path = texturePath + solarPlanets[3].name + ".jpg";
	solarPlanets[3].diffuseMap = solarSystem_loadTexture(path.c_str());      // 纹理载入
	solarPlanetsMap.insert(std::make_pair("earth", 3));
	// mars
	solarPlanets[4].name = "mars";
	solarPlanets[4].site = glm::vec3(228.00f, 0.0f, 0.0f);   // 位置
	solarPlanets[4].color = glm::vec3(1, .5, .5);            // 颜色
	solarPlanets[4].momPlanet = "sun";
	solarPlanets[4].size = 10.19f;
	solarPlanets[4].speed = 0.08f;
	solarPlanets[4].radius = 228.00f;
	solarPlanets[4].rotationAngle = glm::vec3(0, 1, 0);						 // 自转角度
	solarPlanets[4].rotationSpeed = 0.3f;								     // 自转速度
	path = texturePath + solarPlanets[4].name + ".jpg";
	solarPlanets[4].diffuseMap = solarSystem_loadTexture(path.c_str());      // 纹理载入
	solarPlanetsMap.insert(std::make_pair("mars", 4));
	// jupiter
	solarPlanets[5].name = "jupiter";
	solarPlanets[5].site = glm::vec3(333.40f, 0.0f, 0.0f);   // 位置
	solarPlanets[5].color = glm::vec3(1, 1, .5);             // 颜色
	solarPlanets[5].momPlanet = "sun";
	solarPlanets[5].size = 42.90f;
	solarPlanets[5].speed = 0.07f;
	solarPlanets[5].radius = 333.40f;
	solarPlanets[5].rotationAngle = glm::vec3(0, 1, 0);			     		 // 自转角度
	solarPlanets[5].rotationSpeed = 0.8f;								     // 自转速度
	path = texturePath + solarPlanets[5].name + ".jpg";
	solarPlanets[5].diffuseMap = solarSystem_loadTexture(path.c_str());      // 纹理载入
	solarPlanetsMap.insert(std::make_pair("jupiter", 5));
	// saturn
	solarPlanets[6].name = "saturn";
	solarPlanets[6].site = glm::vec3(428.10f, 0.0f, 0.0f);   // 位置
	solarPlanets[6].color = glm::vec3(.5, 1, .5);            // 颜色
	solarPlanets[6].momPlanet = "sun";
	solarPlanets[6].size = 36.16f;
	solarPlanets[6].speed = 0.06f;
	solarPlanets[6].radius = 428.10f;
	solarPlanets[6].rotationAngle = glm::vec3(0, 1, 0);						 // 自转角度
	solarPlanets[6].rotationSpeed = 0.7f;								     // 自转速度
	path = texturePath + solarPlanets[6].name + ".jpg";
	solarPlanets[6].diffuseMap = solarSystem_loadTexture(path.c_str());      // 纹理载入
	solarPlanetsMap.insert(std::make_pair("saturn", 6));
	// uranus
	solarPlanets[7].name = "uranus";
	solarPlanets[7].site = glm::vec3(520.00f, 0.0f, 0.0f);   // 位置
	solarPlanets[7].color = glm::vec3(.4, .4, .4);           // 颜色
	solarPlanets[7].momPlanet = "sun";
	solarPlanets[7].size = 25.56f;
	solarPlanets[7].speed = 0.04f;
	solarPlanets[7].radius = 520.00f;
	solarPlanets[7].rotationAngle = glm::vec3(0, 1, 0);						 // 自转角度
	solarPlanets[7].rotationSpeed = 0.6f;								     // 自转速度
	path = texturePath + solarPlanets[7].name + ".jpg";
	solarPlanets[7].diffuseMap = solarSystem_loadTexture(path.c_str());      // 纹理载入
	solarPlanetsMap.insert(std::make_pair("uranus", 7));
	// neptune
	solarPlanets[8].name = "neptune";
	solarPlanets[8].site = glm::vec3(575.10f, 0.0f, 0.0f);   // 位置
	solarPlanets[8].color = glm::vec3(.5, .5, 1);            // 颜色
	solarPlanets[8].momPlanet = "sun";
	solarPlanets[8].size = 24.78f;
	solarPlanets[8].speed = 0.03f;
	solarPlanets[8].radius = 575.10f;
	solarPlanets[8].rotationAngle = glm::vec3(0, 1, 0);						 // 自转角度
	solarPlanets[8].rotationSpeed = 0.62f;								     // 自转速度
	path = texturePath + solarPlanets[8].name + ".jpg";
	solarPlanets[8].diffuseMap = solarSystem_loadTexture(path.c_str());      // 纹理载入
	solarPlanetsMap.insert(std::make_pair("neptune", 8));
	// moon
	solarPlanets[9].name = "moon";
	solarPlanets[9].site = glm::vec3(168.00f + 26.01f, 0.0f, 0.0f);    // 位置
	solarPlanets[9].color = glm::vec3(1, 1, 0);                        // 颜色
	solarPlanets[9].momPlanet = "earth";
	solarPlanets[9].size = 6.15f;
	solarPlanets[9].speed = 0.9f;
	solarPlanets[9].radius = 26.01f;
	solarPlanets[9].rotationAngle = glm::vec3(0, 1, 0);						 // 自转角度
	solarPlanets[9].rotationSpeed = 0.08f;								     // 自转速度
	path = texturePath + solarPlanets[9].name + ".jpg";
	solarPlanets[9].diffuseMap = solarSystem_loadTexture(path.c_str());      // 纹理载入
	solarPlanetsMap.insert(std::make_pair("moon", 9));

	// 缩小十倍
	for (int i = 0; i < PLANET_NUM; i++) {
		solarPlanets[i].radius = solarPlanets[i].radius / 10;
		solarPlanets[i].site[0] = solarPlanets[i].site[0] / 10;
		solarPlanets[i].size = solarPlanets[i].size / 10;
	}

}
