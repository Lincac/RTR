#pragma once

#ifndef PBR_H
#define PBR_H

#include"RenderMode.h"

class PBR : public RenderMode {
public:
	PBR();
	virtual void Render(std::shared_ptr<Objects> objs) override;
	virtual void DRender() override;
	virtual std::string getRenderModeName() override { return "PBR"; };
private:
	std::shared_ptr<Shader> shader;
	std::shared_ptr<Shader> Dshader;
};
PBR::PBR() {
	OpenSkyBox = true;
	shader = std::make_shared<Shader>("shader/pbr/pbr.vs", "shader/pbr/pbr.fs");
	Dshader = std::make_shared<Shader>("shader/defered/PBRDefered.vs", "shader/defered/PBRDefered.fs");
}

void PBR::Render(std::shared_ptr<Objects> objs) {
	objs->render("PBR", shader);
}

void PBR::DRender() {
	Dshader->use();
	Dshader->setInt("gPosition", 0);
	Dshader->setInt("gNormal", 1);
	Dshader->setInt("gAlbedo", 2);
	Dshader->setInt("gParameter", 3);

	Dshader->setInt("irradianceMap", 4);
	Dshader->setInt("prefilterMap", 5);
	Dshader->setInt("LUTMap", 6);

	Dshader->setInt("shadowMap", 7);

	Dshader->setMat4("view", view);
	Dshader->setVec3("viewPos", camera.Position);

	Dshader->setVec3("lightPos", light->GetLightPos());
	Dshader->setVec3("lightCol", light->GetLightCol());

	Dshader->setFloat("far_plane", camera.farplane);
	Dshader->setInt("countLayer", shadowlayer.size());
	for (int i = 0; i < shadowlayer.size(); i++)
		Dshader->setFloat("planeDistances[" + std::to_string(i) + "]", shadowlayer[i]);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TexMap.at("gPosition"));
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, TexMap.at("gNormal"));
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, TexMap.at("gAlbedo"));
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, TexMap.at("gParameter"));

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_CUBE_MAP, TexMap.at("irradianceMap"));
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_CUBE_MAP, TexMap.at("prefilterMap"));
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, TexMap.at("LUTMap"));
	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D_ARRAY, TexMap.at("CSM"));
	renderQuad();
}

#endif // !BLIN-PBR_H
