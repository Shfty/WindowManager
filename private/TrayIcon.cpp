#include "TrayIcon.h"

#include <QDebug>
#include <QMenu>

TrayIcon::TrayIcon(QObject* parent)
	: QSystemTrayIcon(parent)
	, m_menu(new QMenu())
{
	QAction* quitAction = m_menu->addAction("Quit");
	QObject::connect(quitAction, SIGNAL(triggered()), this, SIGNAL(onQuitTriggered()));

	QObject::connect(this, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(onActivated(QSystemTrayIcon::ActivationReason)));

	setIcon(QIcon(":/logo.png"));
	setContextMenu(m_menu);
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
