#pragma once
#include "Singleton.h"
#include <memory>
#include "./Application/JobSystem.h"

class UIRenderer;
class TestEngine
{
private:
	std::unique_ptr<UIRenderer> mpUI;
public:
	TestEngine();
	~TestEngine();

	void Initialize();
	void Update(float elapsed_time);
	void Render();
};

#define TEST   (Singleton<TestEngine>::Get())