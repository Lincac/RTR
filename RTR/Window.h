#pragma once

#ifndef  WINDOW_H
#define WINDOW_H

#include<glad/glad.h>
#include"imgui/imgui.h"
#include"imgui/imgui_impl_glfw.h"
#include"imgui/imgui_impl_opengl3.h"
#include"Forward.h"
#include"Defered.h"

#include <windows.h>  
#include <commdlg.h> 

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
std::string Window::renderModeName = "BlinPhone";
std::string Window::renderPathName = "Forward";

std::string TCHAR2STRING(TCHAR* STR)
{
	int iLen = WideCharToMultiByte(CP_ACP, 0, STR, -1, NULL, 0, NULL, NULL);
	char* chRtn = new char[iLen * sizeof(char)];
	WideCharToMultiByte(CP_ACP, 0, STR, -1, chRtn, iLen, NULL, NULL);
	std::string str(chRtn);
	delete chRtn;
	return str;
}

std::string OpenFileDialog() {
	OPENFILENAME ofn;      // 公共对话框结构。     
	TCHAR szFile[MAX_PATH]; // 保存获取文件名称的缓冲区。               
	// 初始化选择文件对话框。     
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = NULL;
	ofn.lpstrFile = szFile;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = L"PNG(*.png)\0*.png\0JPG(*.jpg)\0*.jpg";//(LPCWSTR)"All(*.*)\0*.*\0Text(*.txt)\0*.TXT\0\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	//ofn.lpTemplateName =  MAKEINTRESOURCE(ID_TEMP_DIALOG);    
	// 显示打开选择文件对话框。

	if (GetOpenFileName(&ofn))
	{
		//显示选择的文件。 
		std::cout << "Load File : " << TCHAR2STRING(szFile) << std::endl;
	}

	return TCHAR2STRING(szFile);
}

#endif // ! WINDOW_H
