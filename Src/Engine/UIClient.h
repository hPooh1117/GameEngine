#pragma once
#include "./Utilities/ImguiSelf.h"

class UIClient
{
public:
	UIClient(){}

	virtual void RenderUI() = 0;

	virtual ~UIClient(){}
};
