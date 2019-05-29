#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "shader.h"

#include <iostream>
#include <vector>
#include <math.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods); void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void cursor_position_callback(GLFWwindow* window, double x, double y);
void getBezierVertices();

float mousePosX = 0;
float mousePosY = 0;
float* interval;

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

std::vector<float> points;
std::vector<float> linepoints;

int main()
{
	// glfw: initialize and configure
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

	// glfw window creation
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);

	// glad: load all OpenGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// build and compile our shader program
	Shader ourShader("h8shader.vs", "h8shader.fs"); // you can name your shader files however you like

	// set up vertex data (and buffer(s)) and configure vertex attributes
	unsigned int VBOs[2], VAOs[2];
	glGenVertexArrays(2, VAOs);
	glGenBuffers(2, VBOs);
	glBindVertexArray(VAOs[0]);
	glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
	glBindVertexArray(VAOs[1]);
	glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);

	interval = new float[21];
	for (int i = 0; i <= 20; i++)
		interval[i] = i * 0.05;

	// render loop
	while (!glfwWindowShouldClose(window))
	{
		// input
		processInput(window);

		// render
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		// 设置点的大小
		glad_glPointSize(10);
		glClear(GL_COLOR_BUFFER_BIT);

		glDeleteVertexArrays(2, VAOs);
		glDeleteBuffers(2, VBOs);
		
		float* pointsTemp = new float[points.size()];
		if (points.size() > 0) {
			for (int i = 0; i < points.size(); i++)
				pointsTemp[i] = points[i];
		}

		glGenVertexArrays(2, VAOs);
		glGenBuffers(2, VBOs);

		glBindVertexArray(VAOs[0]);
		glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
		glBufferData(GL_ARRAY_BUFFER, points.size()*4, pointsTemp, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		// render the points
		ourShader.use();
		ourShader.setBool("red", false);
		glBindVertexArray(VAOs[0]);
		if (points.size() > 0) {
			if (points.size() == 2) {
				glDrawArrays(GL_POINTS, 0, 1);
			}
			else {
				glDrawArrays(GL_POINTS, 0, points.size() / 2);
				for (int i = 0; i < points.size() /2 - 1; i++)
					glDrawArrays(GL_LINES, i, 2);
					
			}
		}
		delete[] pointsTemp;

		
		getBezierVertices();

		float* linps = new float[linepoints.size()];
		for (int i = 0; i < linepoints.size(); i++) {
			linps[i] = linepoints[i];
		}
		ourShader.use();
		ourShader.setBool("red", true);
		glBindVertexArray(VAOs[1]);
		glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
		glBufferData(GL_ARRAY_BUFFER, linepoints.size() * 4, linps, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glBindVertexArray(VAOs[1]);

		if (linepoints.size() > 0) {
			for (int i = 0; i < linepoints.size() / 2 - 1; i++)
				glDrawArrays(GL_LINES, i, 2);
				
		}

		delete[] linps;
		

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	glDeleteVertexArrays(2, VAOs);
	glDeleteBuffers(2, VBOs);

	// glfw: terminate, clearing all previously allocated GLFW resources.
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	if (action == GLFW_RELEASE) {
		if (button == GLFW_MOUSE_BUTTON_LEFT) {
			points.push_back(mousePosX);
			points.push_back(mousePosY);
		}
		if (button == GLFW_MOUSE_BUTTON_RIGHT) {
			if (points.size() > 0) {
				points.pop_back();
				points.pop_back();
			}
		}
	}
}

void cursor_position_callback(GLFWwindow* window, double x, double y) {
	mousePosX = float((x - SCR_WIDTH / 2) / SCR_WIDTH) * 2;
	mousePosY = float(0 - (y - SCR_HEIGHT / 2) / SCR_HEIGHT) * 2;
}

int factorial(int n) {
	if (n == 0)
		return 1;
	return (n * (factorial(n - 1)));
}

void getBezierVertices() {
	if (points.size() > 2) {
		linepoints.clear();
		for (float t = 0.0f; t <= 1.0f; t = t + 0.01f) {
			float xsum = 0;
			float ysum = 0;
			int n = points.size() / 2 - 1;
			for (int j = 0; j < points.size() / 2; j++) {
				float coefficient = (factorial(n) / (factorial(j)*factorial(n - j)))
					* pow(t, j) * pow(1 - t, n - j);
				xsum += coefficient * points[j * 2];
				ysum += coefficient * points[j * 2 + 1];
			}
			linepoints.push_back(xsum);
			linepoints.push_back(ysum);
		}
	}
}