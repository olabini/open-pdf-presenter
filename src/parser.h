#ifndef PARSER_H
#define PARSER_H

#include <QXmlDefaultHandler>
#include <QAbstractMessageHandler>
#include <QSourceLocation>

class ParserError {
	private:
		QString description;
		QSourceLocation location;

	public:
		ParserError(QString description, QSourceLocation location);

		QString getDescription();
		QSourceLocation getLocation();
};

class ParserException {
	private:
		QList<ParserError> errors;

	public:
		ParserException(QList<ParserError> errors);
		QList<ParserError> getErrors();
};

class NotesParser : public QXmlDefaultHandler, public QAbstractMessageHandler {
	public:
		NotesParser(int slideCount);
		~NotesParser();
		bool startElement(const QString &namespaceURI, const QString &localName, const QString &qName, const QXmlAttributes &atts);
		bool endElement(const QString &namespaceURI, const QString &localName, const QString &qName);
		bool characters(const QString &ch);
		void validateAndParse(QString resource);
		QString getNotes(int slideNumber);
		void handleMessage ( QtMsgType type, const QString & description, const QUrl & identifier, const QSourceLocation & sourceLocation );
	private:
		void setCurrent(QString attrName, QString attrValue);
	private:
		QList<QString> notes;
		QList<ParserError> errors;
		QList<int> * current;
};

#endif // PARSER_H
