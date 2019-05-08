#include "WMObject.h"

#include "AppCore.h"

WMObject::WMObject(QObject *parent) :
	QObject(parent),
	m_appCore(nullptr)
{
	setObjectName("WM Object");
	
	if(parent->inherits("AppCore"))
	{
		m_appCore = qobject_cast<AppCore*>(parent);
	}
	else if(parent->inherits("WMObject"))
	{
		WMObject* wmoParent = qobject_cast<WMObject*>(parent);
		m_appCore = wmoParent->getAppCore();
	}

	Q_ASSERT_X(m_appCore != nullptr, "WMObject", "Attempt to instantiate outside of AppCore hierarchy");
}
