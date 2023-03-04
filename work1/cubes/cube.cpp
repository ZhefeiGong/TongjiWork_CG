// @time   : 2022.9.30
// @func   : ʵ��animateCube��assignment1_CV��
// @notice : none
// @author : gonzalez

/**********************  ͷ�ļ� **********************/
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "..\\includes\headers\stb_image.h"
#include "..\\includes\glm\glm\glm.hpp"
#include "..\\includes\glm\glm\gtc\matrix_transform.hpp"
#include "..\\includes\glm\glm\gtc\type_ptr.hpp"
#include "..\\includes\headers\shader_m.h"
#include <iostream>

/********************** ȫ�ֱ��� **********************/
// ��������
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
// ���
glm::vec3  cube_cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3  cube_cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3  cube_cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

bool  cube_firstMouse = true;
float  cube_yaw = -90.0f;
float  cube_pitch = 0.0f;
float  cube_lastX = 800.0f / 2.0;
float  cube_lastY = 600.0 / 2.0;
float  cube_fov = 45.0f;
// ��ʱ
float  cube_deltaTime = 0.0f;
float  cube_lastFrame = 0.0f;

/********************** ȫ�ֱ��� **********************/
// @func : ������벶׽
void cube_processInput(GLFWwindow * window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float cameraSpeed = static_cast<float>(2.5 * cube_deltaTime);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cube_cameraPos += cameraSpeed * cube_cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cube_cameraPos -= cameraSpeed * cube_cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cube_cameraPos -= glm::normalize(glm::cross(cube_cameraFront, cube_cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cube_cameraPos += glm::normalize(glm::cross(cube_cameraFront, cube_cameraUp)) * cameraSpeed;
}
// @func : ����ص�����
void cube_framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}
// @func : ����ƶ��ص�����
void cube_mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);
    // �����һ���������ͻ�����
    if (cube_firstMouse)               
    {
        cube_lastX = xpos;
        cube_lastY = ypos;
        cube_firstMouse = false;
    }
    // �������ƫ����
    float xoffset = xpos - cube_lastX;
    float yoffset = cube_lastY - ypos; 
    cube_lastX = xpos;
    cube_lastY = ypos;
    // ����������ֵ
    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    cube_yaw += xoffset;
    cube_pitch += yoffset;
    // ����������ƣ�������ֵ��ƶ�
    if (cube_pitch > 89.0f)
        cube_pitch = 89.0f;
    if (cube_pitch < -89.0f)
        cube_pitch = -89.0f;
    // ͨ�������Ǻ�ƫ�����������Եõ������ķ�������
    glm::vec3 front;
    front.x = cos(glm::radians(cube_yaw)) * cos(glm::radians(cube_pitch));
    front.y = sin(glm::radians(cube_pitch));
    front.z = sin(glm::radians(cube_yaw)) * cos(glm::radians(cube_pitch));
    cube_cameraFront = glm::normalize(front);
}
// @func : �����ֻص�����(Zoom)
void cube_scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    cube_fov -= (float)yoffset;
    if (cube_fov < 1.0f)
        cube_fov = 1.0f;
    if (cube_fov > 45.0f)
        cube_fov = 45.0f;
}
// @func : ����޸�cube����ɫֵ
void changeCubeColor(float* array)
{
    float vertex[] = {
        0.5f,0.5f,0.5f,      1.0f, 0.0f, 0.0f,
        0.5f,0.5f,-0.5f,     0.0f, 1.0f, 0.0f,
        0.5f,-0.5f,-0.5f,    0.0f, 0.0f, 1.0f,
        0.5f,-0.5f,0.5f,     0.5f, 0.5f, 0.5f,
        -0.5f,0.5f,-0.5f,    0.0f, 0.5f, 0.5f,
        -0.5f,-0.5f,0.5f,    0.5f, 0.5f, 0.0f,
        -0.5f,0.5f,0.5f,     0.5f, 0.0f, 0.5f,
        -0.5f,-0.5f,-0.5f,   0.2f, 0.2f, 0.8f
    };
    for (int i = 0; i < 8; i++) {
        for (int j = 3; j < 6; j++) {
            float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
            vertex[i * 6 + j] = r;
        }
    }
    for (int i = 0; i < 36; i++) {
        for (int j = 0; j < 8; j++) {
            bool find = true;
            for (int k = 0; k < 3; k++) {
                if (abs(array[i * 6 + k]-vertex[j * 6 + k])<1e-6) {
                    find = false;
                    break;
                }
            }
            if (find) {
                array[i * 6 + 3] = vertex[j * 6 + 3];
                array[i * 6 + 4] = vertex[j * 6 + 4];
                array[i * 6 + 5] = vertex[j * 6 + 5];
                break;
            }
        }
    }
    return;
}

/********************** ȫ�ֱ��� **********************/
int main()
{
    // ��ʼ��glfw
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // ����glfw����
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "cube", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // ��д�ص�����
    glfwSetFramebufferSizeCallback(window, cube_framebuffer_size_callback);
    glfwSetCursorPosCallback(window, cube_mouse_callback);
    glfwSetScrollCallback(window, cube_scroll_callback);

    // ���ع�겢��׽��
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // ����OpenGL����ָ��
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // ����ȫ��OpenGL״̬
    glEnable(GL_DEPTH_TEST);

    // ������ɫ��
    Shader ourShader("shader\\cube.vs", "shader\\cube.fs");

    // 6���� x ÿ������2����������� x ÿ����������3������ --> 36������
    float vertices[] = {
        
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 0.0f
    };

    changeCubeColor(vertices);

    // ��������ϵ��cubeλ��
    glm::vec3 cubePositions[] = {
        glm::vec3(0.0f,  0.0f,  0.0f),
        glm::vec3(2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),
        glm::vec3(1.5f,  2.0f, -2.5f),
        glm::vec3(1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // buffer���ݳ�ʼ��
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // λ������
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // ��ɫ����
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    ourShader.use();

    // ������Ⱦ
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        float currentFrame = static_cast<float>(glfwGetTime());
        cube_deltaTime = currentFrame - cube_lastFrame;
        cube_lastFrame = currentFrame;

        // input
        cube_processInput(window);

        // render
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ourShader.use();

        // ͸�Ӿ���
        glm::mat4 projection = glm::perspective(glm::radians(cube_fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        ourShader.setMat4("projection", projection);

        // �۲����
        glm::mat4 view = glm::lookAt(cube_cameraPos, cube_cameraPos + cube_cameraFront, cube_cameraUp);
        ourShader.setMat4("view", view);

        // ��Ⱦboxes
        glBindVertexArray(VAO);
        for (unsigned int i = 0; i < sizeof(cubePositions) / sizeof(glm::vec3); i++)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);

            // ��ʱ����ת
            model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
            // �̶���ת
            //model = glm::rotate(model, glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));


            ourShader.setMat4("model", model);
            int angle = (float)glfwGetTime() * 50.0f + 1;
            if ((int)angle % 60 == 0) {
                changeCubeColor(vertices);
                // buffer�������³�ʼ��
                glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
            }

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // �ͷ���Դ
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    // ��ֹ
    glfwTerminate();
    return 0;
}

