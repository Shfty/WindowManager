#ifndef TASKBARWINDOW_H
#define TASKBARWINDOW_H

#include <QObject>

#include <QLoggingCategory>
Q_DECLARE_LOGGING_CATEGORY(taskBarWindow);

#include "SystemWindow.h"

class TaskBarWindow : public SystemWindow
{
	Q_OBJECT

public:
	explicit TaskBarWindow(QObject* parent = nullptr);

	HWND getTrayButtonHwnd();

};

#endif // TASKBARWINDOW_H