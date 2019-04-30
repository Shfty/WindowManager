#ifndef ENUMWINDOWSTHREAD_H
#define ENUMWINDOWSTHREAD_H

#include <QThread>
#include <QMap>
#include "WindowInfo.h"

class EnumWindowsThread : public QThread
{
		Q_OBJECT

	public:
		explicit EnumWindowsThread(QObject *parent = nullptr);

		void startProcess();

	signals:
		void windowAdded(WindowInfo windowInfo);
		void windowChanged(WindowInfo windowInfo);
		void windowRemoved(WindowInfo windowInfo);
		void windowScanFinished();

	public slots:
		void stopProcess();

	protected:
		void run();

	private:
		bool m_abort;
		QMap<HWND, WindowInfo> m_windowMap;
};

#endif // ENUMWINDOWSTHREAD_H