#pragma once

#ifndef RENDERHELP_H
#define RENDERHELP_H

#include<iostream>
#include"Objects.h"

class RenderHelp {
public:
	virtual void RenderPass(std::shared_ptr<Objects> objs,std::string renderModeName = "") = 0;
	virtual unsigned int GetPassFBO() { return 0; };
};
#endif // !RENDERHELP_H
