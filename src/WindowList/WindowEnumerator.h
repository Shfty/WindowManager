#ifndef WINDOWENUMERATOR_H
#define WINDOWENUMERATOR_H

#include <QObject>
#include <QTimer>
#include <QVariant>
#include "Windows.h"

class WindowEnumerator : public QObject
{
	Q_OBJECT

public:
	explicit WindowEnumerator(QObject* parent);

signals:
	void enumComplete(QVariant windowList);

public slots:
	void startup();
	void cleanup();

	void enumerateWindows();
	void windowAdded(HWND hwnd);

private:
	QTimer* m_timer;
	QStringList m_windowList;
};

#endif // WINDOWENUMERATOR_H