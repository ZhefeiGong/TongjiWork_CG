// @time   : 2022.11.11
// @func   : ̫��ϵ������
// @author : gonzalez 

//!!! Ԥ���������޸�ͷ�ļ�������ֻ������صĺ�������Դ�룬���ڽ���ͷ�ļ���Ϊһ�� .cpp �ļ�!!!
#define STB_IMAGE_IMPLEMENTATION
#include "..\\includes\headers\stb_image.h"

#include "solarSys_head.h"



/************************* ������ *****************************/
int main()
{
	//-------------------------------------- PART 1 ��ʼ�� --------------------------------------
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
	GLFWwindow* window = glfwCreateWindow(solarSystem_SCREEN_WIDTH, solarSystem_SCREEN_HEIGHT, "solar system", nullptr, nullptr);
	//��ⴰ���Ƿ񴴽��ɹ�
	if (window == nullptr) {
		std::cout << "Failed to Create OpenGL Context" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);                                                 //�����ڵ�����������Ϊ��ǰ���̵���������
	glfwSetFramebufferSizeCallback(window, solarSystem_framebuffer_size_callback);  //���ڴ�С�ص�
	glfwSetCursorPosCallback(window, solarSystem_mouse_callback);                   //���ص�
	glfwSetScrollCallback(window, solarSystem_scroll_callback);                     //���ֻص�
	// GLFW��׽mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	//��ʼ��GLAD������OpenGLָ���ַ�ĺ���
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	//ָ����ǰ�ӿڳߴ磨ǰ��������Ϊ���½�λ�ã���������������Ⱦ���ڿ��ߣ�
	glViewport(0, 0, solarSystem_SCREEN_WIDTH, solarSystem_SCREEN_HEIGHT);
	glEnable(GL_DEPTH_TEST);

	//-------------------------------------- PART 2 ���ʼ�� --------------------------------------
	planets_ini();                        // ����Ϣ��ʼ��
	std::vector<float> sphereVertices;    // ����
	std::vector<int> sphereIndices;       // ����˳��
	std::vector<float> curveVertices;     // ����
	std::vector<int> curveIndices;        // ����˳��


	// ========== �� ==========
	// ������Ķ���+������+��ͼ����
	for (int y = 0; y <= solarSystem_Y_SEGMENTS; y++) {
		for (int x = 0; x <= solarSystem_X_SEGMENTS; x++) {
			float xSegment = (float)x / (float)(solarSystem_X_SEGMENTS);
			float ySegment = (float)y / (float)(solarSystem_Y_SEGMENTS);

			float xPos = std::cos(xSegment * 2.0f * solarSystem_PI) * std::sin(ySegment * solarSystem_PI);
			float yPos = std::cos(ySegment * solarSystem_PI);
			float zPos = std::sin(xSegment * 2.0f * solarSystem_PI) * std::sin(ySegment * solarSystem_PI);

			// ��������
			sphereVertices.push_back(xPos);
			sphereVertices.push_back(yPos);
			sphereVertices.push_back(zPos);

			// ������
			sphereVertices.push_back(xPos);
			sphereVertices.push_back(yPos);
			sphereVertices.push_back(zPos);

			// ��ͼ����
			sphereVertices.push_back(1-xSegment);
			sphereVertices.push_back(ySegment);
		}
	}
	// �������Indices
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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(0 * sizeof(float)));
	glEnableVertexAttribArray(0);
	// ������vec
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// ��ͼ����(u,v)
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// EBO���ݰ�
	GLuint element_buffer_object;
	glGenBuffers(1, &element_buffer_object);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_object);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphereIndices.size() * sizeof(int), &sphereIndices[0], GL_STATIC_DRAW);
	// ���VAO��VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// ==================== ���� ====================
	// �������ߵĶ���ͷ�����
	for (int y = solarSystem_Y_SEGMENTS_CURVE / 2; y <= solarSystem_Y_SEGMENTS_CURVE / 2 + 1; y++) {
		for (int x = 0; x <= solarSystem_X_SEGMENTS_CURVE; x++) {
			float xSegment = (float)x / (float)(solarSystem_X_SEGMENTS_CURVE);
			float ySegment = (float)y / (float)(solarSystem_Y_SEGMENTS_CURVE);

			float xPos = std::cos(xSegment * 2.0f * solarSystem_PI) * std::sin(ySegment * solarSystem_PI);
			float yPos = std::cos(ySegment * solarSystem_PI);
			float zPos = std::sin(xSegment * 2.0f * solarSystem_PI) * std::sin(ySegment * solarSystem_PI);

			// ��������
			curveVertices.push_back(xPos);
			curveVertices.push_back(yPos);
			curveVertices.push_back(zPos);

			// ������
			curveVertices.push_back(xPos);
			curveVertices.push_back(yPos);
			curveVertices.push_back(zPos);

		}
	}
	// �������ߵ�Indices
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

	// ==================== ������ ====================
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

	//��ɫ��
	Shader PLANET_shader("..\\resources\\shader\\solar_PLANET.vs", "..\\resources\\shader\\solar_PLANET.fs");			// ������ɫ��
	Shader SUN_shader("..\\resources\\shader\\solar_SUN.vs", "..\\resources\\shader\\solar_SUN.fs");					// ̫����ɫ��
	Shader CURVE_shader("..\\resources\\shader\\solar_CURVE.vs", "..\\resources\\shader\\solar_CURVE.fs");				// Բ����ɫ��
	Shader SKYBOX_shader("..\\resources\\shader\\solar_SKYBOX.vs", "..\\resources\\shader\\solar_SKYBOX.fs");			// ��պ�
	Shader MODEL_shader("..\\resources\\shader\\solar_MODEL.vs", "..\\resources\\shader\\solar_MODEL.fs");				// ģ��

	// ģ������
	//Model solar_MODEL((string)("../resources/nanosuit/nanosuit.obj"));

	// ��պ���������
	std::vector<std::string> faces{
		std::string("..\\resources\\cosmicbox\\right.png"),
		std::string("..\\resources\\cosmicbox\\left.png"),
		std::string("..\\resources\\cosmicbox\\top.png"),
		std::string("..\\resources\\cosmicbox\\bot.png"),
		std::string("..\\resources\\cosmicbox\\front.png"),
		std::string("..\\resources\\cosmicbox\\back.png"),
	};
	unsigned int cubemapTexture = solarSystem_loadCubemap(faces);


	//-------------------------------------- PART 3 ��Ⱦ --------------------------------------

	while (!glfwWindowShouldClose(window)) {
		// timing
		float currentFrame = static_cast<float>(glfwGetTime());
		solarSystem_deltaTime = currentFrame - solarSystem_lastFrame;
		solarSystem_lastFrame = currentFrame;
		// �������
		solarSystem_processInput(window);
		// �����ɫ����
		glClearColor(0.0f, 0.0f, 0.12f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// ���ݶ���
		glm::mat4 projection, view, model;
		glm::vec3 diffuseColor, ambientColor;

		// ============================ ����̫�� ============================
		SUN_shader.use();
		// view and projection����
		projection = glm::perspective(glm::radians(solarSystem_camera.Zoom), (float)solarSystem_SCREEN_WIDTH / (float)solarSystem_SCREEN_HEIGHT, 0.1f, 100.0f);
		view = solarSystem_camera.GetViewMatrix();
		SUN_shader.setMat4("projection", projection);
		SUN_shader.setMat4("view", view);
		// model����[��������λ������]
		SUN_shader.setFloat("size", solarPlanets[solarPlanetsMap.find("sun")->second].size);
		model = glm::mat4(1.0f);
		model = glm::translate(model, solarPlanets[solarPlanetsMap.find("sun")->second].site);
		model = glm::rotate(model,
			(float)glfwGetTime() * solarPlanets[solarPlanetsMap.find("sun")->second].rotationSpeed,
			solarPlanets[solarPlanetsMap.find("sun")->second].rotationAngle);
		SUN_shader.setMat4("model", model);
		ambientColor = solarLightColor * glm::vec3(0.1f);
		SUN_shader.setVec3("light", ambientColor);		  // ����
		SUN_shader.setInt("textureSun", 0);               // ̫������
		// ���� diffuse map
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, solarPlanets[solarPlanetsMap.find("sun")->second].diffuseMap);
		//���л���
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, solarSystem_X_SEGMENTS * solarSystem_Y_SEGMENTS * 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);


		// ============================ �������� ============================
		PLANET_shader.use();
		// ƽ�й�
		glm::vec3 specular = glm::vec3(0.01f, 0.01f, 0.01f);
		PLANET_shader.setVec3("dirLight.direction", dirLightDirection);
		PLANET_shader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
		PLANET_shader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
		PLANET_shader.setVec3("dirLight.specular", specular);
		// ���Դ
		PLANET_shader.setVec3("pointLights.position", solarPlanets[solarPlanetsMap.find("sun")->second].site);
		diffuseColor = solarLightColor * glm::vec3(0.5f);								    // decrease the influence
		ambientColor = diffuseColor * glm::vec3(0.001f);							    	// low influence
		PLANET_shader.setVec3("pointLights.ambient", ambientColor);							// ��Դ������
		PLANET_shader.setVec3("pointLights.diffuse", diffuseColor);							// ��Դ������
		PLANET_shader.setVec3("pointLights.specular", specular);							// ��Դ���淴��
		PLANET_shader.setFloat("pointLights.constant", solarConstant);
		PLANET_shader.setFloat("pointLights.linear", solarLinear);
		PLANET_shader.setFloat("pointLights.quadratic", solarQuadratic);
		// �۹��
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
		// ��������
		projection = glm::perspective(glm::radians(solarSystem_camera.Zoom), (float)solarSystem_SCREEN_WIDTH / (float)solarSystem_SCREEN_HEIGHT, 0.1f, 100.0f);
		view = solarSystem_camera.GetViewMatrix();
		PLANET_shader.setMat4("projection", projection);
		PLANET_shader.setMat4("view", view);
		float shininess = 32.0f;
		for (int i = 1; i < PLANET_NUM; i++) {
			std::string Pname = solarPlanets[i].name;
			// ���ʲ���
			PLANET_shader.setInt("material.diffuse", 0);				    	    // ������
			PLANET_shader.setInt("material.specular", 0);							// ���淴��
			PLANET_shader.setFloat("material.shininess", shininess);				// ����
			// model����[��������λ������]
			PLANET_shader.setFloat("size", solarPlanets[solarPlanetsMap.find(Pname)->second].size);
			// ��������
			solarRotateUpdate(Pname);
			model = glm::mat4(1.0f);
			model = glm::translate(model, solarPlanets[solarPlanetsMap.find(Pname)->second].site);
			// ��ʾ��rotationSpeed���ٶȣ���rotationAngle����ת
			model = glm::rotate(model,
				(float)glfwGetTime() * solarPlanets[solarPlanetsMap.find(Pname)->second].rotationSpeed,
				solarPlanets[solarPlanetsMap.find(Pname)->second].rotationAngle);
			PLANET_shader.setMat4("model", model);
			// ���� diffuse map
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, solarPlanets[solarPlanetsMap.find(Pname)->second].diffuseMap);
			// ���л���
			glBindVertexArray(VAO);
			glDrawElements(GL_TRIANGLES, solarSystem_X_SEGMENTS * solarSystem_Y_SEGMENTS * 6, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		}

		// ============================ ���ƹ�� ============================
		CURVE_shader.use();
		// ����view��projection����
		projection = glm::perspective(glm::radians(solarSystem_camera.Zoom), (float)solarSystem_SCREEN_WIDTH / (float)solarSystem_SCREEN_HEIGHT, 0.1f, 100.0f);
		view = solarSystem_camera.GetViewMatrix();
		CURVE_shader.setMat4("projection", projection);
		CURVE_shader.setMat4("view", view);
		// ���ǻ���
		for (int i = 1; i < PLANET_NUM; i++) {

			std::string name = solarPlanets[i].name;
			std::string momName = solarPlanets[solarPlanetsMap.find(name)->second].momPlanet;

			// model����[��������λ������]
			CURVE_shader.setFloat("size", solarPlanets[solarPlanetsMap.find(name)->second].radius);
			model = glm::mat4(1.0f);
			model = glm::translate(model, solarPlanets[solarPlanetsMap.find(momName)->second].site);
			CURVE_shader.setMat4("model", model);
			CURVE_shader.setVec3("color", solarPlanets[solarPlanetsMap.find(name)->second].color);

			// ���л���
			glBindVertexArray(curveVAO);
			glDrawElements(GL_TRIANGLES, solarSystem_X_SEGMENTS_CURVE * 2 * 6, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		}

		// =========================== ����ģ�� ===========================
		MODEL_shader.use();
		// �۹��
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

		// ������
		projection = glm::perspective(glm::radians(solarSystem_camera.Zoom), (float)solarSystem_SCREEN_WIDTH / (float)solarSystem_SCREEN_HEIGHT, 0.1f, 100.0f);
		view = solarSystem_camera.GetViewMatrix();
		MODEL_shader.setMat4("projection", projection);
		MODEL_shader.setMat4("view", view);
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -40.0f, -66.0f));	 // translate it down so it's at the center of the scene
		model = glm::scale(model, glm::vec3(3.0, 3.0, 3.0));			 // it's a bit too big for our scene, so scale it down
		MODEL_shader.setMat4("model", model);
		//solar_MODEL.Draw(MODEL_shader);									 // draw

		// =========================== ������պ� ===========================
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


		//�������岢�Ҽ���Ƿ��д����¼�(����������롢����ƶ�)
		glfwSwapBuffers(window);
		glfwPollEvents();

	}

	//-------------------------------------- PART 4 �ͷ� --------------------------------------

	//�ͷ�VAO��VBO��EBO
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &element_buffer_object);
	//�������е���Դ����ȷ������
	glfwTerminate();
	return 0;
}
