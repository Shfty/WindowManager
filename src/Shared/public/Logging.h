#ifndef LOGGING_H
#define LOGGING_H

#include <QDebug>

namespace Logging
{
	static void logHandler(QtMsgType type, const QMessageLogContext& ctx, const QString& msg)
	{
		QTextStream stdOutStream(stdout);
		QTextStream stdErrStream(stderr);

		QTextStream* os = nullptr;
		QString typePrefix;
		switch (type) {
			case QtDebugMsg:
				typePrefix = QString("Debug");
				os = &stdOutStream;
				break;
			case QtInfoMsg:
				typePrefix = QString("Info");
				os = &stdOutStream;
				break;
			case QtWarningMsg:
				typePrefix = QString("Warning");
				os = &stdErrStream;
				break;
			case QtCriticalMsg:
				typePrefix = QString("Critical");
				os = &stdErrStream;
				break;
			case QtFatalMsg:
				typePrefix = QString("Fatal");
				os = &stdErrStream;
				break;
		}

		QTextStream& targetStream = *os;
		if(strncmp(ctx.category, "default", 7) == 0)
		{
			targetStream << msg << endl;
		}
		else
		{
			targetStream << ctx.category << ": " << msg << endl;
		}
	}
}

#endif // LOGGING_H
