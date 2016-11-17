#ifndef GEODEFINDER_H
#define GEODEFINDER_H

#include <osg/NodeVisitor>
#include <QVector>

class GeodeFinder : public osg::NodeVisitor
{
private:
    QVector<osg::ref_ptr<osg::Geode> > m_foundGeodes;

public:
    GeodeFinder() : osg::NodeVisitor( NodeVisitor::TRAVERSE_ALL_CHILDREN )
    {}
    virtual ~GeodeFinder() {}

    virtual void apply(osg::Geode& geode)
    {m_foundGeodes.push_back(&geode);}
    const QVector<osg::ref_ptr<osg::Geode> >& getFoundGeodes() const {return m_foundGeodes;}
};

#endif


