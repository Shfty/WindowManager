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
		void windowAdded(HWND window, QString winTitle, QString winClass, QString winProcess, qint32 winStyle);
		void windowTitleChanged(HWND window, QString title);
		void windowRemoved(HWND window);
		void windowScanFinished();
		void windowListChanged();

	public slots:
		void stopProcess();

	protected:
		void run();

	private:
		bool m_abort;
		QMap<HWND, QString> m_windowMap;
};

#endif // ENUMWINDOWSTHREAD_H