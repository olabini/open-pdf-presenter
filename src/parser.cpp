#include "parser.h"

#include <QtXmlPatterns/QXmlSchema>
#include <QtXmlPatterns/QXmlSchemaValidator>

#include <QList>

ParserError::ParserError(QString description, QSourceLocation location) : description(description), location(location) {
}

QString ParserError::getDescription() {
	return this->description;
}

QSourceLocation ParserError::getLocation() {
	return this->location;
}

ParserException::ParserException(QList<ParserError> errors) : errors(QList<ParserError>(errors)) {
}

QList<ParserError> ParserException::getErrors() {
	return this->errors;
}

NotesParser::NotesParser(int slideCount) : notes(QList<QString>()), errors(QList<ParserError>()) {
	for (int i = 0 ; i < slideCount ; i++)
		this->notes.append("");
}

NotesParser::~NotesParser() {
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
		if (idx < 0 || idx > this->notes.size())
			continue;

		if (this->notes.at(idx) == "")
			this->notes.replace(idx,ch);
	}

	return true;
}

void NotesParser::validateAndParse(QString resource) {
	QXmlSchema schema;
	schema.load( QUrl::fromLocalFile(":/notes/notes.xsd"));

	if (!schema.isValid())
		//TODO: print error
		throw ParserException(this->errors);

	QXmlSchemaValidator validator(schema);
	validator.setMessageHandler(this);
	if (!validator.validate(QUrl(resource))) {
		throw ParserException(this->errors);
	}

	QFile file(resource);
	QXmlInputSource source(&file);

	QXmlSimpleReader reader;
	reader.setContentHandler(this);

	this->current = new QList<int>();
	reader.parse(source);
	delete this->current;

	return;
}

void NotesParser::handleMessage ( QtMsgType type, const QString & description, const QUrl & identifier, const QSourceLocation & sourceLocation ) {
	this->errors.append(ParserError(description, sourceLocation));
}

QString NotesParser::getNotes(int slideNumber) {
	return this->notes.at(slideNumber);
}
