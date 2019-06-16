#ifndef TRAYWINDOW_H
#define TRAYWINDOW_H

#include <QObject>

#include "SystemWindow.h"

class TrayWindow : public SystemWindow
{
	Q_OBJECT

public:
	explicit TrayWindow(QObject* parent = nullptr);
};

#endif // TRAYWINDOW_H