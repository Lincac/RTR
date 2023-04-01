#pragma once


#ifndef RENDERPATH_H
#define RENDERPATH_H

#include<iostream>
#include"RenderHelp.h"

#include"BlinPhone.h"
#include"PBR.h"

class RenderPath {
public:
	virtual unsigned int Render(std::shared_ptr<Objects> objs) = 0;
	virtual void SetRenderMode(std::shared_ptr<RenderMode> rm) = 0;
	virtual std::string getRenderPathName() = 0;
protected:
	virtual void HDR(std::shared_ptr<Objects> objs) = 0;
};

#endif // !RENDERPATH_H
