#pragma once

#ifndef OPERATEWINDOW_H
#define OPERATEWINDOW_H

#include"Window.h"

class OperateWindow : public Window {
public:
	OperateWindow() {};
	virtual void RenderWindow(unsigned int textureID = NULL);
};

void OperateWindow::RenderWindow(unsigned int textureID) {
	ImGui::SetNextWindowPos(ImVec2(float(OWWidth + DWWidth), 0), ImGuiCond_None);
	ImGui::SetNextWindowSize(ImVec2(float(OPWWidth), float(OPWHeight)), ImGuiCond_None);
	{
		ImGui::Begin("Operate", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_MenuBar);

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("Setting"))
			{
				if (ImGui::MenuItem("Reset Data"))
				{

				}
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		ImGui::End();
	}
}

#endif // !OPERATEWINDOW_H
