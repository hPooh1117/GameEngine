#include "..\TestEngine.h"
//#include "./Application/JobSystem.h"
#include "./Application/Helper.h"
#include "UIRenderer.h"
#include "./Renderer/Sprite.h"
#include "./Engine/Scene_ECS.h"
#include "./Utilities/Archive.h"
#include "./Utilities/Log.h"

TestEngine::TestEngine()
	:mpUI(std::make_unique<UIRenderer>())
{
}

TestEngine::~TestEngine()
{
}

void TestEngine::Initialize()
{
	JobSystem::Initialize(); // memory leak!!
	JobSystem::Context ctx;
	JobSystem::Execute(ctx, [](JobArgs args) {float j = 0; for (int i = 0; i < 10000; ++i) { j += sin((float)i); } Log::Info("Finished"); });
	JobSystem::Execute(ctx, [](JobArgs args) {float j = 0; for (int i = 0; i < 10000; ++i) { j += sin((float)i); } Log::Info("Finished");});
	JobSystem::Execute(ctx, [](JobArgs args) {float j = 0; for (int i = 0; i < 10000; ++i) { j += sin((float)i); } Log::Info("Finished");});
	JobSystem::Execute(ctx, [](JobArgs args) {float j = 0; for (int i = 0; i < 10000; ++i) { j += sin((float)i); } Log::Info("Finished");});
	JobSystem::Execute(ctx, [](JobArgs args) {float j = 0; for (int i = 0; i < 10000; ++i) { j += sin((float)i); } Log::Info("Finished");});
	JobSystem::Execute(ctx, [](JobArgs args) {float j = 0; for (int i = 0; i < 10000; ++i) { j += sin((float)i); } Log::Info("Finished");});
	JobSystem::Execute(ctx, [](JobArgs args) {float j = 0; for (int i = 0; i < 10000; ++i) { j += sin((float)i); } Log::Info("Finished");});
	JobSystem::Execute(ctx, [](JobArgs args) {float j = 0; for (int i = 0; i < 10000; ++i) { j += sin((float)i); } Log::Info("Finished");});
	JobSystem::Execute(ctx, [](JobArgs args) {float j = 0; for (int i = 0; i < 10000; ++i) { j += sin((float)i); } Log::Info("Finished"); });
	JobSystem::Execute(ctx, [](JobArgs args) {float j = 0; for (int i = 0; i < 10000; ++i) { j += sin((float)i); } Log::Info("Finished"); });
	JobSystem::Execute(ctx, [](JobArgs args) {float j = 0; for (int i = 0; i < 10000; ++i) { j += sin((float)i); } Log::Info("Finished"); });
	JobSystem::Execute(ctx, [](JobArgs args) {float j = 0; for (int i = 0; i < 10000; ++i) { j += sin((float)i); } Log::Info("Finished"); });
	JobSystem::Execute(ctx, [](JobArgs args) {float j = 0; for (int i = 0; i < 10000; ++i) { j += sin((float)i); } Log::Info("Finished"); });
	JobSystem::Execute(ctx, [](JobArgs args) {float j = 0; for (int i = 0; i < 10000; ++i) { j += sin((float)i); } Log::Info("Finished"); });
	JobSystem::Execute(ctx, [](JobArgs args) {float j = 0; for (int i = 0; i < 10000; ++i) { j += sin((float)i); } Log::Info("Finished"); });
	JobSystem::Execute(ctx, [](JobArgs args) {float j = 0; for (int i = 0; i < 10000; ++i) { j += sin((float)i); } Log::Info("Finished"); });
	JobSystem::Execute(ctx, [](JobArgs args) {float j = 0; for (int i = 0; i < 10000; ++i) { j += sin((float)i); } Log::Info("Finished"); });
	JobSystem::Execute(ctx, [](JobArgs args) {float j = 0; for (int i = 0; i < 10000; ++i) { j += sin((float)i); } Log::Info("Finished"); });
	JobSystem::Execute(ctx, [](JobArgs args) {float j = 0; for (int i = 0; i < 10000; ++i) { j += sin((float)i); } Log::Info("Finished"); });

	JobSystem::Wait(ctx);
	Log::Info("All Completed");
}

void TestEngine::Update(float elapsed_time)
{
	mpUI->UpdateFrame();
}

void TestEngine::Render()
{
	mpUI->SetNextWindowSettings(Vector2(0, 0), Vector2(500, 500));
	mpUI->BeginRenderingNewWindow("Test");
	//if (ImGui::Button("Is Busy?"))
	//{
	//	Log::Info("%s", JobSystem::IsBusy(mCtx) ? "Busy" : "Free");
	//}
	if (ImGui::Button("Add Object"))
	{
		ECS::Scene scene;
		scene.CreateEntityObject("GameObject");
		ECS::GetScene().Merge(scene);
	}
	if (ImGui::Button("Serialize"))
	{
		std::string filename = "./Data/Scene/MyScene.scn";
		Archive archive(filename, false);
		if (archive.IsOpen())
		{
			ECS::Scene& scene = ECS::GetScene();

			scene.Serialize(archive);
		}
		else
		{
			Log::Error("Couldn't save %s", filename.c_str());
		}
	}
	ECS::Scene& scene = ECS::GetScene();
	size_t count = scene.names.GetCount();
	for (auto i = 0; i < count; ++i)
	{
		ImGui::Text("%d : %s", i, scene.names[i].name.c_str());
	}
	mpUI->FinishRenderingWindow();

	mpUI->Execute();
}
