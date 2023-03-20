#pragma once

#ifndef BLINPHONE_H
#define BLINPHONE_H

#include"RenderMode.h"

class BlinPhone : public RenderMode {
public:
	BlinPhone();
	virtual void Render(std::shared_ptr<Objects> objs) override;
	virtual void DRender() override;
	virtual std::string getRenderModeName() override { return "BlinPhone"; };
private:
	std::shared_ptr<Shader> shader;
	std::shared_ptr<Shader> Dshader;
};
BlinPhone::BlinPhone() {
	OpenSkyBox = false;
	shader = std::make_shared<Shader>("shader/blinphone/blinphone.vs", "shader/blinphone/blinphone.fs");
	Dshader = std::make_shared<Shader>("shader/defered/BlinPhoneDefered.vs", "shader/defered/BlinPhoneDefered.fs");

}

void BlinPhone::Render(std::shared_ptr<Objects> objs) {
	objs->render("BlinPhone",shader);
}

void BlinPhone::DRender() {
	Dshader->use();
	Dshader->setInt("gPosition", 0);
	Dshader->setInt("gNormal", 1);
	Dshader->setInt("gAlbedo", 2);
	Dshader->setInt("shadowMap", 3);

	Dshader->setVec3("lightCol", light->GetLightCol());
	Dshader->setVec3("lightDir", glm::normalize(light->GetLightPos()));

	Dshader->setVec3("Ambient", Ambient);

	Dshader->setVec3("viewPos", camera.Position);
	Dshader->setFloat("farPlane", camera.farplane);
	Dshader->setMat4("view", view);

	Dshader->setInt("cascadeCount", shadowlayer.size());
	for (size_t i = 0; i < shadowlayer.size(); i++)
		Dshader->setFloat("cascadePlaneDistances[" + std::to_string(i) + "]", shadowlayer[i]);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TexMap.at("gPosition"));
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, TexMap.at("gNormal"));
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, TexMap.at("gAlbedo"));
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D_ARRAY, TexMap.at("CSM"));
	renderQuad();


}

#endif // !BLINPHONE_H
