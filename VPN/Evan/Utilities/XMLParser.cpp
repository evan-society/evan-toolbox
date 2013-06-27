#include "XMLParser.h"
#include <QMessageBox>

XMLParser::XMLParser() : m_XMLRoot(), m_currentElement(), m_newElement()
{}

XMLParser::XMLParser(const QString& filePath) : m_xmlFile(filePath), m_XMLRoot(filePath), m_currentElement(), m_newElement()
{
	if (!QFile::exists(filePath))
		QMessageBox::critical(0, ("Error"), ("File ")+ (filePath) + (" was not found!!"));
	else if (!m_xmlFile.open(QIODevice::ReadOnly))
		QMessageBox::critical(0, ("Error"), ("Failed to open file for reading")+ (filePath));
	else
	{
		QString errorStr;
		int errorLine;
		int errorColumn;

		if (!m_XMLRoot.setContent(&m_xmlFile, true, &errorStr, &errorLine,&errorColumn))
			QMessageBox::information(0, ("DOM Bookmarks"),("Parse error : " + (errorStr) + filePath));
		else
			m_currentElement = m_XMLRoot.documentElement();
	}
	m_xmlFile.close();
}

XMLParser::~XMLParser()
{}

bool XMLParser::gotoElement(const QString& elementName)
{
	QDomNode n = m_currentElement.firstChild();
	if( n.isNull())
		n = m_currentElement.nextSibling();

	while( !n.isNull() )
	{
		if( !n.toElement().isNull() )
		{
			if( n.toElement().tagName() == elementName )
			{
			    m_currentElement = n.toElement();
				return true;
			}
		}

		n = n.nextSibling();
	}
	return false;
}

void XMLParser::addElement(const QString& eName)
{
    m_newElement = m_XMLRoot.createElement(eName);
    m_currentElement.appendChild(m_newElement);
}

void XMLParser::clearContents()
{
    QDomNode n = m_currentElement.firstChild();

	while( !n.isNull() )
	{
	    m_currentElement.removeChild(n);
	    n = m_currentElement.firstChild();
	}
}

void XMLParser::closeNewElement()
{
    if(!m_newElement.isNull() && !m_currentElement.isNull())
        m_currentElement = m_newElement;
    m_newElement.clear();
}

void XMLParser::goUp(int level)
{
    while(level > 0)
    {
        if(!m_currentElement.isNull())
            m_currentElement = m_currentElement.parentNode().toElement();
        level--;
    }
}

void XMLParser::goDown(int level)
{
    while(level > 0)
    {
        if(!m_currentElement.isNull())
            m_currentElement = m_currentElement.firstChildElement();
        level--;
    }
}

bool XMLParser::next()
{
    if(!m_currentElement.nextSiblingElement().isNull())
    {
        m_currentElement = m_currentElement.nextSiblingElement();
        return true;
    }
    return false;
}

void XMLParser::commit()
{
    if(!m_newElement.isNull() && !m_currentElement.isNull())
    {
        if (!m_xmlFile.open(QIODevice::WriteOnly))
        {
            QMessageBox::critical(0, ("Error"), ("Failed to open file for writing")+ m_xmlFile.fileName());
            return;
        }
        m_xmlFile.seek(0);
        m_xmlFile.write(m_XMLRoot.toByteArray());
        m_xmlFile.close();
    }
}
