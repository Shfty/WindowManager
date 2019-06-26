#include "TrayIcon.h"

#include <QSystemTrayIcon>
#include <QIcon>
#include <QApplication>

#include <QDebug>
Q_LOGGING_CATEGORY(trayIcon, "launcher.trayIcon")

TrayIcon::TrayIcon(QObject* parent)
	: QObject(parent)
	, m_trayIcon(nullptr)
{
	// Setup tray icon
	m_trayIcon = new QSystemTrayIcon(QIcon(":/graphics/logo.png"), this);
	connect(m_trayIcon, &QSystemTrayIcon::activated, [=](QSystemTrayIcon::ActivationReason reason){
		Q_UNUSED(reason);
		QApplication::quit();
	});

	m_trayIcon->show();
}
