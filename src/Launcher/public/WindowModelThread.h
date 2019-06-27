#ifndef WINDOWMODELTHREAD_H
#define WINDOWMODELTHREAD_H

#include <QThread>
#include <QMap>

#include <QLoggingCategory>
Q_DECLARE_LOGGING_CATEGORY(windowModel);

#include "WindowInfo.h"

class WindowModelThread : public QThread
{
	Q_OBJECT

	public:
		explicit WindowModelThread(QObject *parent = nullptr);

	signals:
		void windowAdded(HWND window, QString winTitle, QString winClass, QString winProcess, qint32 winStyle);
		void windowTitleChanged(HWND window, QString title);
		void windowRemoved(HWND window);

		void windowScanFinished();

	public slots:
		void cleanup();

	protected:
		void run();

	private slots:
		void updateWindowMap();

	private:
		bool m_abort;
		QMap<HWND, QString> m_windowMap;
};

#endif // WINDOWMODELTHREAD_H