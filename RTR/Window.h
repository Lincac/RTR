#pragma once

#ifndef  WINDOW_H
#define WINDOW_H

#include<glad/glad.h>
#include"imgui/imgui.h"
#include"imgui/imgui_impl_glfw.h"
#include"imgui/imgui_impl_opengl3.h"
#include"Forward.h"
#include"Defered.h"

static const unsigned int scr_WIDTH = 1600, scr_HEIGHT = 900;
static const unsigned int OWWidth = 350, OWHeight = 900;
static const unsigned int DWWidth = 900, DWHeight = 900;
static const unsigned int OPWWidth = 350, OPWHeight = 900;

bool windowFocus = false;

class Window {
public:
	virtual ~Window() {}; // 虚继承，鼓励子类实现，让继承类释放自身资源
	virtual void RenderWindow(std::shared_ptr<Objects> objs,unsigned int textureID = NULL) = 0;
	virtual std::shared_ptr<RenderPath> GetRenderPath() { return nullptr; };

protected:
	static int ObjID;
	static std::string renderModeName;
	static std::string renderPathName;
};
int Window::ObjID = 0;
std::string Window::renderModeName = "PBR";
std::string Window::renderPathName = "Forward";
#endif // ! WINDOW_H
