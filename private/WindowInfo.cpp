#include "WindowInfo.h"

WindowInfo::WindowInfo(QObject* parent)
	: QObject(parent)
	, m_hwnd(nullptr)
	, m_winTitle("")
	, m_winClass("")
	, m_winProcess("")
{
	setObjectName("Window Info");
}

WindowInfo::~WindowInfo()
{
}