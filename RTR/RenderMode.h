#pragma once

#ifndef RENDERMODE_H
#define RENDERMODE_H

#include<iostream>

#include"SSAOPass.h"
#include"SSRPass.h"	
#include"GBufferPass.h"
#include"IrradiancePass.h"
#include"LUTPass.h"
#include"PrefilterPass.h"
#include"CSMPass.h"

#include<unordered_map> // ÎÞÐòmap
#include<vector>

class RenderMode {
public:
	virtual void Render(std::shared_ptr<Objects> objs) = 0;
	virtual void DRender() = 0;
	virtual std::string getRenderModeName() = 0;
};

std::unordered_map<std::string, std::shared_ptr<RenderHelp>> passes;
#endif // !RENDERMODE_H
