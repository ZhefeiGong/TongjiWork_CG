// @time   : 2022.11.11
// @func   : 太阳系主函数
// @author : gonzalez 

//!!! 预处理器会修改头文件，让其只包含相关的函数定义源码，等于将该头文件变为一个 .cpp 文件!!!
#define STB_IMAGE_IMPLEMENTATION
#include "..\\includes\headers\stb_image.h"

#include "solarSys_head.h"



/************************* 主函数 *****************************/
int main()
{
	//-------------------------------------- PART 1 初始化 --------------------------------------
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
	GLFWwindow* window = glfwCreateWindow(solarSystem_SCREEN_WIDTH, solarSystem_SCREEN_HEIGHT, "solar system", nullptr, nullptr);
	//检测窗口是否创建成功
	if (window == nullptr) {
		std::cout << "Failed to Create OpenGL Context" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);                                                 //将窗口的上下文设置为当前进程的主上下文
	glfwSetFramebufferSizeCallback(window, solarSystem_framebuffer_size_callback);  //窗口大小回调
	glfwSetCursorPosCallback(window, solarSystem_mouse_callback);                   //鼠标回调
	glfwSetScrollCallback(window, solarSystem_scroll_callback);                     //滚轮回调
	// GLFW捕捉mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	//初始化GLAD，加载OpenGL指针地址的函数
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	//指定当前视口尺寸（前两个参数为左下角位置，后两个参数是渲染窗口宽、高）
	glViewport(0, 0, solarSystem_SCREEN_WIDTH, solarSystem_SCREEN_HEIGHT);
	glEnable(GL_DEPTH_TEST);

	//-------------------------------------- PART 2 球初始化 --------------------------------------
	planets_ini();                        // 球信息初始化
	std::vector<float> sphereVertices;    // 顶点
	std::vector<int> sphereIndices;       // 绘制顺序
	std::vector<float> curveVertices;     // 曲线
	std::vector<int> curveIndices;        // 绘制顺序


	// ========== 球 ==========
	// 生成球的顶点+法向量+贴图坐标
	for (int y = 0; y <= solarSystem_Y_SEGMENTS; y++) {
		for (int x = 0; x <= solarSystem_X_SEGMENTS; x++) {
			float xSegment = (float)x / (float)(solarSystem_X_SEGMENTS);
			float ySegment = (float)y / (float)(solarSystem_Y_SEGMENTS);

			float xPos = std::cos(xSegment * 2.0f * solarSystem_PI) * std::sin(ySegment * solarSystem_PI);
			float yPos = std::cos(ySegment * solarSystem_PI);
			float zPos = std::sin(xSegment * 2.0f * solarSystem_PI) * std::sin(ySegment * solarSystem_PI);

			// 顶点坐标
			sphereVertices.push_back(xPos);
			sphereVertices.push_back(yPos);
			sphereVertices.push_back(zPos);

			// 法向量
			sphereVertices.push_back(xPos);
			sphereVertices.push_back(yPos);
			sphereVertices.push_back(zPos);

			// 贴图坐标
			sphereVertices.push_back(1-xSegment);
			sphereVertices.push_back(ySegment);
		}
	}
	// 生成球的Indices
	for (int i = 0; i < solarSystem_Y_SEGMENTS; i++) {
		for (int j = 0; j < solarSystem_X_SEGMENTS; j++) {
			sphereIndices.push_back(i * (solarSystem_X_SEGMENTS + 1) + j);
			sphereIndices.push_back((i + 1) * (solarSystem_X_SEGMENTS + 1) + j);
			sphereIndices.push_back((i + 1) * (solarSystem_X_SEGMENTS + 1) + j + 1);

			sphereIndices.push_back(i * (solarSystem_X_SEGMENTS + 1) + j);
			sphereIndices.push_back((i + 1) * (solarSystem_X_SEGMENTS + 1) + j + 1);
			sphereIndices.push_back(i * (solarSystem_X_SEGMENTS + 1) + j + 1);
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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(0 * sizeof(float)));
	glEnableVertexAttribArray(0);
	// 法向量vec
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// 贴图坐标(u,v)
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// EBO数据绑定
	GLuint element_buffer_object;
	glGenBuffers(1, &element_buffer_object);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_object);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphereIndices.size() * sizeof(int), &sphereIndices[0], GL_STATIC_DRAW);
	// 解绑VAO和VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// ==================== 曲线 ====================
	// 生成曲线的顶点和法向量
	for (int y = solarSystem_Y_SEGMENTS_CURVE / 2; y <= solarSystem_Y_SEGMENTS_CURVE / 2 + 1; y++) {
		for (int x = 0; x <= solarSystem_X_SEGMENTS_CURVE; x++) {
			float xSegment = (float)x / (float)(solarSystem_X_SEGMENTS_CURVE);
			float ySegment = (float)y / (float)(solarSystem_Y_SEGMENTS_CURVE);

			float xPos = std::cos(xSegment * 2.0f * solarSystem_PI) * std::sin(ySegment * solarSystem_PI);
			float yPos = std::cos(ySegment * solarSystem_PI);
			float zPos = std::sin(xSegment * 2.0f * solarSystem_PI) * std::sin(ySegment * solarSystem_PI);

			// 顶点坐标
			curveVertices.push_back(xPos);
			curveVertices.push_back(yPos);
			curveVertices.push_back(zPos);

			// 法向量
			curveVertices.push_back(xPos);
			curveVertices.push_back(yPos);
			curveVertices.push_back(zPos);

		}
	}
	// 生成曲线的Indices
	for (int i = 0; i < 1; i++) {
		for (int j = 0; j < solarSystem_X_SEGMENTS_CURVE; j++) {
			curveIndices.push_back(i * (solarSystem_X_SEGMENTS_CURVE + 1) + j);
			curveIndices.push_back((i + 1) * (solarSystem_X_SEGMENTS_CURVE + 1) + j);
			curveIndices.push_back((i + 1) * (solarSystem_X_SEGMENTS_CURVE + 1) + j + 1);

			curveIndices.push_back(i * (solarSystem_X_SEGMENTS_CURVE + 1) + j);
			curveIndices.push_back((i + 1) * (solarSystem_X_SEGMENTS_CURVE + 1) + j + 1);
			curveIndices.push_back(i * (solarSystem_X_SEGMENTS_CURVE + 1) + j + 1);
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

	// ==================== 背景盒 ====================
	float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};

	// skybox VAO
	unsigned int skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	//着色器
	Shader PLANET_shader("..\\resources\\shader\\solar_PLANET.vs", "..\\resources\\shader\\solar_PLANET.fs");			// 行星着色器
	Shader SUN_shader("..\\resources\\shader\\solar_SUN.vs", "..\\resources\\shader\\solar_SUN.fs");					// 太阳着色器
	Shader CURVE_shader("..\\resources\\shader\\solar_CURVE.vs", "..\\resources\\shader\\solar_CURVE.fs");				// 圆弧着色器
	Shader SKYBOX_shader("..\\resources\\shader\\solar_SKYBOX.vs", "..\\resources\\shader\\solar_SKYBOX.fs");			// 天空盒
	Shader MODEL_shader("..\\resources\\shader\\solar_MODEL.vs", "..\\resources\\shader\\solar_MODEL.fs");				// 模型

	// 模型载入
	//Model solar_MODEL((string)("../resources/nanosuit/nanosuit.obj"));

	// 天空盒纹理载入
	std::vector<std::string> faces{
		std::string("..\\resources\\cosmicbox\\right.png"),
		std::string("..\\resources\\cosmicbox\\left.png"),
		std::string("..\\resources\\cosmicbox\\top.png"),
		std::string("..\\resources\\cosmicbox\\bot.png"),
		std::string("..\\resources\\cosmicbox\\front.png"),
		std::string("..\\resources\\cosmicbox\\back.png"),
	};
	unsigned int cubemapTexture = solarSystem_loadCubemap(faces);


	//-------------------------------------- PART 3 渲染 --------------------------------------

	while (!glfwWindowShouldClose(window)) {
		// timing
		float currentFrame = static_cast<float>(glfwGetTime());
		solarSystem_deltaTime = currentFrame - solarSystem_lastFrame;
		solarSystem_lastFrame = currentFrame;
		// 检测输入
		solarSystem_processInput(window);
		// 清空颜色缓冲
		glClearColor(0.0f, 0.0f, 0.12f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// 数据定义
		glm::mat4 projection, view, model;
		glm::vec3 diffuseColor, ambientColor;

		// ============================ 绘制太阳 ============================
		SUN_shader.use();
		// view and projection矩阵
		projection = glm::perspective(glm::radians(solarSystem_camera.Zoom), (float)solarSystem_SCREEN_WIDTH / (float)solarSystem_SCREEN_HEIGHT, 0.1f, 100.0f);
		view = solarSystem_camera.GetViewMatrix();
		SUN_shader.setMat4("projection", projection);
		SUN_shader.setMat4("view", view);
		// model矩阵[行星中心位置坐标]
		SUN_shader.setFloat("size", solarPlanets[solarPlanetsMap.find("sun")->second].size);
		model = glm::mat4(1.0f);
		model = glm::translate(model, solarPlanets[solarPlanetsMap.find("sun")->second].site);
		model = glm::rotate(model,
			(float)glfwGetTime() * solarPlanets[solarPlanetsMap.find("sun")->second].rotationSpeed,
			solarPlanets[solarPlanetsMap.find("sun")->second].rotationAngle);
		SUN_shader.setMat4("model", model);
		ambientColor = solarLightColor * glm::vec3(0.1f);
		SUN_shader.setVec3("light", ambientColor);		  // 光线
		SUN_shader.setInt("textureSun", 0);               // 太阳材质
		// 捆绑 diffuse map
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, solarPlanets[solarPlanetsMap.find("sun")->second].diffuseMap);
		//进行绘制
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, solarSystem_X_SEGMENTS * solarSystem_Y_SEGMENTS * 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);


		// ============================ 绘制行星 ============================
		PLANET_shader.use();
		// 平行光
		glm::vec3 specular = glm::vec3(0.01f, 0.01f, 0.01f);
		PLANET_shader.setVec3("dirLight.direction", dirLightDirection);
		PLANET_shader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
		PLANET_shader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
		PLANET_shader.setVec3("dirLight.specular", specular);
		// 点光源
		PLANET_shader.setVec3("pointLights.position", solarPlanets[solarPlanetsMap.find("sun")->second].site);
		diffuseColor = solarLightColor * glm::vec3(0.5f);								    // decrease the influence
		ambientColor = diffuseColor * glm::vec3(0.001f);							    	// low influence
		PLANET_shader.setVec3("pointLights.ambient", ambientColor);							// 光源环境光
		PLANET_shader.setVec3("pointLights.diffuse", diffuseColor);							// 光源漫反射
		PLANET_shader.setVec3("pointLights.specular", specular);							// 光源镜面反射
		PLANET_shader.setFloat("pointLights.constant", solarConstant);
		PLANET_shader.setFloat("pointLights.linear", solarLinear);
		PLANET_shader.setFloat("pointLights.quadratic", solarQuadratic);
		// 聚光灯
		PLANET_shader.setVec3("viewPos", solarSystem_camera.Position);
		PLANET_shader.setVec3("spotLight.position", solarSystem_camera.Position);
		PLANET_shader.setVec3("spotLight.direction", solarSystem_camera.Front);
		PLANET_shader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
		PLANET_shader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
		PLANET_shader.setVec3("spotLight.specular", specular);
		PLANET_shader.setFloat("spotLight.constant", 1.0f);
		PLANET_shader.setFloat("spotLight.linear", 0.09f);
		PLANET_shader.setFloat("spotLight.quadratic", 0.032f);
		PLANET_shader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
		PLANET_shader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));
		// 绘制行星
		projection = glm::perspective(glm::radians(solarSystem_camera.Zoom), (float)solarSystem_SCREEN_WIDTH / (float)solarSystem_SCREEN_HEIGHT, 0.1f, 100.0f);
		view = solarSystem_camera.GetViewMatrix();
		PLANET_shader.setMat4("projection", projection);
		PLANET_shader.setMat4("view", view);
		float shininess = 32.0f;
		for (int i = 1; i < PLANET_NUM; i++) {
			std::string Pname = solarPlanets[i].name;
			// 材质参数
			PLANET_shader.setInt("material.diffuse", 0);				    	    // 漫反射
			PLANET_shader.setInt("material.specular", 0);							// 镜面反射
			PLANET_shader.setFloat("material.shininess", shininess);				// 亮度
			// model矩阵[行星中心位置坐标]
			PLANET_shader.setFloat("size", solarPlanets[solarPlanetsMap.find(Pname)->second].size);
			// 更新坐标
			solarRotateUpdate(Pname);
			model = glm::mat4(1.0f);
			model = glm::translate(model, solarPlanets[solarPlanetsMap.find(Pname)->second].site);
			// 表示以rotationSpeed的速度，沿rotationAngle轴旋转
			model = glm::rotate(model,
				(float)glfwGetTime() * solarPlanets[solarPlanetsMap.find(Pname)->second].rotationSpeed,
				solarPlanets[solarPlanetsMap.find(Pname)->second].rotationAngle);
			PLANET_shader.setMat4("model", model);
			// 捆绑 diffuse map
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, solarPlanets[solarPlanetsMap.find(Pname)->second].diffuseMap);
			// 进行绘制
			glBindVertexArray(VAO);
			glDrawElements(GL_TRIANGLES, solarSystem_X_SEGMENTS * solarSystem_Y_SEGMENTS * 6, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		}

		// ============================ 绘制轨道 ============================
		CURVE_shader.use();
		// 设置view和projection矩阵
		projection = glm::perspective(glm::radians(solarSystem_camera.Zoom), (float)solarSystem_SCREEN_WIDTH / (float)solarSystem_SCREEN_HEIGHT, 0.1f, 100.0f);
		view = solarSystem_camera.GetViewMatrix();
		CURVE_shader.setMat4("projection", projection);
		CURVE_shader.setMat4("view", view);
		// 行星绘制
		for (int i = 1; i < PLANET_NUM; i++) {

			std::string name = solarPlanets[i].name;
			std::string momName = solarPlanets[solarPlanetsMap.find(name)->second].momPlanet;

			// model矩阵[行星中心位置坐标]
			CURVE_shader.setFloat("size", solarPlanets[solarPlanetsMap.find(name)->second].radius);
			model = glm::mat4(1.0f);
			model = glm::translate(model, solarPlanets[solarPlanetsMap.find(momName)->second].site);
			CURVE_shader.setMat4("model", model);
			CURVE_shader.setVec3("color", solarPlanets[solarPlanetsMap.find(name)->second].color);

			// 进行绘制
			glBindVertexArray(curveVAO);
			glDrawElements(GL_TRIANGLES, solarSystem_X_SEGMENTS_CURVE * 2 * 6, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		}

		// =========================== 绘制模型 ===========================
		MODEL_shader.use();
		// 聚光灯
		MODEL_shader.setVec3("viewPos", solarSystem_camera.Position);
		MODEL_shader.setVec3("spotLight.position", solarSystem_camera.Position);
		MODEL_shader.setVec3("spotLight.direction", solarSystem_camera.Front);
		MODEL_shader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
		MODEL_shader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
		MODEL_shader.setVec3("spotLight.specular", specular);
		MODEL_shader.setFloat("spotLight.constant", 1.0f);
		MODEL_shader.setFloat("spotLight.linear", 0.09f);
		MODEL_shader.setFloat("spotLight.quadratic", 0.032f);
		MODEL_shader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
		MODEL_shader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));
		MODEL_shader.setFloat("shinness", 32.0f);

		// 矩阵传入
		projection = glm::perspective(glm::radians(solarSystem_camera.Zoom), (float)solarSystem_SCREEN_WIDTH / (float)solarSystem_SCREEN_HEIGHT, 0.1f, 100.0f);
		view = solarSystem_camera.GetViewMatrix();
		MODEL_shader.setMat4("projection", projection);
		MODEL_shader.setMat4("view", view);
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -40.0f, -66.0f));	 // translate it down so it's at the center of the scene
		model = glm::scale(model, glm::vec3(3.0, 3.0, 3.0));			 // it's a bit too big for our scene, so scale it down
		MODEL_shader.setMat4("model", model);
		//solar_MODEL.Draw(MODEL_shader);									 // draw

		// =========================== 绘制填空盒 ===========================
		glDepthFunc(GL_LEQUAL);											 // change depth function so depth test passes when values are equal to depth buffer's content
		SKYBOX_shader.use();
		view = glm::mat4(glm::mat3(solarSystem_camera.GetViewMatrix())); // remove translation from the view matrix
		SKYBOX_shader.setMat4("view", view);
		SKYBOX_shader.setMat4("projection", projection);
		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS);											 // set depth function back to default


		//交换缓冲并且检查是否有触发事件(比如键盘输入、鼠标移动)
		glfwSwapBuffers(window);
		glfwPollEvents();

	}

	//-------------------------------------- PART 4 释放 --------------------------------------

	//释放VAO和VBO，EBO
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &element_buffer_object);
	//清理所有的资源并正确出程序
	glfwTerminate();
	return 0;
}
