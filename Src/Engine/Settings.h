#pragma once

namespace Settings
{
	struct Logger
	{
		bool bConsole;
		bool bFile;
	};

	struct Window
	{
		int width;
		int height;
		int fullscreen;
		int vsync;
	};

	struct Renderer
	{
		bool bCastShadow;
		bool bEnableAO;
		bool bIsDeferred;
		bool bCubeMap;
	};

	struct Camera
	{
		float fovH;
		float fovV;
		float aspect;
		float nearPlane;
		float farPlane;
	};

	struct Engine
	{
		Logger   logger;
		Window   window;
		Renderer renderer;
		Camera   camera;
		bool     bEnableImGui;
	};
}