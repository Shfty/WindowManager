#ifndef WMOBJECT_H
#define WMOBJECT_H

#include <QObject>

class AppCore;

class WMObject : public QObject
{
		Q_OBJECT
public:
	explicit WMObject(QObject *parent = nullptr);


protected:
	AppCore* getAppCore() const { return m_appCore; }

private:
	AppCore* m_appCore;
};

#endif // WMOBJECT_H
