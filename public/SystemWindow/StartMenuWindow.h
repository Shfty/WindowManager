#ifndef STARTMENUWINDOW_H
#define STARTMENUWINDOW_H

#include <QObject>

#include "SystemWindow.h"

class StartMenuWindow : public SystemWindow
{
	Q_OBJECT

	public:
		explicit StartMenuWindow(QObject *parent = nullptr);
		~StartMenuWindow();
};

#endif // STARTMENUWINDOW_H