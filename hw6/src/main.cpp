#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

//使用了官方的shader库文件，可以更方便地操作shader
//注意，使用官方库操作shadder时，自己写的顶点和片段着色器代码必须另外写成文件
//然后把文件路径传入shader.h提供的构造函数
#include "shader.h"  
#include "camera.h"

using namespace std;

const int WINDOW_WIDTH = 1000;
const int WINDOW_HEIGHT = 800;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

//camera的位置与后面物体的view向量相反
Camera camera(glm::vec3(0.0f, 0.0f, 10.0f));
//鼠标位置
float mouseX = 0.0f;
float mouseY = 0.0f;
bool firstMouse = true;
//每一帧间隔时间
float deltaTime = 0.0f;
//上一帧的时刻
float lastFrame = 0.0f;

int main() {
	//初始化opengl窗口和配置
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	//深度测试
	glEnable(GL_DEPTH_TEST);

	//创建并绑定ImGui
	const char* glsl_version = "#version 130";
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	//正方体边长为2.0f*2s
	float half_edge = 2.0f;
	glm::vec3 verticalVec1 = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 verticalVec2 = glm::vec3(0.0f, 0.0f, 1.0f);
	glm::vec3 verticalVec3 = glm::vec3(-1.0f, 0.0f, 0.0f);
	glm::vec3 verticalVec4 = glm::vec3(1.0f, 0.0f, 0.0f);
	glm::vec3 verticalVec5 = glm::vec3(0.0f, -1.0f, 0.0f);
	glm::vec3 verticalVec6 = glm::vec3(0.0f, 1.0f, 0.0f);

	float vertices[] = {
	-half_edge, -half_edge, -half_edge, verticalVec1.x, verticalVec1.y, verticalVec1.z,
	 half_edge, -half_edge, -half_edge, verticalVec1.x, verticalVec1.y, verticalVec1.z,
	 half_edge,  half_edge, -half_edge, verticalVec1.x, verticalVec1.y, verticalVec1.z,
	 half_edge,  half_edge, -half_edge, verticalVec1.x, verticalVec1.y, verticalVec1.z,
	-half_edge,  half_edge, -half_edge, verticalVec1.x, verticalVec1.y, verticalVec1.z,
	-half_edge, -half_edge, -half_edge, verticalVec1.x, verticalVec1.y, verticalVec1.z,

	-half_edge, -half_edge,  half_edge, verticalVec2.x, verticalVec2.y, verticalVec2.z,
	 half_edge, -half_edge,  half_edge, verticalVec2.x, verticalVec2.y, verticalVec2.z,
	 half_edge,  half_edge,  half_edge, verticalVec2.x, verticalVec2.y, verticalVec2.z,
	 half_edge,  half_edge,  half_edge, verticalVec2.x, verticalVec2.y, verticalVec2.z,
	-half_edge,  half_edge,  half_edge, verticalVec2.x, verticalVec2.y, verticalVec2.z,
	-half_edge, -half_edge,  half_edge, verticalVec2.x, verticalVec2.y, verticalVec2.z,

	-half_edge,  half_edge,  half_edge, verticalVec3.x, verticalVec3.y, verticalVec3.z,
	-half_edge,  half_edge, -half_edge, verticalVec3.x, verticalVec3.y, verticalVec3.z,
	-half_edge, -half_edge, -half_edge, verticalVec3.x, verticalVec3.y, verticalVec3.z,
	-half_edge, -half_edge, -half_edge, verticalVec3.x, verticalVec3.y, verticalVec3.z,
	-half_edge, -half_edge,  half_edge, verticalVec3.x, verticalVec3.y, verticalVec3.z,
	-half_edge,  half_edge,  half_edge, verticalVec3.x, verticalVec3.y, verticalVec3.z,

	 half_edge,  half_edge,  half_edge, verticalVec4.x, verticalVec4.y, verticalVec4.z,
	 half_edge,  half_edge, -half_edge, verticalVec4.x, verticalVec4.y, verticalVec4.z,
	 half_edge, -half_edge, -half_edge, verticalVec4.x, verticalVec4.y, verticalVec4.z,
	 half_edge, -half_edge, -half_edge, verticalVec4.x, verticalVec4.y, verticalVec4.z,
	 half_edge, -half_edge,  half_edge, verticalVec4.x, verticalVec4.y, verticalVec4.z,
	 half_edge,  half_edge,  half_edge, verticalVec4.x, verticalVec4.y, verticalVec4.z,

	-half_edge, -half_edge, -half_edge, verticalVec5.x, verticalVec5.y, verticalVec5.z,
	 half_edge, -half_edge, -half_edge, verticalVec5.x, verticalVec5.y, verticalVec5.z,
	 half_edge, -half_edge,  half_edge, verticalVec5.x, verticalVec5.y, verticalVec5.z,
	 half_edge, -half_edge,  half_edge, verticalVec5.x, verticalVec5.y, verticalVec5.z,
	-half_edge, -half_edge,  half_edge, verticalVec5.x, verticalVec5.y, verticalVec5.z,
	-half_edge, -half_edge, -half_edge, verticalVec5.x, verticalVec5.y, verticalVec5.z,

	-half_edge,  half_edge, -half_edge, verticalVec6.x, verticalVec6.y, verticalVec6.z,
	 half_edge,  half_edge, -half_edge, verticalVec6.x, verticalVec6.y, verticalVec6.z,
	 half_edge,  half_edge,  half_edge, verticalVec6.x, verticalVec6.y, verticalVec6.z,
	 half_edge,  half_edge,  half_edge, verticalVec6.x, verticalVec6.y, verticalVec6.z,
	-half_edge,  half_edge,  half_edge, verticalVec6.x, verticalVec6.y, verticalVec6.z,
	-half_edge,  half_edge, -half_edge, verticalVec6.x, verticalVec6.y, verticalVec6.z,
	};

	unsigned int indices[] = { // 注意索引从0开始! 
		0, 1, 2, // 第一个三角形
		3, 4, 5,

		6, 7, 8,
		9, 10, 11,
		
		12, 13, 14,
		15, 16, 17,

		18, 19, 20,
		21, 22, 23,

		24, 25, 26,
		27, 28, 29,

		30, 31, 32,
		33, 34, 35
	};

	//物体（正方体）的VAO
	unsigned int objectVAO;
	unsigned int VBO;
	unsigned int EBO;

	//必须先绑定VA0
	glGenVertexArrays(1, &objectVAO);
	glBindVertexArray(objectVAO);

	//再绑定VBO
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//使用EBO画多个三角形，组合成其它图形
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	
	//再设置属性
	//位置属性
	//属性位置值为0的顶点属性，有3个值，顶点着色器总参数大小为6个float，位置属性偏移为0
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//法向量属性
	//属性位置值为0的顶点属性，有3个值，顶点着色器总参数大小为6个float，位置属性偏移为3个float
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//灯光VAO
	unsigned int lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	//顶点着色器总参数大小为6个float，是为了与前面的objectVAO保持一致
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	float scaleDelta = 1.0f;
	float rotateAngle = glm::radians(15.0f);
	float perspective_fov = 45.0f;
	float perspective_division = (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT;
	float perspective_near = 0.1f;
	float perspective_far = 100.0f;
	bool enable_camera = false;
	bool is_phong_shader = true;
	bool is_gouraud_shader = false;
	bool enable_translate = false;
	bool is_translate_to_left = true;
	bool show_window = true;
	float ambientStrength = 0.1f;
	float diffuseStrength = 1.0f;
	float specularStrength = 1.0f;
	int ShininessStrength = 32;
	glm::vec3 lightPos(20.0f, 8.0f, 20.0f);
	glm::vec3 viewPos;

	while (!glfwWindowShouldClose(window)) {
		if (enable_camera) {
			//告诉glfw获取我们的鼠标
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
		else {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
	
		//计算当前时间和帧间隔时间，为了使移动更平滑
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);

		//清除屏幕
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//创建坐标变换
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);
		
		if (enable_camera) {
			view = camera.GetViewMatrix();
			viewPos = camera.Position;
			
			//它的第一个参数定义了fov的值，它表示的是视野，并且设置了观察空间的大小。第二个参数设置了宽高比。第三和第四个参数设置了平截头体的近和远平面
			//glm::radians()是把角度转为弧度
			//利用鼠标放缩
			projection = glm::perspective(glm::radians(camera.Zoom), perspective_division, perspective_near, perspective_far);

		}
		else {
			//把场景移动(-1.0f, -1.0f, -5.0f)
			view = glm::translate(view, glm::vec3(0.0f, 0.0f, -10.0f));
			viewPos = glm::vec3(1.0f, 1.0f, 5.0f);
			
			projection = glm::perspective(glm::radians(perspective_fov), perspective_division, perspective_near, perspective_far);
		}

		//灯光左右移动
		if (enable_translate) {
			if (is_translate_to_left) {
				lightPos.x = lightPos.x - 0.5f;
				if (lightPos.x < -30) {
					is_translate_to_left = false;
				}
			}
			else {
				lightPos.x = lightPos.x + 0.5f;
				if (lightPos.x > 30) {
					is_translate_to_left = true;
				}
			}	
		}

		//利用官方库shader.h构造着色器
		if (is_phong_shader) {
			Shader objectShader("objectVertexPhongShader.vs", "objectFragmentPhongShader.fs");
			//使用着色器程序
			objectShader.use();
			//设置着色器程序uniform
			objectShader.setFloat("ambientStrength", ambientStrength);
			objectShader.setFloat("diffuseStrength", diffuseStrength);
			objectShader.setFloat("specularStrength", specularStrength);
			objectShader.setInt("ShininessStrength", ShininessStrength);

			objectShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
			objectShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
			objectShader.setVec3("lightPos", lightPos);
			objectShader.setVec3("viewPos", viewPos);

			objectShader.setMat4("view", view);
			objectShader.setMat4("projection", projection);
			//model包含了位移、缩放与旋转操作
			model = glm::mat4(1.0f);
			model = glm::rotate(model, rotateAngle, glm::vec3(0.0f, 1.0f, 0.0f));
			objectShader.setMat4("model", model);

			//画物体
			glBindVertexArray(objectVAO);
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		}
		else if (is_gouraud_shader) {
			Shader objectShader("objectVertexGouraudShader.vs", "objectFragmentGouraudShader.fs");
			//使用着色器程序
			objectShader.use();
			//设置着色器程序uniform
			objectShader.setFloat("ambientStrength", ambientStrength);
			objectShader.setFloat("diffuseStrength", diffuseStrength);
			objectShader.setFloat("specularStrength", specularStrength);
			objectShader.setInt("ShininessStrength", ShininessStrength);

			objectShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
			objectShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
			objectShader.setVec3("lightPos", lightPos);
			objectShader.setVec3("viewPos", viewPos);

			objectShader.setMat4("view", view);
			objectShader.setMat4("projection", projection);
			//model包含了位移、缩放与旋转操作
			model = glm::mat4(1.0f);
			model = glm::rotate(model, rotateAngle, glm::vec3(0.0f, 1.0f, 0.0f));
			objectShader.setMat4("model", model);

			//画物体
			glBindVertexArray(objectVAO);
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		}

		Shader lightShader("lightVertexShader.vs", "lightFragmentShader.fs");

		lightShader.use();
		lightShader.setMat4("view", view);
		lightShader.setMat4("projection", projection);
		model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(0.2f));
		model = glm::translate(model, lightPos);
		lightShader.setMat4("model", model);

		//画灯光
		glBindVertexArray(lightVAO);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

		//创建imgui
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::Begin("Edit cube", &show_window, ImGuiWindowFlags_MenuBar);

		if (ImGui::Button("enable camera")) {
			if (enable_camera) {
				enable_camera = false;
			}
			else {
				enable_camera = true;
			}
		}
		
		if (ImGui::RadioButton("Phong Shader", is_phong_shader)) {
			is_gouraud_shader = false;
			is_phong_shader = true;
		}
		if (ImGui::RadioButton("Gouraud Shader", is_gouraud_shader)) {
			is_gouraud_shader = true;
			is_phong_shader = false;
		}
		ImGui::SliderFloat("ambientStrength", &ambientStrength, 0.01f, 3.0f);
		ImGui::SliderFloat("diffuseStrength", &diffuseStrength, 0.01f, 3.0f);
		ImGui::SliderFloat("specularStrength", &specularStrength, 0.01f, 5.0f);
		ImGui::SliderInt("ShininessStrength", &ShininessStrength, 0, 256);
		
		if (ImGui::Button("enable translate")) {
			if (enable_translate) {
				enable_translate = false;
			}
			else {
				enable_translate = true;
			}
		}

		ImGui::End();

		ImGui::Render();
		int display_w, display_h;
		glfwMakeContextCurrent(window);
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	glDeleteVertexArrays(1, &objectVAO);
	glDeleteVertexArrays(1, &lightVAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	glfwTerminate();

	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

//鼠标移动
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	if (firstMouse)
	{
		mouseX = xpos;
		mouseY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - mouseX;
	float yoffset = mouseY - ypos;

	mouseX = xpos;
	mouseY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

//鼠标放缩
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	camera.ProcessMouseScroll(yoffset);
}