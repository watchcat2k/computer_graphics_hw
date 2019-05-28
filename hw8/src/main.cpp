#include <iostream>
#include <cmath>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

// ��������Ŀ�
#include "stb_image.h"

//ʹ���˹ٷ���shader���ļ������Ը�����ز���shader
//ע�⣬ʹ�ùٷ������shadderʱ���Լ�д�Ķ����Ƭ����ɫ�������������д���ļ�
//Ȼ����ļ�·������shader.h�ṩ�Ĺ��캯��
#include "shader.h"  

using namespace std;

const int WINDOW_WIDTH = 1000;
const int WINDOW_HEIGHT = 800;
const int MAX_VERTEX_LEN = 300;
const int MAX_INDICES_LEN = 600;
const int MAX_BEZIER_VERTEX_LEN = 3000;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void cursor_position_callback(GLFWwindow* window, double x, double y);
int getBezierVertex(float lineVertices[MAX_VERTEX_LEN],int lineVertexLen, float bezierVertices[MAX_BEZIER_VERTEX_LEN]);

// �������ȫ��λ�ñ���
float mouseXPos, mouseYPos;

// ����ȫ�ֶ������, �����Ϊ vertexLen / 3
int lineVertexLen = 0;
float lineVertices[MAX_VERTEX_LEN] = {
	//λ��
	//-0.5f, 0.5f, 0.0f,
	//0.5f, 0.5f, 0.0f,
	//0.5f, -0.5f, 0.0f
};
int lineIndicesLen = 0;
unsigned int lineIndices[MAX_INDICES_LEN] = { // ע��������0��ʼ! 
	//0, 1, // ��һ���߶�
	//1, 2
};


int main() {
	//��ʼ��opengl���ں�����
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
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// ��Ȳ���
	glEnable(GL_DEPTH_TEST);

	//��������ImGui
	const char* glsl_version = "#version 130";
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	Shader lineShader("vertexShader.vs", "lineFragmentShader.fs");
	Shader bezierShader("vertexShader.vs", "bezierFragmentShader.fs");

	bool show_window = true;

	while (!glfwWindowShouldClose(window)) {
		processInput(window);

		//�����Ļ
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		unsigned int VAO;
		unsigned int VBO;
		unsigned int EBO;

		//�����Ȱ�VA0
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		//�ٰ�VBO
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), lineVertices, GL_STATIC_DRAW);

		//ʹ��EBO������߶Σ���ϳ�����ͼ��
		glGenBuffers(1, &EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(lineIndices), lineIndices, GL_STATIC_DRAW);

		//����������
		//λ������
		//����λ��ֵΪ0�Ķ�������
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glEnableVertexAttribArray(1);

		// ʹ����ɫ������
		lineShader.use();

		//���߶Σ�
		glBindVertexArray(VAO);
		// ��Ϊͨ�� indices ȷ���߶Σ�ÿ����ȷ��һ�߶Σ����Ի������߶Σ��������߶�������2
		if (lineIndicesLen >= 2) {
			glDrawElements(GL_LINES, lineIndicesLen, GL_UNSIGNED_INT, 0);
		}


		// ������ bezier ������
		// ���� bezier ������� 
		int bezierVertexLen = 0;
		float bezierVertices[MAX_BEZIER_VERTEX_LEN] = {
			//-0.5f, 0.5f, 0.0f,
		};

		bezierVertexLen = getBezierVertex(lineVertices, lineVertexLen, bezierVertices);

		unsigned int bezierVAO;
		unsigned int bezierVBO;

		//�����Ȱ�VA0
		glGenVertexArrays(1, &bezierVAO);
		glBindVertexArray(bezierVAO);

		//�ٰ�VBO
		glGenBuffers(1, &bezierVBO);
		glBindBuffer(GL_ARRAY_BUFFER, bezierVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(bezierVertices), bezierVertices, GL_STATIC_DRAW);

		//����������
		//λ������
		//����λ��ֵΪ0�Ķ�������
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glEnableVertexAttribArray(1);

		// ʹ����ɫ������
		bezierShader.use();

		//���߶Σ�
		glBindVertexArray(bezierVAO);
		glDrawArrays(GL_POINTS, 0, bezierVertexLen / 3);

		//����imgui
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::Begin("Edit cube", &show_window, ImGuiWindowFlags_MenuBar);


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

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) { 
	if (action == GLFW_PRESS) switch (button) { 
		case GLFW_MOUSE_BUTTON_LEFT:			
			// ÿ�������㻭һ��ֱ��
			// ������ĵ�
			lineVertices[lineVertexLen] = mouseXPos;
			lineVertexLen++;
			lineVertices[lineVertexLen] = mouseYPos;
			lineVertexLen++;
			lineVertices[lineVertexLen] = 0.0f;
			lineVertexLen++;
			// �������,ǰһ����Ҳ�µĵ�һ��ȷ�����߶�
			if (lineIndicesLen >= 2) {
				lineIndices[lineIndicesLen] = lineIndices[lineIndicesLen - 1];
				lineIndicesLen++;
				lineIndices[lineIndicesLen] = lineIndices[lineIndicesLen - 1] + 1;
				lineIndicesLen++;
			}
			else {
				lineIndices[lineIndicesLen] = lineIndicesLen;
				lineIndicesLen++;
			}
			break;
		case GLFW_MOUSE_BUTTON_RIGHT:
			if (lineVertexLen >= 3) {
				lineVertexLen = lineVertexLen - 3;
			}

			if (lineIndicesLen >= 4) {
				lineIndicesLen = lineIndicesLen - 2;
			}
			else {
				lineIndicesLen = lineIndicesLen - 1;
			}

			break;
		default:				
			break;
	}	
	return; 
}

void cursor_position_callback(GLFWwindow* window, double x, double y) { 
	mouseXPos = float((x - WINDOW_WIDTH / 2) / WINDOW_WIDTH) * 2;
	mouseYPos = float(0 - (y - WINDOW_HEIGHT / 2) / WINDOW_HEIGHT) * 2;
	return; 
}

int getBezierVertex(float lineVertices[MAX_VERTEX_LEN], int lineVertexLen, float bezierVertices[MAX_BEZIER_VERTEX_LEN]) {
	int bezierVertexLen = 0;
	if (lineVertexLen == 0) return bezierVertexLen;
	else if (lineVertexLen == 3) {
		bezierVertices[bezierVertexLen] = lineVertices[0];
		bezierVertexLen++;
		bezierVertices[bezierVertexLen] = lineVertices[1];
		bezierVertexLen++;
		bezierVertices[bezierVertexLen] = lineVertices[2];
		bezierVertexLen++;
	}
	else {
		for (float t = 0.000f; t <= 1.000f; t = t + 0.001f) {
			double new_xPos = 0, new_yPos = 0;
			for (int index = 0; index < lineVertexLen / 3; index++) {
				// x����
				new_xPos += lineVertices[index * 3] * pow(t, index) * pow((1 - t), (lineVertexLen / 3 - 1 - index));
				// y����
				new_yPos += lineVertices[index * 3 + 1] * pow(t, index) * pow((1 - t), (lineVertexLen / 3 - 1 - index));
			}
			bezierVertices[bezierVertexLen] = new_xPos;
			bezierVertexLen++;
			bezierVertices[bezierVertexLen] = new_yPos;
			bezierVertexLen++;
			bezierVertices[bezierVertexLen] = 0.0f;
			bezierVertexLen++;
		}
	}
	return bezierVertexLen;
}

