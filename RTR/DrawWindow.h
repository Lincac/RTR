#pragma once

#ifndef DRAWWINDOW_H
#define DRAWWINDOW_H

#include"Window.h"	

//#include<unordered_map>
// extern 别处定义 此处引用
//extern std::unordered_map<std::string, unsigned int> TexMap;

class DrawWindow : public Window {
public:
	DrawWindow() {
		renderMode = std::make_shared<PBR>();
		renderPath = std::make_shared<Forward>(renderMode);
	};

	virtual ~DrawWindow() override {};
	virtual void RenderWindow(std::shared_ptr<Objects> objs, unsigned int textureID = NULL) override;
	virtual std::shared_ptr<RenderPath> GetRenderPath()override { return renderPath; };
private:
	std::shared_ptr<RenderMode> renderMode;
	std::shared_ptr<RenderPath> renderPath;
};

void DrawWindow::RenderWindow(std::shared_ptr<Objects> objs, unsigned int textureID) {
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

			if (ImGui::BeginMenu("RenderPath"))
			{
				if (ImGui::MenuItem("Forward") && renderPathName != "Forward")
					renderPath = std::make_shared<Forward>(renderMode);
				else if (ImGui::MenuItem("Defered") && renderPathName != "Defered") 
					renderPath = std::make_shared<Defered>(renderMode);

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("RenderMode"))
			{
				if (ImGui::MenuItem("PBR") && renderModeName != "PBR")
				{
					renderMode = std::make_shared<PBR>();
					renderPath->SetRenderMode(renderMode);
				}
				else if (ImGui::MenuItem("BlinPhone") && renderModeName != "BlinPhone")
				{
					renderMode = std::make_shared<BlinPhone>();
					renderPath->SetRenderMode(renderMode);
				}
				else if (ImGui::MenuItem("Cartoon") && renderModeName != "Cartoon") {

				}

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}
		
		ImGui::Image((void*)(intptr_t)textureID, ImGui::GetContentRegionAvail(),ImVec2(0,1),ImVec2(1,0));

		ImGui::End();
	}
}

#endif // !DRAWWINDOW_H
