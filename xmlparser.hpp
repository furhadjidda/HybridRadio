#ifndef XMLPARSER_HPP
#define XMLPARSER_HPP
#include <QTextStream>
#include <QXmlStreamReader>
#include <QFile>
#include <QDebug>
#include "datacollecter.hpp"
class XmlReader{
public:
    XmlReader( DataCollector* collector );
    void ReadSiXmlData( const QString& fileName);
    void ReadPiXmlData( const QString& fileName);
private:
    DataCollector *mCollector;
};

#endif // XMLPARSER_HPP

