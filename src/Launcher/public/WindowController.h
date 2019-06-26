#ifndef WINDOWCONTROLLER_H
#define WINDOWCONTROLLER_H

#include <QMetaType>
#include <QObject>
#include <QRect>
#include <QVariant>
#include <QWindow>

#include <QLoggingCategory>
Q_DECLARE_LOGGING_CATEGORY(windowController);

#include <WindowInfo.h>

class WindowController : public QObject
{
	Q_OBJECT

public:
	explicit WindowController(QObject* parent = nullptr);

public slots:
	void beginMoveWindows();
	void moveWindow(HWND hwnd, QPoint position, QSize size = QSize(), qlonglong layer = -2LL, quint32 extraFlags = 0);
	void endMoveWindows();

	void setWindowStyle(HWND hwnd, qint32 style);

private:
	HDWP m_dwp;
};

#endif // WINDOWCONTROLLER_H