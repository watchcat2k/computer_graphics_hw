#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

//顶点着色器可以向片段着色器传数据
const char* vertexShaderSource = "#version 330 core\n"
	"layout (location = 0) in vec3 aPos;\n"
	"layout (location = 1) in vec3 aColor;\n"
	"out vec3 ourColor;\n"
	"void main()\n"
	"{\n"
	"gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
	"ourColor = aColor;\n"
	"}\0";

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
	
	GLFWwindow* window = glfwCreateWindow(1000, 800, "LearnOpenGL", NULL, NULL);
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

	/*
	float vertices[] = {
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		0.0f, 0.5f, 0.0f
	};
	*/

	//颜色数据
	ImVec4 topColor = ImVec4(0.0f, 0.0f, 1.0f, 1.0f);
	ImVec4 leftColor = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
	ImVec4 rightColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);

	bool show_window = true;

	unsigned int VAO;
	unsigned int VBO;
	unsigned int EBO;

	while (!glfwWindowShouldClose(window)) {
		float vertices[] = { // 注意索引从0开始! 
		//位置				颜色
		0.5f, -0.5f, 0.0f, rightColor.x, rightColor.y, rightColor.z,
		-0.5f, -0.5f, 0.0f, leftColor.x, leftColor.y, leftColor.z,
		0.0f, 0.5f, 0.0f, topColor.x, topColor.y, topColor.z,
		1.0f, -0.5f, 0.0f, rightColor.x, rightColor.y, rightColor.z,
		0.5f, -0.5f, 0.0f, leftColor.x, leftColor.y, leftColor.z,
		0.75f, 0.25f, 0.0f, topColor.x, topColor.y, topColor.z,
		-0.75f, 0.5f, 0.0f, rightColor.x, rightColor.y, rightColor.z,
		-0.75f, -0.5f, 0.0f, leftColor.x, leftColor.y, leftColor.z,
		};

		unsigned int indices[] = { // 注意索引从0开始! 
			0, 1, 2, // 第一个三角形
			3, 4, 5,
			6, 7, 8
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
		//属性位置值为0的顶点属性
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		//颜色属性
		//属性位置值为1的顶点属性。颜色值有3个float那么大
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		processInput(window);

		//使用着色器程序
		glUseProgram(shaderProgram);

		glfwPollEvents();

		//清除屏幕
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//画三角形
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		//画线段,偏移6个点，画2点之间的线段
		glDrawArrays(GL_LINES, 6, 2);

		//创建imgui
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::Begin("Edit color", &show_window, ImGuiWindowFlags_MenuBar);
		ImGui::ColorEdit3("top color", (float*)&topColor);
		ImGui::ColorEdit3("left color", (float*)&leftColor);
		ImGui::ColorEdit3("right color", (float*)&rightColor);
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