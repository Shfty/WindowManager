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
		void windowAdded(HWND window, QString title);
		void windowTitleChanged(HWND window, QString title);
		void windowRemoved(HWND window);
		void windowScanFinished();

	public slots:
		void stopProcess();

	protected:
		void run();

	private:
		bool m_abort;
		QMap<HWND, QString> m_windowMap;
};

#endif // ENUMWINDOWSTHREAD_H