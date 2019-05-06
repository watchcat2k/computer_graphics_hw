#include <iostream>
#include <algorithm>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

CONST INT MAX_LENGTH = 240000;
CONST INT WINDOW_SIZE = 800;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void getLineVertex(int x1, int y1, int x2, int y2, float vertices[MAX_LENGTH], int* length);
void edge_equations(int point1[2], int point2[2], int point3[2], float vertices[MAX_LENGTH], int* length);
void getCircleVertex(int originPointX, int originPointY, int radius, float vertices[MAX_LENGTH], int* length);
float normalize(int input);
void initImGUI(GLFWwindow* window);
void createAndRenderLineImGUI(GLFWwindow* window, bool *show_window, int point1[2], int point2[2], int point3[2], bool *isDrawLine);
void createAndRenderCircleImGUI(GLFWwindow* window, bool *show_window, int point1[2], int *radius, bool *isDrawLine);
int myMin(int a, int b, int c);
int myMax(int a, int b, int c);

//顶点着色器可以向片段着色器传数据
const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";

const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
"}\0";

int main() {
	//初始化opengl窗口和配置
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(WINDOW_SIZE, WINDOW_SIZE, "LearnOpenGL", NULL, NULL);
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

	initImGUI(window);

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
	bool isDrawLine = true;

	int point1[2] = { -150, -150 };
	int point2[2] = { 150, -150 };
	int point3[2] = { 0, 150 };
	int radius = 100;
	int originPoint[2] = { 0, 0 };

	while (!glfwWindowShouldClose(window)) {
		float vertices[MAX_LENGTH] = { 0 }; // 注意索引从0开始! 
		int length = 0;

		//获取线段点数据
		if (isDrawLine) {
			getLineVertex(point1[0], point1[1], point2[0], point2[1], vertices, &length);
			getLineVertex(point2[0], point2[1], point3[0], point3[1], vertices, &length);
			getLineVertex(point1[0], point1[1], point3[0], point3[1], vertices, &length);
			edge_equations(point1, point2, point3, vertices, &length);
		}
		else {
			//获取圆数据
			getCircleVertex(originPoint[0], originPoint[1], radius, vertices, &length);
		}

		unsigned int VAO;
		unsigned int VBO;

		//必须先绑定VA0
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		//再绑定VBO
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		//再设置属性
		//位置属性
		//属性位置值为0的顶点属性
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);


		processInput(window);

		//使用着色器程序
		glUseProgram(shaderProgram);

		glfwPollEvents();

		//清除屏幕
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//画imGUI
		if (isDrawLine) {
			createAndRenderLineImGUI(window, &show_window, point1, point2, point3, &isDrawLine);
		}
		else {
			createAndRenderCircleImGUI(window, &show_window, originPoint, &radius, &isDrawLine);
		}


		//画线段
		//length是vertices的长度，而vertices三个元素为一个点
		//glBindVertexArray(VAO);
		glDrawArrays(GL_POINTS, 0, length / 3);

		glfwSwapBuffers(window);

		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
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

void getLineVertex(int x1, int y1, int x2, int y2, float vertices[MAX_LENGTH], int* length) {
	int x, y, endx, endy;
	float slope;	//斜率
	bool isSlopeExist = true;
	//确定起初点
	if (x1 < x2) {
		x = x1;
		y = y1;
		endx = x2;
		endy = y2;
	}
	else {
		x = x2;
		y = y2;
		endx = x1;
		endy = y1;
	}

	//计算斜率
	if (x == endx) {
		//斜率不存在
		isSlopeExist = false;
	}
	else {
		slope = ((float)endy - (float)y) / ((float)endx - (float)x);
	}

	//初始点
	vertices[*length] = normalize(x);
	(*length)++;
	vertices[*length] = normalize(y);
	(*length)++;
	vertices[*length] = 0.0f;
	(*length)++;

	if (isSlopeExist == false) {
		//斜率不存在时
		int first;
		int last;
		if (y < endy) {
			first = y;
			last = endy;
		}
		else {
			first = endy;
			last = y;
		}
		for (int i = first; i <= last; i++) {
			vertices[*length] = normalize(x);
			(*length)++;
			vertices[*length] = normalize(i);
			(*length)++;
			vertices[*length] = 0.0f;
			(*length)++;
		}
	}
	else {
		//对称变换前，先把起始点移到原点，其它点移到对应位置
		int moveX = x;
		int moveY = y;
		x = x - moveX;
		y = y - moveY;
		endx = endx - moveX;
		endy = endy - moveY;

		//对称变换
		//利用对称变换,将画其他斜率线段转化为画斜率 0 <=  <= 1的线段
		if (slope > 1) {
			//第一象限上半部分
			//起点不动，互换终点的x y
			int temp = endy;
			endy = endx;
			endx = temp;
		}
		else if (slope < 0 && slope >= -1) {
			//第二象限上半部分
			//起点不动，终点的y变相反数
			endy = -endy;
		}
		else if (slope < -1) {
			//第二象限下半部分
			//起点不动，终点的y变相反数，然后互换终点的x y
			endy = -endy;
			int temp = endy;
			endy = endx;
			endx = temp;
		}

		int deltaX = abs(endx - x);
		int deltaY = abs(endy - y);
		int p = 2 * deltaY - deltaX;

		//只能画斜率 0 <=  <= 1的线段 
		while (x <= endx) {
			if (p <= 0) {
				x = x + 1;
				y = y;
				p = p + 2 * deltaY;
			}
			else {
				x = x + 1;
				y = y + 1;
				p = p + 2 * deltaY - 2 * deltaX;
			}

			//利用对称变换,将斜率 0 <=  <= 1的线段上的点转化为原来斜率线段上的点
			if (slope > 1) {
				//第一象限上半部分
				int temp = y;
				y = x;
				x = temp;
			}
			else if (slope < 0 && slope >= -1) {
				//第二象限上半部分
				//起点不动，终点的y变相反数
				y = -y;
			}
			else if (slope < -1) {
				//第二象限下半部分
				//起点不动，互换x y, y变相反数
				int temp = y;
				y = x;
				x = temp;

				y = -y;
			}

			//对称变换后，先把起始点移到原本位置，然后画出来
			x = x + moveX;
			y = y + moveY;

			vertices[*length] = normalize(x);
			(*length)++;
			vertices[*length] = normalize(y);
			(*length)++;
			vertices[*length] = 0.0f;
			(*length)++;

			//画图后,再恢复起始点
			x = x - moveX;
			y = y - moveY;

			//利用对称变换,恢复原来的点到斜率 0 <=  <= 1的线段上，继续计算下一个点
			if (slope > 1) {
				//第一象限上半部分
				int temp = y;
				y = x;
				x = temp;
			}
			else if (slope < 0 && slope >= -1) {
				//第二象限上半部分
				//起点不动，终点的y变相反数
				y = -y;
			}
			else if (slope < -1) {
				//第二象限下半部分
				//起点不动，y变相反数,互换x y
				y = -y;

				int temp = y;
				y = x;
				x = temp;
			}

		}
	}

}

void edge_equations(int point1[2], int point2[2], int point3[2], float vertices[MAX_LENGTH], int* length) {
	//求3条直线一般方程Ax+By+C=0的参数
	//序号12为point1和point2的参数，序号23为point2和point3的参数，序号13为point1和point3的参数
	int A12, B12, C12;
	int A23, B23, C23;
	int A13, B13, C13;
	A12 = point2[1] - point1[1];
	B12 = point1[0] - point2[0];
	C12 = point2[0] * point1[1] - point1[0] * point2[1];
	A23 = point3[1] - point2[1];
	B23 = point2[0] - point3[0];
	C23 = point3[0] * point2[1] - point2[0] * point3[1];
	A13 = point3[1] - point1[1];
	B13 = point1[0] - point3[0];
	C13 = point3[0] * point1[1] - point1[0] * point3[1];

	//将三角形中点带入各直线，若结果小于0，则将参数A、B、C都乘以-1。 
	float middleX = ((float)point1[0] + (float)point2[0] + (float)point3[0]) / 3;
	float middleY = ((float)point1[1] + (float)point2[1] + (float)point3[1]) / 3;
	float temp = A12 * middleX + B12 * middleY + C12;
	if (temp < 0) {
		A12 = -A12;
		B12 = -B12;
		C12 = -C12;
	}
	temp = A23 * middleX + B23 * middleY + C23;
	if (temp < 0) {
		A23 = -A23;
		B23 = -B23;
		C23 = -C23;
	}
	temp = A13 * middleX + B13 * middleY + C13;
	if (temp < 0) {
		A13 = -A13;
		B13 = -B13;
		C13 = -C13;
	}

	//计算两条边分别于x轴和y轴平行的外接矩形
	int leftX = myMin(point1[0], point2[0], point3[0]);
	int rightX = myMax(point1[0], point2[0], point3[0]);
	int topY = myMax(point1[1], point2[1], point3[1]);
	int bottomY = myMin(point1[1], point2[1], point3[1]);

	//遍历矩形，处理后的一般方程，三角形中的任意一个点代入3条曲线，都会使Ax+By+C的结果都大于0
	for (int i = leftX; i <= rightX; i = i + 1) {
		for (int j = bottomY; j <= topY; j = j + 1) {
			if (A12 * i + B12 * j + C12 > 0 &&
				A23 * i + B23 * j + C23 > 0 &&
				A13 * i + B13 * j + C13 > 0) {
				//防止数组越界
				if ((*length) >= MAX_LENGTH - 3) break;

				vertices[(*length)++] = normalize(i);
				vertices[(*length)++] = normalize(j);
				vertices[(*length)++] = 0.0f;	
			}
		}
	}
}

void getCircleVertex(int originPointX, int originPointY, int radius, float vertices[MAX_LENGTH], int* length) {
	//移到原点，记录偏移量
	int moveX = originPointX;
	int moveY = originPointY;

	int x = 0;
	int y = radius;
	int end = (int)(radius * 1.0 / (float)sqrt(2));
	float d = 1.25 - radius;

	//加入初始点及对称的其它7个点
	while (x <= end) {
		//加入新点及对称的其它7个点
		vertices[(*length)++] = normalize(x + moveX);
		vertices[(*length)++] = normalize(y + moveY);
		vertices[(*length)++] = 0.0f;
		vertices[(*length)++] = normalize(y + moveX);
		vertices[(*length)++] = normalize(-x + moveY);
		vertices[(*length)++] = 0.0f;
		vertices[(*length)++] = normalize(-x + moveX);
		vertices[(*length)++] = normalize(-y + moveY);
		vertices[(*length)++] = 0.0f;
		vertices[(*length)++] = normalize(-y + moveX);
		vertices[(*length)++] = normalize(x + moveY);
		vertices[(*length)++] = 0.0f;
		vertices[(*length)++] = normalize(y + moveX);
		vertices[(*length)++] = normalize(x + moveY);
		vertices[(*length)++] = 0.0f;
		vertices[(*length)++] = normalize(x + moveX);
		vertices[(*length)++] = normalize(-y + moveY);
		vertices[(*length)++] = 0.0f;
		vertices[(*length)++] = normalize(-y + moveX);
		vertices[(*length)++] = normalize(-x + moveY);
		vertices[(*length)++] = 0.0f;
		vertices[(*length)++] = normalize(-x + moveX);
		vertices[(*length)++] = normalize(y + moveY);
		vertices[(*length)++] = 0.0f;	
		vertices[(*length)++] = normalize(x + moveX);
		vertices[(*length)++] = normalize(y + moveY);
		vertices[(*length)++] = 0.0f;
		
		
		if (d <= 0) {
			x = x + 1;
			y = y;
			d = d + 2 * x + 2;	
		}
		else {
			x = x + 1;
			y = y - 1;
			d = d + 2 * (x - y) + 5;
		}
	}
}

float normalize(int input) {
	float result = float(input) / WINDOW_SIZE;
	return result;
}

void initImGUI(GLFWwindow* window) {
	//创建并绑定ImGui
	const char* glsl_version = "#version 130";
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);
}

void createAndRenderLineImGUI(GLFWwindow* window, bool *show_window, int point1[2], int point2[2], int point3[2], bool *isDrawLine) {
	//创建imgui
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Edit points", show_window, ImGuiWindowFlags_MenuBar);
	//ImGui::ColorEdit3("top color", (float*)&topColor);
	ImGui::Text("input 3 points");
	ImGui::SliderInt2("point1", point1, -180, 180);
	ImGui::SliderInt2("point2", point2, -180, 180);
	ImGui::SliderInt2("point3", point3, -180, 180);
	if (ImGui::Button("Draw Circle"))
		(*isDrawLine) = false;
	ImGui::End();

	ImGui::Render();
	int display_w, display_h;
	glfwMakeContextCurrent(window);
	glfwGetFramebufferSize(window, &display_w, &display_h);
	glViewport(0, 0, display_w, display_h);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void createAndRenderCircleImGUI(GLFWwindow* window, bool *show_window, int point[2], int *radius, bool *isDrawLine) {
	//创建imgui
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Edit points", show_window, ImGuiWindowFlags_MenuBar);
	ImGui::Text("input 3 points");
	ImGui::SliderInt2("point1", point, -WINDOW_SIZE, WINDOW_SIZE);
	ImGui::SliderInt("point1", radius, 0, WINDOW_SIZE);
	if (ImGui::Button("Draw Line"))
		(*isDrawLine) = true;
	ImGui::End();

	ImGui::Render();
	int display_w, display_h;
	glfwMakeContextCurrent(window);
	glfwGetFramebufferSize(window, &display_w, &display_h);
	glViewport(0, 0, display_w, display_h);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

int myMin(int a, int b, int c) {
	if (a < b) {
		if (a < c) {
			return a;
		}
		else {
			return c;
		}
	}
	else {
		if (b < c) {
			return b;
		}
		else {
			return c;
		}
	}
}

int myMax(int a, int b, int c) {
	if (a > b) {
		if (a > c) {
			return a;
		}
		else {
			return c;
		}
	}
	else {
		if (b > c) {
			return b;
		}
		else {
			return c;
		}
	}
}