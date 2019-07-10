#ifndef SYSTEMWINDOW_H
#define SYSTEMWINDOW_H

#include <QObject>
#include <QPoint>
#include <QSize>

#include <QLoggingCategory>
Q_DECLARE_LOGGING_CATEGORY(systemWindow);

#include "Win.h"

class SystemWindow : public QObject
{
	Q_OBJECT

public:
	explicit SystemWindow(QObject* parent = nullptr);

	void setPosition(QPoint position, QSize size = QSize());

	void show();
	void hide();
	void toggle();

protected:
	HWND getWindowHwnd();

	QString m_windowTitle;
	QString m_windowClass;
};

#endif // SYSTEMWINDOW_H