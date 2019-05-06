#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

//ʹ���˹ٷ���shader���ļ������Ը�����ز���shader
//ע�⣬ʹ�ùٷ������shadderʱ���Լ�д�Ķ����Ƭ����ɫ�������������д���ļ�
//Ȼ����ļ�·������shader.h�ṩ�Ĺ��캯��
#include "shader.h"  
#include "camera.h"

using namespace std;

const int WINDOW_WIDTH = 1000;
const int WINDOW_HEIGHT = 800;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

//camera��λ������������view�����෴
Camera camera(glm::vec3(0.0f, 0.0f, 10.0f));
//���λ��
float mouseX = 0.0f;
float mouseY = 0.0f;
bool firstMouse = true;
//ÿһ֡���ʱ��
float deltaTime = 0.0f;
//��һ֡��ʱ��
float lastFrame = 0.0f;

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
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	//��Ȳ���
	glEnable(GL_DEPTH_TEST);

	//��������ImGui
	const char* glsl_version = "#version 130";
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	//������߳�Ϊ2.0f*2s
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

	unsigned int indices[] = { // ע��������0��ʼ! 
		0, 1, 2, // ��һ��������
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

	//���壨�����壩��VAO
	unsigned int objectVAO;
	unsigned int VBO;
	unsigned int EBO;

	//�����Ȱ�VA0
	glGenVertexArrays(1, &objectVAO);
	glBindVertexArray(objectVAO);

	//�ٰ�VBO
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//ʹ��EBO����������Σ���ϳ�����ͼ��
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	
	//����������
	//λ������
	//����λ��ֵΪ0�Ķ������ԣ���3��ֵ��������ɫ���ܲ�����СΪ6��float��λ������ƫ��Ϊ0
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//����������
	//����λ��ֵΪ0�Ķ������ԣ���3��ֵ��������ɫ���ܲ�����СΪ6��float��λ������ƫ��Ϊ3��float
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//�ƹ�VAO
	unsigned int lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	//������ɫ���ܲ�����СΪ6��float����Ϊ����ǰ���objectVAO����һ��
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
			//����glfw��ȡ���ǵ����
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
		else {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
	
		//���㵱ǰʱ���֡���ʱ�䣬Ϊ��ʹ�ƶ���ƽ��
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);

		//�����Ļ
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//��������任
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);
		
		if (enable_camera) {
			view = camera.GetViewMatrix();
			viewPos = camera.Position;
			
			//���ĵ�һ������������fov��ֵ������ʾ������Ұ�����������˹۲�ռ�Ĵ�С���ڶ������������˿�߱ȡ������͵��ĸ�����������ƽ��ͷ��Ľ���Զƽ��
			//glm::radians()�ǰѽǶ�תΪ����
			//����������
			projection = glm::perspective(glm::radians(camera.Zoom), perspective_division, perspective_near, perspective_far);

		}
		else {
			//�ѳ����ƶ�(-1.0f, -1.0f, -5.0f)
			view = glm::translate(view, glm::vec3(0.0f, 0.0f, -10.0f));
			viewPos = glm::vec3(1.0f, 1.0f, 5.0f);
			
			projection = glm::perspective(glm::radians(perspective_fov), perspective_division, perspective_near, perspective_far);
		}

		//�ƹ������ƶ�
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

		//���ùٷ���shader.h������ɫ��
		if (is_phong_shader) {
			Shader objectShader("objectVertexPhongShader.vs", "objectFragmentPhongShader.fs");
			//ʹ����ɫ������
			objectShader.use();
			//������ɫ������uniform
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
			//model������λ�ơ���������ת����
			model = glm::mat4(1.0f);
			model = glm::rotate(model, rotateAngle, glm::vec3(0.0f, 1.0f, 0.0f));
			objectShader.setMat4("model", model);

			//������
			glBindVertexArray(objectVAO);
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		}
		else if (is_gouraud_shader) {
			Shader objectShader("objectVertexGouraudShader.vs", "objectFragmentGouraudShader.fs");
			//ʹ����ɫ������
			objectShader.use();
			//������ɫ������uniform
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
			//model������λ�ơ���������ת����
			model = glm::mat4(1.0f);
			model = glm::rotate(model, rotateAngle, glm::vec3(0.0f, 1.0f, 0.0f));
			objectShader.setMat4("model", model);

			//������
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

		//���ƹ�
		glBindVertexArray(lightVAO);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

		//����imgui
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

//����ƶ�
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

//������
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	camera.ProcessMouseScroll(yoffset);
}