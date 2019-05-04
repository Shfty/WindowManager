#include "Settings.h"

Settings::Settings(QObject* parent)
	: QObject(parent)
	, m_wallpaperUrl("file:///C:/Users/Josh/Pictures/space-1511293211222-8946.jpg")
	, m_wallpaperFillMode(2)
	, m_headerSize(30)
	, m_itemMargin(10)
	, m_colorInactiveHeader("#00007acc")
	, m_colorActiveHeader("#80007acc")
	, m_colorContainerPlaceholder("#80007acc")
{
	setObjectName("Settings");
}

Settings::~Settings()
{
}
