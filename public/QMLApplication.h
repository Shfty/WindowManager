#ifndef QMLAPPLICATION_H
#define QMLAPPLICATION_H

#include <QQmlApplicationEngine>

class TreeItem;

class QMLApplication : public QQmlApplicationEngine
{
	Q_OBJECT

	public:
		explicit QMLApplication(QObject* parent = nullptr);

		void setModel(TreeItem* model);
		void showWindow();
};

#endif // QMLAPPLICATION_H