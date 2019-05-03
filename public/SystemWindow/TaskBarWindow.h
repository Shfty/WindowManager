#ifndef TASKBARWINDOW_H
#define TASKBARWINDOW_H

#include <QObject>

#include "SystemWindow.h"

class TaskBarWindow : public SystemWindow
{
	Q_OBJECT

public:
	explicit TaskBarWindow(QObject* parent = nullptr);
	~TaskBarWindow();
};

#endif // TASKBARWINDOW_H