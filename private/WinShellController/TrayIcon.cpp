#include "WinShellController/TrayIcon.h"

TrayIcon::TrayIcon(QObject* parent)
	: QSystemTrayIcon(parent)
{
	setObjectName("Tray Icon");

	setIcon(QIcon("qrc:/graphics/logo.png"));
	setToolTip("Window Manager");

	QObject::connect(this, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(onActivated(QSystemTrayIcon::ActivationReason)));
}

void TrayIcon::onActivated(QSystemTrayIcon::ActivationReason reason)
{
	if(reason == ActivationReason::Trigger)
	{
		onClicked();
	}
}
