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
#ifndef _VIEW_H_
#define _VIEW_H_

#include <QWidget>
#include <QString>
#include "../renderer.h"

class Controller {
};

template<class C> class View {
	public:
		virtual void setController(C * controller) = 0;
		virtual QWidget * asWidget() = 0;
	protected:
		~View() {}
};

class MainWindowViewController : public Controller {
	public:
		virtual void onKeyExit() = 0;
		virtual void onKeyNext() = 0;
		virtual void onKeyPrev() = 0;
		virtual void onKeyReset() = 0;
		virtual void onKeyToggleSlideGrid() = 0;
		virtual void onKeyToggleNotes() = 0;
		virtual void onKeySwapScreens() = 0;
		virtual void onKeyBlackScreen() = 0;
		virtual void onKeyWhiteScreen() = 0;
		virtual void onKeyIncFontSize() = 0;
		virtual void onKeyDecFontSize() = 0;
	protected:
		~MainWindowViewController() {}
};

class MainWindowView : public View<MainWindowViewController> {
	public:
		virtual void setContent(QWidget * content) = 0;
		virtual void clearContent() = 0;
	protected:
		~MainWindowView() {}
};

class PresenterConsoleViewController : public Controller {
	public:
		virtual void onNextSlideButton() = 0;
		virtual void onPrevSlideButton() = 0;
		virtual void onSlideGridButton() = 0;
		virtual void onNotesButton() = 0;
	protected:
		~PresenterConsoleViewController() {}
};

class PresenterConsoleView : public View<PresenterConsoleViewController> {
	public:
		virtual void setElapsedTime(int hours, int minutes, int seconds) = 0;
		virtual void setSlidePercentage (int percentage) = 0;
		virtual void setRemainingTime(int hours, int minutes, int seconds, bool overtime) = 0;
		virtual void setTimePercentage (int percentage) = 0;
		virtual void setTotalSlideCount(int count) = 0;
		virtual void setCurrentSlideNumber(int currentSlide) = 0;
		virtual void addContent(QWidget * view) = 0;
		virtual void setConfirmExitVisible(bool isVisible) = 0;
	protected:
		~PresenterConsoleView() {}
};

class CurrentNextSlideConsoleViewController : public Controller {
	public:
		virtual void setGeometry(int width, int height) = 0;
	protected:
		~CurrentNextSlideConsoleViewController() { }
};

class ConsoleView {
	protected:
		~ConsoleView() { }
};

class CurrentNextSlideConsoleView : public ConsoleView, public View<CurrentNextSlideConsoleViewController> {

	public:
		virtual void setGeometry(int width, int height) = 0;
		virtual void setCurrentSlide(Slide slide) = 0;
		virtual void setNextSlide(Slide slide) = 0;

	protected:
		~CurrentNextSlideConsoleView() { }
};

class CurrentNextSlideNotesConsoleViewController : public Controller {
	public:
		virtual void setGeometry(int width, int height) = 0;
		virtual void onIncNoteFontSizeButton() = 0;
		virtual void onDecNoteFontSizeButton() = 0;
	protected:
		~CurrentNextSlideNotesConsoleViewController() { }
};

class CurrentNextSlideNotesConsoleView : public ConsoleView, public View<CurrentNextSlideNotesConsoleViewController> {

	public:
		virtual void setGeometry(int width, int height) = 0;
		virtual void setCurrentSlide(Slide slide) = 0;
		virtual void setNextSlide(Slide slide) = 0;
		virtual void setNotes(QString notes) = 0;
		virtual int getNotesFontSize() = 0;
		virtual void setNotesFontSize(int size) = 0;

	protected:
		~CurrentNextSlideNotesConsoleView() { }
};

class SlideGridConsoleViewController {
	public:
		virtual void setGeometry(int width, int height) = 0;
		virtual void onSelectSlide(int slideNumber) = 0;
	protected:
		~SlideGridConsoleViewController() { }
};

class SlideGridConsoleView : public ConsoleView, public View<SlideGridConsoleViewController> {
	public:
		virtual void setGeometry(int width, int height) = 0;
		virtual void setTotalNumberOfSlides(int total) = 0;
		virtual void setCurrentSlide(int slideNumber) = 0;
		virtual void setSlide(int slideNumber, Slide slide) = 0;
	protected:
		~SlideGridConsoleView() { }
};

class MainSlideViewController : public Controller {
	public:
		virtual void setGeometry(int width, int height) = 0;
	protected:
		~MainSlideViewController() { }
};

class MainSlideView : public View<MainSlideViewController> {
	public:
		virtual void setCurrentSlide(QPixmap slide) = 0;
		virtual void setBlackBlankScreen() = 0;
		virtual void setWhiteBlankScreen() = 0;
	protected:
		~MainSlideView() { }
};

class StartScreenViewController : public Controller {
	public:
		virtual void browsePresentation() = 0;
		virtual void browseNotes() = 0;
		virtual void discardNotes() = 0;
		virtual void ok() = 0;
		virtual void quit() = 0;
		virtual void about() = 0;
		virtual void setSlidePreview(int slide) = 0;

	protected:
		~StartScreenViewController() { }
};

class StartScreenView : public View<StartScreenViewController> {
	public:
		virtual void setPdfFileName(QString fileName) = 0;
		virtual void setNotesFileName(QString fileName) = 0;
		virtual void setHours(int hours) = 0;
		virtual void setMinutes(int minutes) = 0;
		virtual void setSeconds(int seconds) = 0;
		virtual void setRehearse(bool isRehearse) = 0;
		virtual void setWindowed(bool isWindowed) = 0;
		virtual int getHours() = 0;
		virtual int getMinutes() = 0;
		virtual int getSeconds() = 0;
		virtual bool isRehearse() = 0;
		virtual bool isWindowed() = 0;
		virtual void setPdfTitle(QString title) = 0;
		virtual void setPdfTotalPages(int totalPages) = 0;
		virtual void setCurrentSlideNumber(int currentSlide) = 0;
		virtual void setSlidePreview(Slide slide) = 0;

	protected:
		~StartScreenView() { }
};

#endif
