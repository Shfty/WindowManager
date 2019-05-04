#include "WindowInfo.h"

WindowInfo::WindowInfo(QObject* parent)
	: QObject(parent)
	, m_hwnd(nullptr)
{
	setObjectName("Window Info");
}

WindowInfo::~WindowInfo()
{
}
