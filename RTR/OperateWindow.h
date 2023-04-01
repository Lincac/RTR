#pragma once

#ifndef OPERATEWINDOW_H
#define OPERATEWINDOW_H

#include"Window.h"

class OperateWindow : public Window {
public:
	OperateWindow() {
		tempPos[0] = tempPos[1] = tempPos[2] = 1;
		tempSca[0] = tempSca[1] = tempSca[2] = 1;
		tempRot[0] = tempRot[1] = tempRot[2] = 1;
		temp[0] = temp[1] = temp[2] = temp[3] = 1;

		lightPos[0] = lightPos[1] = lightPos[2] = 1;
		lightCol[0] = lightCol[1] = lightCol[2] = 1;

		OpenNewWindow = false;
		TexName = "";
	};

	virtual ~OperateWindow() override {};
	virtual void RenderWindow(std::shared_ptr<Objects> objs, unsigned int textureID = NULL) override;

	float tempPos[3];
	float tempSca[3];
	float tempRot[3];
	float temp[4];

	float lightPos[3];
	float lightCol[3];

private:
	void RenderNewWindow(std::shared_ptr<Objects> objs);
	bool OpenNewWindow;
	std::string TexName;
};

void OperateWindow::RenderWindow(std::shared_ptr<Objects> objs, unsigned int textureID) {
	ImGui::SetNextWindowPos(ImVec2(float(OWWidth + DWWidth), 0), ImGuiCond_None);
	ImGui::SetNextWindowSize(ImVec2(float(OPWWidth), float(OPWHeight / 2.0)), ImGuiCond_None);
	{
		ImGui::Begin("Operate Object", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_MenuBar);

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("Object Setting"))
			{
				if (ImGui::MenuItem("Reset Data"))
				{
						const auto& tempObj = objs->get_objs();
						tempObj[ObjID- 1]->SetPosition(glm::vec3(0));
						tempObj[ObjID - 1]->SetScale(glm::vec3(1));
						tempObj[ObjID - 1]->SetRotate(glm::vec3(1));

						tempPos[0] = tempPos[1] = tempPos[2] = 1;
						tempSca[0] = tempSca[1] = tempSca[2] = 1;
						tempRot[0] = tempRot[1] = tempRot[2] = 1;
						temp[0] = temp[1] = temp[2] = 1;

						OpenNewWindow = false;
						windowFocus = false;
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		if (ObjID != 0)
		{
			const auto& tempObj = objs->get_objs();
			ImGui::Text(tempObj[ObjID - 1]->GetObjName().c_str());

			if (ImGui::DragFloat3(" Position ", tempPos, 0.002f, -10.0f, 10.0f, "%.4f"))
			{
				glm::vec3 delta = glm::vec3(tempPos[0], tempPos[1], tempPos[2]) - glm::vec3(temp[0], temp[1], temp[2]);
				glm::vec3 Pos = tempObj[ObjID - 1]->GetPosition();
				Pos += delta;
				tempObj[ObjID - 1]->SetPosition(Pos);

				temp[0] = tempPos[0];
				temp[1] = tempPos[1];
				temp[2] = tempPos[2];
			}
			if (ImGui::DragFloat3(" Scale ", tempSca, 0.002f, -10.0f, 10.0f, "%.4f"))
			{
				glm::vec3 delta = glm::vec3(tempSca[0], tempSca[1], tempSca[2]) - glm::vec3(temp[0], temp[1], temp[2]);
				glm::vec3 Sca = tempObj[ObjID - 1]->GetScale();
				Sca += delta;
				tempObj[ObjID - 1]->SetScale(Sca);

				temp[0] = tempSca[0];
				temp[1] = tempSca[1];
				temp[2] = tempSca[2];
			}
			if (ImGui::DragFloat3(" Rotate ", tempRot, 0.002f, -10.0f, 10.0f, "%.4f"))
			{
				glm::vec3 delta = glm::vec3(tempRot[0], tempRot[1], tempRot[2]) - glm::vec3(temp[0], temp[1], temp[2]);
				glm::vec3 Rot = tempObj[ObjID - 1]->GetScale();
				Rot += delta;
				tempObj[ObjID - 1]->SetScale(Rot);

				temp[0] = tempRot[0];
				temp[1] = tempRot[1];
				temp[2] = tempRot[2];
			}

			ImGui::NewLine();
			ImGui::Text("Texture");
			unsigned int Albedo = tempObj[ObjID - 1]->GetAlbedo();
			unsigned int Normal = tempObj[ObjID - 1]->GetNormal();
			unsigned int Metallic = tempObj[ObjID - 1]->GetMetallic();
			unsigned int Roughness = tempObj[ObjID - 1]->GetRoughness();
			unsigned int Ao = tempObj[ObjID - 1]->GetAo();

			if (renderModeName == "PBR")
			{
				ImGui::Image((void*)(intptr_t)Albedo, ImVec2(100.0f, 100.0f));
				if (ImGui::Button("Change Albedo(PBR)"))
				{
					OpenNewWindow = true;
					TexName = "Albedo";
				}

				ImGui::Image((void*)(intptr_t)Normal, ImVec2(100.0f, 100.0f));
				if (ImGui::Button("Change Normal(PBR)"))
				{
					OpenNewWindow = true;
					TexName = "Normal";
				}

				ImGui::Image((void*)(intptr_t)Metallic, ImVec2(100.0f, 100.0f));
				if (ImGui::Button("Change Metallic(PBR)"))
				{
					OpenNewWindow = true;
					TexName = "Metallic";
				}

				ImGui::Image((void*)(intptr_t)Roughness, ImVec2(100.0f, 100.0f));
				if (ImGui::Button("Change Roughness(PBR)"))
				{
					OpenNewWindow = true;
					TexName = "Roughness";
				}

				ImGui::Image((void*)(intptr_t)Ao, ImVec2(100.0f, 100.0f));
				if (ImGui::Button("Change Ao(PBR)"))
				{
					OpenNewWindow = true;
					TexName = "Ao";
				}
			}
			else if (renderModeName == "BlinPhone") {
				ImGui::Image((void*)(intptr_t)Albedo, ImVec2(100.0f, 100.0f));
				if (ImGui::Button("Change Albedo(BlinPhone)"))
				{
					OpenNewWindow = true;
					TexName = "Albedo";
				}

				ImGui::Image((void*)(intptr_t)Normal, ImVec2(100.0f, 100.0f));
				if (ImGui::Button("Change Normal(BlinPhone)"))
				{
					OpenNewWindow = true;
					TexName = "Normal";
				}

				temp[3] = tempObj[ObjID - 1]->GetGloss();
				ImGui::DragFloat("Gloss", &temp[3], 0.1f, 8.0f, 256.0f);
				tempObj[ObjID - 1]->SetGloss(temp[3]);
			}
		}

		ImGui::End();

		windowFocus = OpenNewWindow;

		if (OpenNewWindow) RenderNewWindow(objs);
	}

	ImGui::SetNextWindowPos(ImVec2(float(OWWidth + DWWidth), OPWHeight / 2.0), ImGuiCond_None);
	ImGui::SetNextWindowSize(ImVec2(float(OPWWidth), float(OPWHeight / 2.0)), ImGuiCond_None);
	{
		ImGui::Begin("Operate World", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_MenuBar);

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("World Setting"))
			{
				if (ImGui::MenuItem("Reset World"))
				{
					temp[0] = temp[1] = temp[2] = 1;

					light->SetLightPos(glm::vec3(20, 50, 20));
					light->SetLightCol(glm::vec3(255 / 256.0, 244 / 256.0, 214 / 256.0));

					lightCol[0] = 255 / 256.0;
					lightCol[1] = 244 / 256.0;
					lightCol[2] = 214 / 256.0;

					OpenSSAO = false;
					OpenSSR = false;
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		if (ImGui::BeginTabBar("##Tabs", ImGuiTabBarFlags_None)) {
				if(ImGui::BeginTabItem("World"))
				{
					if (ImGui::DragFloat3(" Light Position ", lightPos, 0.002f, -10.0f, 10.0f, "%.4f"))
					{
						glm::vec3 delta = glm::vec3(lightPos[0], lightPos[1], lightPos[2]) - glm::vec3(temp[0], temp[1], temp[2]);
						glm::vec3 Pos = light->GetLightPos();
						Pos += delta;
						light->SetLightPos(Pos);

						temp[0] = lightPos[0];
						temp[1] = lightPos[1];
						temp[2] = lightPos[2];
					}

					ImGui::ColorEdit3("Light Color", lightCol);
					light->SetLightCol(glm::vec3(lightCol[0], lightCol[1], lightCol[2]));

					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem("Other"))
				{
					ImGui::Checkbox("OpenSSR", &OpenSSR);
					ImGui::Checkbox("OpenSSAO", &OpenSSAO);
					ImGui::Checkbox("OpenSkyBox", &OpenSkyBox);

					ImGui::EndTabItem();
				}

				ImGui::EndTabBar();
		}

		ImGui::End();
	}
}

void  OperateWindow::RenderNewWindow(std::shared_ptr<Objects> objs) {
	ImGui::SetNextWindowSize(ImVec2(500, 440), ImGuiCond_FirstUseEver);
	if (ImGui::Begin("Texture Select", &OpenNewWindow, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoCollapse))
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
					OpenNewWindow = false;
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
					if(selected != "") ImGui::Image((void*)(intptr_t)ChartletMap.at(selected), ImGui::GetContentRegionAvail(), ImVec2(0, 1), ImVec2(1, 0));
					ImGui::EndTabItem();
				}
				ImGui::EndTabBar();
			}
			ImGui::EndChild();

			if (ImGui::Button("Select")) {
				const auto& tempObj = objs->get_objs();
				if(TexName == "Albedo") tempObj[ObjID - 1]->SetAlbedo(ChartletMap.at(selected));
				if (TexName == "Normal") tempObj[ObjID - 1]->SetNormal(ChartletMap.at(selected));
				if (TexName == "Metallic") tempObj[ObjID - 1]->SetMetallic(ChartletMap.at(selected));
				if (TexName == "Roughness") tempObj[ObjID - 1]->SetRoughness(ChartletMap.at(selected));
				if (TexName == "Ao") tempObj[ObjID - 1]->SetAo(ChartletMap.at(selected));
			}
			ImGui::EndGroup();
		}
	}
	ImGui::End();
}

#endif // !OPERATEWINDOW_H
