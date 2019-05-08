#ifndef WINDOWCONTROLLER_H
#define WINDOWCONTROLLER_H

#include <QMetaType>
#include <QObject>
#include <QRect>
#include <QVariant>
#include <QWindow>

#include "WindowInfo.h"

class EnumWindowsThread;

class WindowController : public QObject
{
	Q_OBJECT

public:
	explicit WindowController(QObject* parent = nullptr);
	~WindowController();

	Q_INVOKABLE void beginMoveWindows();
	Q_INVOKABLE void moveWindow(HWND hwnd, QPoint position, QSize size = QSize(), int layer = -2);
	Q_INVOKABLE void endMoveWindows();

	Q_INVOKABLE void showWindow(HWND hwnd);
	Q_INVOKABLE void hideWindow(HWND hwnd);

private:
	HDWP m_dwp;
};

#endif // WINDOWCONTROLLER_H