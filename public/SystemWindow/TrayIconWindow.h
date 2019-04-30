#ifndef TRAYICONWINDOW_H
#define TRAYICONWINDOW_H

#include <QObject>

#include "SystemWindow.h"

class TrayIconWindow : public SystemWindow
{
	Q_OBJECT

	public:
		explicit TrayIconWindow(QObject *parent = nullptr);
		~TrayIconWindow();
};

#endif // TRAYICONWINDOW_H