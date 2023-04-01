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
		renderMode = std::make_shared<BlinPhone>();
		renderPath = std::make_shared<Forward>(renderMode);

		OpenTextureView = false;
	};

	virtual ~DrawWindow() override {};
	virtual void RenderWindow(std::shared_ptr<Objects> objs, unsigned int textureID = NULL) override;
	virtual std::shared_ptr<RenderPath> GetRenderPath()override { return renderPath; };
private:
	std::shared_ptr<RenderMode> renderMode;
	std::shared_ptr<RenderPath> renderPath;

	bool OpenTextureView;
	void TextureViewWindow();
};

void DrawWindow::RenderWindow(std::shared_ptr<Objects> objs, unsigned int textureID) {
	ImGui::SetNextWindowPos(ImVec2(float(OWWidth), 0), ImGuiCond_None);
	ImGui::SetNextWindowSize(ImVec2(float(DWWidth), float(DWHeight)), ImGuiCond_None);
	{
		ImGui::Begin("Draw", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_MenuBar);

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("View"))
			{
				if (ImGui::MenuItem("View Texture"))
				{
					OpenTextureView = true;
				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("RenderPath"))
			{
				if (ImGui::MenuItem("Forward") && renderPathName != "Forward") {
					renderPathName = "Forward";
					renderPath = std::make_shared<Forward>(renderMode);
				}
				else if (ImGui::MenuItem("Defered") && renderPathName != "Defered") 
				{
					renderPathName = "Defered";
					renderPath = std::make_shared<Defered>(renderMode);
				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("RenderMode"))
			{
				if (ImGui::MenuItem("PBR") && renderModeName != "PBR")
				{
					renderMode = std::make_shared<PBR>();
					renderModeName = renderMode->getRenderModeName();
					renderPath->SetRenderMode(renderMode);
				}
				else if (ImGui::MenuItem("BlinPhone") && renderModeName != "BlinPhone")
				{
					renderMode = std::make_shared<BlinPhone>();
					renderModeName = renderMode->getRenderModeName();
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

	if (OpenTextureView)
	{
		TextureViewWindow();
	}
}

void DrawWindow::TextureViewWindow() {
	ImGui::SetNextWindowSize(ImVec2(500, 440), ImGuiCond_FirstUseEver);
	if (ImGui::Begin("Texture View", &OpenTextureView, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoCollapse))
	{
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Load Texture"))
				{
					std::string path = OpenFileDialog();
					if (path != "")
					{
						std::string sub = path.substr(path.find_last_of('\\') + 1);
						std::string name = sub.substr(0, sub.find('.'));
						unsigned int texID = LoadTexture(path.c_str());
						ChartletMap.emplace(name, texID);
					}
				}

				if (ImGui::MenuItem("Close")) {
					OpenTextureView = false;
					windowFocus = false;
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		// Left
		static std::string selected = "";
		{
			ImGui::BeginChild("left pane", ImVec2(150, 0), true);
			for (const auto& temp : ChartletMap) {
				std::string TexName = temp.first;
				if (ImGui::Selectable(TexName.c_str())) selected = TexName;
			}
			ImGui::EndChild();
		}
		ImGui::SameLine();

		// Right
		{
			ImGui::BeginGroup();
			ImGui::BeginChild("item view", ImVec2(0, -ImGui::GetFrameHeightWithSpacing())); // Leave room for 1 line below us
			ImGui::Text(selected.c_str());
			ImGui::Separator();
			if (ImGui::BeginTabBar("##Tabs", ImGuiTabBarFlags_None))
			{
				if (ImGui::BeginTabItem("Description"))
				{
					if (selected != "") ImGui::Image((void*)(intptr_t)ChartletMap.at(selected), ImGui::GetContentRegionAvail(), ImVec2(0, 1), ImVec2(1, 0));
					ImGui::EndTabItem();
				}
				ImGui::EndTabBar();
			}
			ImGui::EndChild();

			ImGui::EndGroup();
		}
	}
	ImGui::End();
}

#endif // !DRAWWINDOW_H
