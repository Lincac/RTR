#pragma once

#ifndef OPERATEWINDOW_H
#define OPERATEWINDOW_H

#include"Window.h"
#include <windows.h>  
#include <commdlg.h> 

class OperateWindow : public Window {
public:
	OperateWindow() {
		tempPos[0] = tempPos[1] = tempPos[2] = 1;
		tempSca[0] = tempSca[1] = tempSca[2] = 1;
		tempRot[0] = tempRot[1] = tempRot[2] = 1;
		temp[0] = temp[1] = temp[2] = 1;

		OpenNewWindow = false;
		TexName = "";
	};

	virtual ~OperateWindow() override {};
	virtual void RenderWindow(std::shared_ptr<Objects> objs, unsigned int textureID = NULL) override;

	float tempPos[3];
	float tempSca[3];
	float tempRot[3];
	float temp[3];

private:
	void RenderNewWindow(std::shared_ptr<Objects> objs);
	std::string OpenFileDialog();
	bool OpenNewWindow;
	std::string TexName;
};

void OperateWindow::RenderWindow(std::shared_ptr<Objects> objs, unsigned int textureID) {
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
			unsigned int Albedo = tempObj[ObjID-1]->GetAlbedo();
			unsigned int Normal = tempObj[ObjID - 1]->GetNormal();
			unsigned int Metallic = tempObj[ObjID - 1]->GetMetallic();
			unsigned int Roughness = tempObj[ObjID - 1]->GetRoughness();
			unsigned int Ao = tempObj[ObjID - 1]->GetAo();

			if (renderModeName == "PBR")
			{
				ImGui::Image((void*)(intptr_t)Albedo, ImVec2(100.0f, 100.0f));
				if (ImGui::Button("change"))
				{
					OpenNewWindow = true;
					windowFocus = true;
					TexName = "Albedo";
				}

				ImGui::Image((void*)(intptr_t)Normal, ImVec2(100.0f, 100.0f));
				if (ImGui::Button("change"))
				{
					OpenNewWindow = true;
					windowFocus = true;
					TexName = "Normal";
				}

				ImGui::Image((void*)(intptr_t)Metallic, ImVec2(100.0f, 100.0f));
				if (ImGui::Button("change"))
				{
					OpenNewWindow = true;
					windowFocus = true;
					TexName = "Metallic";
				}

				ImGui::Image((void*)(intptr_t)Roughness, ImVec2(100.0f, 100.0f));
				if (ImGui::Button("change"))
				{
					OpenNewWindow = true;
					windowFocus = true;
					TexName = "Roughness";
				}

				ImGui::Image((void*)(intptr_t)Ao, ImVec2(100.0f, 100.0f));
				if (ImGui::Button("change"))
				{
					OpenNewWindow = true;
					windowFocus = true;
					TexName = "Ao";
				}
			}
			else if (renderModeName == "BlinPhone") {
				ImGui::Image((void*)(intptr_t)Albedo, ImVec2(100.0f, 100.0f));
				if (ImGui::Button("change"))
				{
					OpenNewWindow = true;
					windowFocus = true;
					TexName = "Albedo";
				}

				ImGui::Image((void*)(intptr_t)Normal, ImVec2(100.0f, 100.0f));
				if (ImGui::Button("change"))
				{
					OpenNewWindow = true;
					windowFocus = true;
					TexName = "Normal";
				}
			}
		}

		ImGui::End();
	}

	if (OpenNewWindow) RenderNewWindow(objs);
}

void  OperateWindow::RenderNewWindow(std::shared_ptr<Objects> objs) {
	ImGui::SetNextWindowSize(ImVec2(500, 440), ImGuiCond_FirstUseEver);
	if (ImGui::Begin("Example: Simple layout", &OpenNewWindow, ImGuiWindowFlags_MenuBar))
	{
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Load Texture"))
				{
					std::string path = OpenFileDialog();
					std::string sub = path.substr(path.find_last_of('\\') + 1);
					std::string name = sub.substr(0, sub.find('.'));

					unsigned int texID = LoadTexture(path.c_str());
					ChartletMap.emplace(name, texID);
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

std::string TCHAR2STRING(TCHAR* STR)
{
	int iLen = WideCharToMultiByte(CP_ACP, 0, STR, -1, NULL, 0, NULL, NULL);
	char* chRtn = new char[iLen * sizeof(char)];
	WideCharToMultiByte(CP_ACP, 0, STR, -1, chRtn, iLen, NULL, NULL);
	std::string str(chRtn);
	delete chRtn;
	return str;
}

std::string OperateWindow::OpenFileDialog() {

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

#endif // !OPERATEWINDOW_H
