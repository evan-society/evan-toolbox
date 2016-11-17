#include "Logger.h"
#include <QTextEdit>
#include <QMessageBox>
#include <QDateTime>

Logger* Logger::m_instance = NULL;

Logger* Logger::getInstance()
{
  if(m_instance == NULL)
    m_instance = new Logger();
  return m_instance;
}

Logger::Logger() : m_logFile(""), m_logArea(NULL)
{}

Logger::~Logger()
{
    m_logFile.close();
}

void Logger::setLog(const QString& fileName)
{
    m_logArea = NULL;
    if (m_logFile.isOpen()) m_logFile.close();
    m_logFile.setFileName(fileName);
    if(!m_logFile.open(QIODevice::WriteOnly | QIODevice::Text))
        QMessageBox::critical(0, ("Logger Error"), ("Failed to create Log file ") + (fileName));
}

void Logger::setLog(QTextEdit* areaPtr)
{
    m_logArea = areaPtr;
    if (m_logFile.isOpen()) m_logFile.close();
    m_logFile.setFileName("");
}

void Logger::log(const QString& msg, LogType msgType, bool showDate)
{
    QString logMsg = msg;
    switch(msgType)
    {
        case WARNING:
            logMsg = "Warning: " + logMsg;
            if(m_logArea) logMsg = "<FONT COLOR=blue>" + logMsg + "</FONT>";
        break;
        case SYS_ERROR:
            logMsg = "System Error: " + logMsg;
            if(m_logArea) logMsg = "<FONT COLOR=red>" + logMsg + "</FONT>";
        break;
        case RUN_ERROR:
            logMsg = "Runtime Error: " + logMsg;
            if(m_logArea) logMsg = "<FONT COLOR=red>" + logMsg + "</FONT>";
        break;
        default: break;
    }

    if(showDate)
        logMsg = QDateTime::currentDateTime().toString(Qt::SystemLocaleShortDate) + " -> " + logMsg;
    //logMsg += "\n";

    if(m_logArea) m_logArea->append(logMsg);
    else if (m_logFile.isOpen())
    {
        QByteArray fileLine = "";
        fileLine += logMsg;
        m_logFile.write(fileLine);
    }
}

void Logger::log(int msg, LogType msgType, bool showDate)
{
    QString qmsg;
    qmsg.setNum(msg);
    log(qmsg, msgType, showDate);
}

void Logger::log(double msg, LogType msgType, bool showDate)
{
    QString qmsg;
    qmsg.setNum(msg);
    log(qmsg, msgType, showDate);
}
