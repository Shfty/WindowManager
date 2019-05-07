#include "TrayIcon.h"

#include <QDebug>
#include <QMenu>

TrayIcon::TrayIcon(QObject* parent)
	: QSystemTrayIcon(parent)
{
	QObject::connect(this, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(onActivated(QSystemTrayIcon::ActivationReason)));

	setIcon(QIcon(":/logo.png"));
	setToolTip("Window Manager");
	show();
}

void TrayIcon::onActivated(QSystemTrayIcon::ActivationReason reason)
{
	if(reason == ActivationReason::Trigger)
	{
		this->onClicked();
	}
}
