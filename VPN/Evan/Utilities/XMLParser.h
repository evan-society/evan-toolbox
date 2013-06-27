#ifndef XMLPARSER_H
#define XMLPARSER_H

#include <QtXml/QDomDocument>
#include <QFile>

class XMLParser
{
private:
    QFile m_xmlFile;
	QDomDocument m_XMLRoot;
	QDomElement m_currentElement;
	QDomElement m_newElement;
public:
	XMLParser();
	XMLParser(const QString&);
	~XMLParser();

	bool gotoElement(const QString&);
	QString getAttribute(const QString& aName) const {return m_currentElement.attribute(aName,"");}
	QString getElementValue() const {return m_currentElement.text();}
	void addElement(const QString& eName);
	void clearContents();
	void closeNewElement();
	void addAttribute(const QString& aName, const QString& aValue)
	{if(!m_newElement.isNull()) m_newElement.setAttribute(aName,aValue);}
	void resetCursor() {m_currentElement = m_XMLRoot.documentElement();}
	void goUp(int);
	void goDown(int);
	bool next();
	void commit();
};

#endif
