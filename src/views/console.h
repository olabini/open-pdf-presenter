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
#ifndef _CONSOLE_H_
#define _CONSOLE_H_

#include <QWidget>
#include <QVBoxLayout>
#include <QPixmap>
#include <QList>
#include "view.h"
#include "viewutils.h"
#include "ui_frame.h"
#include "ui_currentnextslide.h"
#include "ui_controlbar.h"

class PresenterConsoleViewImpl: public QWidget, public PresenterConsoleView {

	Q_OBJECT

	public:
		PresenterConsoleViewImpl(QWidget * parent = 0);
		virtual void setElapsedTime(int hours, int minutes, int seconds);
		virtual void setRemainingTime(int hours, int minutes, int seconds, bool overtime);
		virtual void setSlidePercentage (int percentage);
		virtual void setTotalSlideCount(int count);
		virtual void setTimePercentage (int currentSlide);
		virtual void setCurrentSlideNumber(int currentSlide);
		virtual void setController(PresenterConsoleViewController * controller);
		virtual QWidget * asWidget();
		virtual void setContent(QWidget * view);
	private:
		void refresh();

	private:
		PresenterConsoleViewController * controller;
		QVBoxLayout * layout;
		QWidget * content, * controlBarWrapper;
		Ui::ControlBar * controlBarUi;

	private slots:
		void onNextButtonClick();
		void onPrevButtonClick();
};

class CurrentNextSlideConsoleViewImpl : public QWidget, public CurrentNextSlideConsoleView {
	Q_OBJECT

	public:
		CurrentNextSlideConsoleViewImpl(QWidget * parent = 0);
		virtual void setCurrentSlide(Slide slide);
		virtual void setNextSlide(Slide slide);
		virtual void setController(CurrentNextSlideConsoleViewController * controller);
		virtual QWidget * asWidget();

	private:
		SlideFrame * currentSlideFrame;
		SlideFrame * nextSlideFrame;
		Ui::CurrentNextSlide currentNextSlideUi;
};

class SlideGridConsoleViewImpl : public Frame, public SlideGridConsoleView {
	Q_OBJECT

	public:
		SlideGridConsoleViewImpl(QWidget * parent = 0);
		~SlideGridConsoleViewImpl();
		virtual void setTotalNumberOfSlides(int total);
		virtual void setCurrentSlide(int slideNumber);
		virtual void setSlide(int slideNumber, Slide slide);
		virtual void setController(SlideGridConsoleViewController * controller);
		virtual QWidget * asWidget();

	private:
		void deleteSlides();

	private:
		QGridLayout * layout;
		QList<QLabel*> * slides;
		int selectedSlide;
		int rows, cols;
		SlideGridConsoleViewController * controller;
};

#endif
