#include "parser.h"

#include <QtXmlPatterns/QXmlSchema>
#include <QtXmlPatterns/QXmlSchemaValidator>

#include <QList>

NotesParser::NotesParser(int slideCount) {
	this->notes = new QList<QString>();
	for (int i = 0 ; i < slideCount ; i++)
		this->notes->append("");
}

NotesParser::~NotesParser() {
	delete this->notes;
}

bool NotesParser::startElement(const QString &namespaceURI, const QString &localName, const QString &qName, const QXmlAttributes &atts) {
	if (atts.count() == 1)
		this->setCurrent(atts.qName(0),atts.value(0));

	return true;
}

void NotesParser::setCurrent(QString attrName, QString attrValue) {
	this->current->clear();

	if (attrName == "number") {
		QStringList toks = attrValue.split(",");
		QString tok;
		for (int i = 0 ; i < toks.size() ; i++)
			this->current->append(toks.at(i).toInt()-1);
	} else if (attrName == "range") {
		QStringList toks = attrValue.split("-");
		int start, stop;
		start = toks.at(0).toInt();
		stop = toks.at(1).toInt();
		for (int i = start ; i <= stop ; i++)
			this->current->append(i-1);
	}
}

bool NotesParser::endElement(const QString &namespaceURI, const QString &localName, const QString &qName) {
	return true;
}

bool NotesParser::characters(const QString &ch) {
	for (int i = 0 ; i < this->current->size() ; i++) {
		int idx = this->current->at(i);
		if (idx < 0 || idx > this->notes->size())
			continue;

		if (this->notes->at(idx) == "")
			this->notes->replace(idx,ch);
	}

	return true;
}

bool NotesParser::validateAndParse(QString resource) {
	QXmlSchema schema;
	schema.load( QUrl::fromLocalFile(":/notes/notes.xsd"));

	if (!schema.isValid())
		//TODO: print error
		return false;

	QXmlSchemaValidator validator(schema);
	if (!validator.validate(QUrl(resource)))
		//TODO: print error
		return false;

	QFile file(resource);
	QXmlInputSource source(&file);

	QXmlSimpleReader reader;
	reader.setContentHandler(this);

	this->current = new QList<int>();
	reader.parse(source);
	delete this->current;

	return true;
}

QString NotesParser::getNotes(int slideNumber) {
	return this->notes->at(slideNumber);
}
