#include "Printables.h"

void Printables::clear()
{
	for(int i=0; i<m_printables.size(); ++i)
		m_printables[i].release();
	m_printables.clear();
}
void Printables::addPart(DataTypePtr& part, const QString& desc)
{

	if(part->isValid())
	{
		if(part->isType(IPRINTABLE_T))
		{
			IPrintable* printable = dynamic_cast<IPrintable*>(part.getPtr());
			addPrintable(printable);
		}
	}
}
void Printables::removePart(DataTypePtr& part)
{
	if(part->isValid())
	{
		if(part->isType(IPRINTABLE_T))
		{
			IPrintable* printable = dynamic_cast<IPrintable*>(part.getPtr());
			removePrintable(printable);
		}
	}
}
