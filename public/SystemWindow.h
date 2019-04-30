#ifndef SYSTEMWINDOW_H
#define SYSTEMWINDOW_H

#include <QObject>

#include "Win.h"

class SystemWindow : public QObject
{
	Q_OBJECT

  public:
	explicit SystemWindow(QObject *parent = nullptr);
	~SystemWindow();

	void setPosition(QPoint position);

	void show();
	void hide();
	void toggle();

  protected:
	HWND getWindowHwnd();

	QString m_windowTitle;
	QString m_windowClass;
};

#endif // SYSTEMWINDOW_H