#ifndef SYSTEMWINDOW_H
#define SYSTEMWINDOW_H

#include "WMObject.h"

#include "Win.h"

class WindowController;
class WindowView;

class SystemWindow : public WMObject
{
	Q_OBJECT

public:
	explicit SystemWindow(QObject* parent = nullptr);
	~SystemWindow();

	void setPosition(QPoint position);

	void show();
	void hide();
	void toggle();

protected:
	WindowController* getWindowController();
	WindowView* getWindowView();
	HWND getWindowHwnd();

	QString m_windowTitle;
	QString m_windowClass;
};

#endif // SYSTEMWINDOW_H