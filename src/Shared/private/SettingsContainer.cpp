#include "SettingsContainer.h"

#include <QDebug>
Q_LOGGING_CATEGORY(settingsContainer, "shared.settingsContainer")

SettingsContainer::SettingsContainer(QObject* parent)
	: QObject(parent)
	, m_wallpaperUrl("file:///C:/Users/Josh/Pictures/wallpaper-space.jpg")
	, m_headerSize(30)
	, m_itemMargin(10)
	, m_itemBorder(1)
	, m_itemRadius(10)
	, m_itemAnimationCurve(6)
	, m_itemAnimationDuration(300)
	, m_colorInactiveHeader("#80000000")
	, m_colorActiveHeader("#80007acc")
	, m_colorContainerPlaceholder("#80007acc")
	, m_colorContainerBorder("#807fcbff")
{
	qRegisterMetaType<SettingsContainer*>();
	setObjectName("Settings Container");

	qCInfo(settingsContainer, "Startup");
}
