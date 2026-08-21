#pragma once

enum class PlatformType
{
	Windows,
	Linux,
	OSX
};
enum class RenderPlatformType
{
	OpenGL, // Y
	Vulcan, // NO
	DirectX11, // NO
	DirectX12 // NO
};

enum class GuiPlatformType
{
	GLFW, // Y
	QT6, // MB
	Win32, // NO
	GTK, // NO

};