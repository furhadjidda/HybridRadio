//!
//! \file xmlparser.hpp
//!
//! \author Furhad Jidda
//!
//! \brief This file containg API's and methods to read
//! xml data from SI, XSI or PI xml files.
//!

#ifndef XMLPARSER_HPP
#define XMLPARSER_HPP

#include <QTextStream>
#include <QXmlStreamReader>
#include <QFile>
#include <QDebug>
#include "datacollecter.hpp"


//!
//! \brief The XmlReader class
//!
//! \defgroup HybridRadio
//!
//!

//!
//! \brief The SiXmldata struct
//!
struct SiXmldata{

};

class XmlReader
{
public:
    XmlReader
        (
        DataCollector* collector
        );

    void ReadSiXmlData
        (
        const QString& fileName
        );

    void ReadXsiXmlData
        (
        const QString& fileName
        );

    void ReadPiXmlData
        (
        const QString& fileName
        );
private:
    DataCollector *mCollector;
};

#endif // XMLPARSER_HPP

