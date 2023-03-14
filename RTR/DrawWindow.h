#pragma once

#ifndef DRAWWINDOW_H
#define DRAWWINDOW_H

#include"Window.h"	

//#include<unordered_map>
// extern 别处定义 此处引用
//extern std::unordered_map<std::string, unsigned int> TexMap;

class DrawWindow : public Window {
public:
	DrawWindow() {};

	virtual void RenderWindow(unsigned int textureID = NULL);
private:
};

void DrawWindow::RenderWindow(unsigned int textureID) {
	ImGui::SetNextWindowPos(ImVec2(float(OWWidth), 0), ImGuiCond_None);
	ImGui::SetNextWindowSize(ImVec2(float(DWWidth), float(DWHeight)), ImGuiCond_None);
	{
		ImGui::Begin("Draw", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_MenuBar);

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Load Texture"))
				{

				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("View"))
			{
				if (ImGui::MenuItem("All Texture"))
				{

				}
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}
		
		//ImGui::Image((void*)(intptr_t)textureID, ImVec2(float(DWWidth - 20), float(DWHeight - 40)));
		ImGui::Image((void*)(intptr_t)textureID, ImGui::GetContentRegionAvail(),ImVec2(0,1),ImVec2(1,0));

		ImGui::End();
	}
}

#endif // !DRAWWINDOW_H
