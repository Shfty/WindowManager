#include "Win.h"

#include <QDebug>

QDataStream& operator <<(QDataStream& ds, const HWND& hwnd)
{
	ds << QVariant(reinterpret_cast<qlonglong>(hwnd));
	return ds;
}

QDataStream& operator >>(QDataStream& ds, HWND& hwnd)
{
	QVariant hwndVar;
	ds >> hwndVar;
	hwnd = reinterpret_cast<HWND>(hwndVar.toLongLong());
	return ds;
}

QDebug operator <<(QDebug dbg, const HWND& hwnd)
{
	dbg.nospace() << reinterpret_cast<qlonglong>(hwnd);
	return dbg.maybeSpace();
}
