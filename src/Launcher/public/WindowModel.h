#ifndef WINDOWMODEL_H
#define WINDOWMODEL_H

#include <QThread>
#include <QMap>

#include <QLoggingCategory>
Q_DECLARE_LOGGING_CATEGORY(windowModel);

#include "WindowInfo.h"

class WindowModel : public QThread
{
	Q_OBJECT

	public:
		explicit WindowModel(QObject *parent = nullptr);

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

#endif // WINDOWMODEL_H