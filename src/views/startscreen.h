/*
		This file is part of open-pdf-presenter.

		open-pdf-presenter is free software: you can redistribute it and/or modify
		it under the terms of the GNU General Public License as published by
		the Free Software Foundation, either version 3 of the License, or
		(at your option) any later version.

		open-pdf-presenter is distributed in the hope that it will be useful,
		but WITHOUT ANY WARRANTY; without even the implied warranty of
		MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
		GNU General Public License for more details.

		You should have received a copy of the GNU General Public License
		along with open-pdf-presenter.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef STARTSCREEN_H
#define STARTSCREEN_H

#include "view.h"
#include "ui_start-screen.h"
#include <QRect>

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
		virtual bool isRehearse();
		virtual bool isWindowed();
		virtual void setPdfTitle(QString title);
		virtual void setPdfTotalPages(int totalPages);
		virtual void setCurrentSlideNumber(int currentSlide);
		virtual void setSlidePreview(Slide slide);
		virtual void setRehearse(bool isRehearse);
		virtual void setWindowed(bool isWindowed);

	private:
		StartScreenViewController * controller;
		Ui::StartScreen ui;
		QRect previewArea;

	private slots:
		void onStartButtonClick();
		void onCancelButtonClick();
		void onPdfFileBrowseButtonClick();
		void onNotesFileBrowseButtonClick();
		void onNotesFileDiscard();
		void onSliderMove(int position);
		void onAboutClick();
};

#endif // STARTSCREEN_H
