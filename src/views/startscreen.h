#ifndef STARTSCREEN_H
#define STARTSCREEN_H

#include "view.h"
#include "ui_start-screen.h"

class StartScreenViewImpl : public QWidget, public StartScreenView {
	Q_OBJECT

	public:
		StartScreenViewImpl(QWidget * parent = 0);
		virtual void setController(StartScreenViewController * controller);
		virtual QWidget * asWidget();

		virtual void setPdfFileName(QString fileName);
		virtual void setNotesFileName(QString fileName);
		virtual void setHours(int hours);
		virtual void setMinutes(int minutes);
		virtual void setSeconds(int seconds);
		virtual int getHours();
		virtual int getMinutes();
		virtual int getSeconds();
		virtual void setPdfTitle(QString title);
		virtual void setPdfTotalPages(int totalPages);

	private:
		StartScreenViewController * controller;
		Ui::StartScreen ui;

	private slots:
		void onStartButtonClick();
		void onCancelButtonClick();
		void onPdfFileBrowseButtonClick();
		void onNotesFileBrowseButtonClick();
		void onNotesFileDiscard();
};

#endif // STARTSCREEN_H
