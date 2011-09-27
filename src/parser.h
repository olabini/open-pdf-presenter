#ifndef PARSER_H
#define PARSER_H

#include <QXmlDefaultHandler>

class NotesParser : public QXmlDefaultHandler {
	public:
		NotesParser(int slideCount);
		~NotesParser();
		bool startElement(const QString &namespaceURI, const QString &localName, const QString &qName, const QXmlAttributes &atts);
		bool endElement(const QString &namespaceURI, const QString &localName, const QString &qName);
		bool characters(const QString &ch);
		bool validateAndParse(QString resource);
		QString getNotes(int slideNumber);
	private:
		void setCurrent(QString attrName, QString attrValue);
	private:
		QList<QString> * notes;
		QList<int> * current;
};

#endif // PARSER_H
