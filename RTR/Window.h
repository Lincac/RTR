#pragma once

#ifndef  WINDOW_H
#define WINDOW_H

#include<glad/glad.h>
#include"imgui/imgui.h"
#include"imgui/imgui_impl_glfw.h"
#include"imgui/imgui_impl_opengl3.h"
#include"Message.h"

class Window {
public:
	virtual ~Window() {}; // 虚继承，鼓励子类实现，让继承类释放自身资源
	static const unsigned int scr_WIDTH = 1600,scr_HEIGHT = 900;
	unsigned int OWWidth = 350,OWHeight = 900;
	static const unsigned int DWWidth = 900, DWHeight = 900;
	unsigned int OPWWidth = 350, OPWHeight = 900;
	virtual void RenderWindow(unsigned int textureID = NULL) = 0;
	virtual Message SendMessage() = 0;
	virtual void ReciveMessage(Message message) = 0;
};
#endif // ! WINDOW_H
