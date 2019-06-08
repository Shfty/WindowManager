#include "SettingsContainer.h"

SettingsContainer::SettingsContainer(QObject* parent)
	: QObject(parent)
	, m_headerSize(30)
	, m_itemMargin(10)
	, m_itemBorder(1)
	, m_itemRadius(10)
	, m_colorInactiveHeader("#80000000")
	, m_colorActiveHeader("#80007acc")
	, m_colorContainerPlaceholder("#80007acc")
	, m_colorContainerBorder("#807fcbff")
{
	qRegisterMetaType<SettingsContainer*>();
	setObjectName("Settings Container");
}
