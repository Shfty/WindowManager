#ifndef TRAYWINDOW_H
#define TRAYWINDOW_H

#include <QObject>

#include <QLoggingCategory>
Q_DECLARE_LOGGING_CATEGORY(trayWindow);

#include "SystemWindow.h"

class TrayWindow : public SystemWindow
{
	Q_OBJECT

public:
	explicit TrayWindow(QObject* parent = nullptr);
};

#endif // TRAYWINDOW_H