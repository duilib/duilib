#ifndef DirectUI_h__
#define DirectUI_h__
#pragma once

#ifndef DIRECTUI_STATIC

#ifdef DIRECTUI_EXPORTS
#define DIRECTUI_API __declspec(dllexport)
#else
#define DIRECTUI_API __declspec(dllimport)
#pragma comment(lib,"DirectUI.lib")
#endif

#else
#define DIRECTUI_API

#endif // !DIRECTUI_STATIC

#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NON_CONFORMING_SWPRINTFS
#define WIN32_LEAN_AND_MEAN

#pragma   warning (disable : 4251)

// std
#include <string>
#include <map>
#include <vector>

// 防止全局命名空间污染
//using namespace std;

#include <windows.h>
#include <WTypes.h>
#include <ObjBase.h>
#include <tchar.h>
#include <atlstr.h>
#include <assert.h>

#include <WinBase.h>
#include <CommCtrl.h>

// Picture Decoder
#include "Utils/stb_image.h"

// DirectUI

// Micro&Utils
#include "Define/UIDefine.h"
#include "Define/IUIRender.h"
#include "Utils/UIUtils.h"
#include "Utils/UIDelegate.h"

// BaseDefine
#include "Base/ImageObject.h"
#include "Base/FontObject.h"
#include "Render/UIPaint.h"
#include "Render/GDIRender.h"

// CoreEngine
#include "Base/ResourceManager.h"
#include "Base/UIEngine.h"
#include "Base/ObjectUI.h"

#include "Window/WindowUI.h"

// Base Control
#include "Control/ControlUI.h"
#include "Layout/ContainerUI.h"
#include "Control/ScrollBarUI.h"

// Layout
#include "Layout/HorizontalLayoutUI.h"
#include "Layout/VerticalLayoutUI.h"
#include "Layout/ChildLayoutUI.h"
#include "Layout/TabLayoutUI.h"

// Control
#include "Control/LabelUI.h"
#include "Control/ButtonUI.h"

#include "Control/EditUI.h"
#include "Control/RichEditUI.h"


#endif // DirectUI_h__
