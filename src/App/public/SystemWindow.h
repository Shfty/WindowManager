#ifndef SYSTEMWINDOW_H
#define SYSTEMWINDOW_H

#include "WMObject.h"

#include <QPoint>
#include <QSize>

#include "Win.h"

class WindowController;
class WindowView;

class SystemWindow : public WMObject
{
	Q_OBJECT

public:
	explicit SystemWindow(QObject* parent = nullptr);

	void setPosition(QPoint position, QSize size = QSize());

	void show();
	void hide();
	void toggle();

signals:
	void beginMoveWindows();
	void moveWindow(HWND hwnd, QPoint position, QSize size, qlonglong layer = -2LL);
	void endMoveWindows();
	void showWindow(HWND);
	void hideWindow(HWND);

protected:
	WindowController* getWindowController();
	WindowView* getWindowView();
	HWND getWindowHwnd();

	QString m_windowTitle;
	QString m_windowClass;
};

#endif // SYSTEMWINDOW_H