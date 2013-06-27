#ifndef LOGGER_H
#define LOGGER_H

#include <QFile>

class QTextEdit;

class Logger
{
private:
    QFile m_logFile;
    QTextEdit* m_logArea;
    static Logger* m_instance;

    Logger();
    Logger(const Logger&);
    Logger& operator=(const Logger&);

public:
    enum LogType {INFO, WARNING, SYS_ERROR, RUN_ERROR};
    ~Logger();

    void setLog(const QString&);
    void setLog(QTextEdit*);
    void log(const QString&, LogType msgType = INFO, bool showDate = true);
    void log(int, LogType msgType = INFO, bool showDate = true);
    void log(double, LogType msgType = INFO, bool showDate = true);
    static Logger* getInstance();
};

#endif

