#include "SystemWindow/StartMenuWindow.h"

#include <QDebug>

StartMenuWindow::StartMenuWindow(QObject* parent)
	: SystemWindow(parent)
{
	m_windowTitle = "Start";
	m_windowClass = "Windows.UI.Core.CoreWindow";
	this->setPosition(QPoint(0, 0));
}

StartMenuWindow::~StartMenuWindow()
{
}
