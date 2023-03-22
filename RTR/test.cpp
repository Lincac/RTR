/*
	3.1 ~ 3.3 ：基本完成项目骨架，但类之间的通信尚未完成
	3.4 ~ 3.5 ：基本完成类之间通信模板，并完善了部分框架内容
	3.6 : 完成了全部代码框架，全部完善所有类架构
	3.7 : 完成简单场景测试
	3.8~3.9 :  完成BlinPhone模型测试
	3.13~3.14 : 完成前向渲染和延迟渲染场景
	3.15 : 添加了延迟渲染TAA
	3.20: SSR完成！
	准备界面完善！！！
*/
#include"Program.h"

Program program;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

bool mouseClick = false;
float lastx = 0, lasty = 0;
float currentx = 0, currenty = 0;

void processInput(GLFWwindow* window);
void mouse_call_back(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(scr_WIDTH, scr_HEIGHT, "RTR", NULL, NULL);

	if (window == NULL)
	{
		std::cout << "error to create window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetCursorPosCallback(window, mouse_call_back);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetScrollCallback(window, scroll_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "error to load glad" << std::endl;
		glfwTerminate();
		return -1;
	} 

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	program = Program(window);
	program.Init();

	preview = camera.GetViewMatrix();
	preprojection = glm::perspective(glm::radians(camera.Zoom), (float)DWWidth / (float)DWHeight, camera.nearplane, camera.farplane);

	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = float(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		view = camera.GetViewMatrix();
		projection = glm::perspective(glm::radians(camera.Zoom), (float)DWWidth / (float)DWHeight, camera.nearplane, camera.farplane);

		processInput(window);

		program.Render();

		preview = view;
		preprojection = projection;
		offsetindex++;


		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();

	return 0;
}
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}
void mouse_call_back(GLFWwindow* window, double xpos, double ypos) {

	currentx = scr_WIDTH - static_cast<float>(xpos);
	currenty = static_cast<float>(ypos);

	if (mouseClick)
	{
		float deltax = currentx - lastx;
		float deltay = currenty - lasty;

		camera.ProcessMouseMovement(deltax, deltay);
	}
	lastx = currentx;
	lasty = currenty;
}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(float(yoffset));
}
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	if (!windowFocus && button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && OWWidth <= currentx && currentx <= (OWWidth + DWWidth))
		mouseClick = true;
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
		mouseClick = false;
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, scr_WIDTH, scr_HEIGHT);
}
