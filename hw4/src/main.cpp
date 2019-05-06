#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

using namespace std;

const int WINDOW_WIDTH = 1000;
const int WINDOW_HEIGHT = 800;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

//顶点着色器可以向片段着色器传数据
const char* vertexShaderSource = "#version 330 core\n"
	"layout (location = 0) in vec3 aPos;\n"
	"layout (location = 1) in vec3 aColor;\n"
	"out vec3 ourColor;\n"
	"uniform mat4 model;\n"
	"uniform mat4 view;\n"
	"uniform mat4 projection;\n"
	"void main()\n"
	"{\n"
	"gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
	"ourColor = aColor;\n"
	"}\0";

//片段着色器
const char* fragmentShaderSource = "#version 330 core\n"
	"out vec4 FragColor;\n"
	"in vec3 ourColor;\n"
	"void main()\n"
	"{\n"
	"FragColor = vec4(ourColor, 1.0);\n"
	"}\0";

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

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	//创建并绑定ImGui
	const char* glsl_version = "#version 130";
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	//顶点着色器
	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	//片段着色器
	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	//着色器程序
	unsigned int shaderProgram;
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	bool show_window = true;

	unsigned int VAO;
	unsigned int VBO;
	unsigned int EBO;

	//正方体边长为2.0f*2s
	float half_edge = 2.0f;

	//颜色数据
	ImVec4 color1 = ImVec4(0.5f, 0.0f, 0.0f, 1.0f);
	ImVec4 color2 = ImVec4(0.0f, 0.5f, 0.0f, 1.0f);
	ImVec4 color3 = ImVec4(0.0f, 0.0f, 0.5f, 1.0f);
	ImVec4 color4 = ImVec4(0.0f, 0.5f, 0.5f, 1.0f);
	ImVec4 color5 = ImVec4(0.5f, 0.0f, 0.5f, 1.0f);
	ImVec4 color6 = ImVec4(0.5f, 0.5f, 0.0f, 1.0f);

	float vertices[] = {
	-half_edge, -half_edge, -half_edge, color1.x, color1.y, color1.z,
	 half_edge, -half_edge, -half_edge, color1.x, color1.y, color1.z,
	 half_edge,  half_edge, -half_edge, color1.x, color1.y, color1.z,
	 half_edge,  half_edge, -half_edge, color1.x, color1.y, color1.z,
	-half_edge,  half_edge, -half_edge, color1.x, color1.y, color1.z,
	-half_edge, -half_edge, -half_edge, color1.x, color1.y, color1.z,

	-half_edge, -half_edge,  half_edge, color2.x, color2.y, color2.z,
	 half_edge, -half_edge,  half_edge, color2.x, color2.y, color2.z,
	 half_edge,  half_edge,  half_edge, color2.x, color2.y, color2.z,
	 half_edge,  half_edge,  half_edge, color2.x, color2.y, color2.z,
	-half_edge,  half_edge,  half_edge, color2.x, color2.y, color2.z,
	-half_edge, -half_edge,  half_edge, color2.x, color2.y, color2.z,

	-half_edge,  half_edge,  half_edge, color3.x, color3.y, color3.z,
	-half_edge,  half_edge, -half_edge, color3.x, color3.y, color3.z,
	-half_edge, -half_edge, -half_edge, color3.x, color3.y, color3.z,
	-half_edge, -half_edge, -half_edge, color3.x, color3.y, color3.z,
	-half_edge, -half_edge,  half_edge, color3.x, color3.y, color3.z,
	-half_edge,  half_edge,  half_edge, color3.x, color3.y, color3.z,

	 half_edge,  half_edge,  half_edge, color4.x, color4.y, color4.z,
	 half_edge,  half_edge, -half_edge, color4.x, color4.y, color4.z,
	 half_edge, -half_edge, -half_edge, color4.x, color4.y, color4.z,
	 half_edge, -half_edge, -half_edge, color4.x, color4.y, color4.z,
	 half_edge, -half_edge,  half_edge, color4.x, color4.y, color4.z,
	 half_edge,  half_edge,  half_edge, color4.x, color4.y, color4.z,

	-half_edge, -half_edge, -half_edge, color5.x, color5.y, color5.z,
	 half_edge, -half_edge, -half_edge, color5.x, color5.y, color5.z,
	 half_edge, -half_edge,  half_edge, color5.x, color5.y, color5.z,
	 half_edge, -half_edge,  half_edge, color5.x, color5.y, color5.z,
	-half_edge, -half_edge,  half_edge, color5.x, color5.y, color5.z,
	-half_edge, -half_edge, -half_edge, color5.x, color5.y, color5.z,

	-half_edge,  half_edge, -half_edge, color6.x, color6.y, color6.z,
	 half_edge,  half_edge, -half_edge, color6.x, color6.y, color6.z,
	 half_edge,  half_edge,  half_edge, color6.x, color6.y, color6.z,
	 half_edge,  half_edge,  half_edge, color6.x, color6.y, color6.z,
	-half_edge,  half_edge,  half_edge, color6.x, color6.y, color6.z,
	-half_edge,  half_edge, -half_edge, color6.x, color6.y, color6.z,
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

	//必须先绑定VA0
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

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

	//颜色属性
	//属性位置值为0的顶点属性，有3个值，顶点着色器总参数大小为6个float，位置属性偏移为3个float
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	bool enableDepthTest = true;
	bool enableScale = true;
	bool enableTranslate = true;
	bool enableRotate = true;
	float posDelta = 1.0f;
	bool posDeltaIncreasement = true;
	float scaleDelta = 1.0f;
	bool scaleDeltaIncreasement = true;
	float rotateAngle = 0.0f;

	while (!glfwWindowShouldClose(window)) {
		processInput(window);

		if (enableDepthTest) {
			glEnable(GL_DEPTH_TEST);
		}
		else {
			glDisable(GL_DEPTH_TEST);
		}

		if (enableTranslate) {
			if (posDeltaIncreasement) {
				posDelta = posDelta + 0.01f;
				if (posDelta >= 10) {
					posDeltaIncreasement = false;
				}
			}
			else {
				posDelta = posDelta - 0.01f;
				if (posDelta <= -10) {
					posDeltaIncreasement = true;
				}
			}
		}
		
		if (enableScale) {
			if (scaleDeltaIncreasement) {
				scaleDelta = scaleDelta + 0.001f;
				if (scaleDelta >= 1.5) {
					scaleDeltaIncreasement = false;
				}
			}
			else {
				scaleDelta = scaleDelta - 0.001f;
				if (scaleDelta <= 0.5) {
					scaleDeltaIncreasement = true;
				}
			}
		}

		if (enableRotate) {
			rotateAngle = rotateAngle + 0.002f;
		}

		//创建坐标变换
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);
		//model包含了位移、缩放与旋转操作
		
		model = glm::scale(model, glm::vec3(scaleDelta, scaleDelta, scaleDelta));
		model = glm::translate(model, glm::vec3(posDelta, 0, 0));
		model = glm::rotate(model, rotateAngle, glm::vec3(1.0f, 0.0f, 1.0f));
		view = glm::translate(view, glm::vec3(0.0f, 0.0f, -40.0f));
		projection = glm::perspective(glm::radians(45.0f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);

		//使用着色器程序
		glUseProgram(shaderProgram);

		//获取着色器程序uniform
		unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
		unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
		unsigned int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, &projection[0][0]);
		
		glfwPollEvents();

		//清除屏幕
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//画三角形，第二个参数代表三角形的个数 * 3
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


		//创建imgui
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::Begin("Edit cube", &show_window, ImGuiWindowFlags_MenuBar);
		if (ImGui::Button("enable depth test")) {
			if (enableDepthTest) {
				enableDepthTest = false;
			}
			else {
				enableDepthTest = true;
			}
		}
		if (ImGui::Button("enable scale")) {
			if (enableScale) {
				enableScale = false;
			}
			else {
				enableScale = true;
			}
		}
		if (ImGui::Button("enable translate")) {
			if (enableTranslate) {
				enableTranslate = false;
			}
			else {
				enableTranslate = true;
			}
		}
		if (ImGui::Button("enable rotate")) {
			if (enableRotate) {
				enableRotate= false;
			}
			else {
				enableRotate = true;
			}
		}
		
		ImGui::End();

		ImGui::Render();
		int display_w, display_h;
		glfwMakeContextCurrent(window);
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
	}

	glDeleteVertexArrays(1, &VAO);
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
}