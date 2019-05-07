#ifndef QMLAPPLICATION_H
#define QMLAPPLICATION_H

#include <QObject>

class QQuickWindow;
class QQmlContext;
class QQmlEngine;
class QQuickItem;
class QQuickWindow;

class QMLApplication : public QObject
{
	Q_OBJECT

	public:
		explicit QMLApplication(QObject* parent = nullptr);
		~QMLApplication();

		static QMLApplication& instance()
		{
			static QMLApplication inst;
			return inst;
		}

		QQmlContext* getRootContext() const;

		QQuickWindow* createWindow(QUrl url, QRect geometry, QQmlContext* newContext = nullptr);

	public slots:
		void showConfigWindow();

	private:
		QQmlEngine* m_qmlEngine;
		QQuickWindow* m_configWindow;
};

#endif // QMLAPPLICATION_H