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
#ifndef _PRESENTER_H_
#define _PRESENTER_H_

#include "events/event.h"
#include "events/slide.h"
#include "events/timer.h"
#include "events/lifecycle.h"
#include "views/view.h"
#include "views/console.h"
#include "views/mainwindow.h"
#include "views/mainslide.h"
#include "renderer.h"
#include "controllers.h"
#include "parser.h"
#include "powermanagement.h"

#include <QList>
#include <QWidget>
#include <poppler-qt4.h>

class PresenterConfiguration {
	private:
		int totalSlides;
		int totalTime;
		int mainScreen, auxScreen;
		bool rehearseMode, windowedMode;
		bool skipStartScreen;
		int transitionDuration;
		bool listTransitions;

		QString transitionEffect;
		QString pdfFileName;
		QString notesFileName;
		Poppler::Document * document;
		NotesParser * parser;
		Renderer * renderer;
		IEventBus * bus;

	public: //constructors / destructors
		PresenterConfiguration(IEventBus * bus);
		~PresenterConfiguration();

	public: // getters
		QString getPdfFileName();
		QString getNotesFileName();
		Poppler::Document * getDocument();
		NotesParser * getParser();
		Renderer * getRenderer();
		int getTotalSlides();
		int getTotalTime();
		int getMainScreen();
		int getAuxScreen();
		bool isRehearseMode();
		bool isWindowedMode();
		bool isSkipStartScreen();
		int getTransitionDuration();
		QString getTransitionEffect();
		bool isListTransitions();

	public: // setters
		void setPdfFileName(QString fileName);
		void setNotesFileName(QString fileName);
		void setTotalTime(int totalTime);
		void setMainScreen(int screen);
		void setAuxScreen(int screen);
		void setRehearseMode(bool rehearseMode);
		void setWindowedMode(bool windowedMode);
		void setTransitionDuration(int duration);
		void setTransitionEffect(QString duration);
		void setListTransitions(bool list);

	public: // misc
		void swapScreens();
		void parseArguments();
};

class OpenPdfPresenter : public SlideEventHandler, public ITimerEventHandler, public StartStopPresentationEventHandler, public SlideRenderedEventHandler, public ResetPresentationEventHandler, public SwapScreensEventHandler {
	public:
		OpenPdfPresenter();
		~OpenPdfPresenter();
		int getCurrentSlide();
		int getTotalSlides();
		int getTotalTimeSeconds();
		PresenterConfiguration * getConfiguration();
		Slide getSlide(int slideNumber);
		QString getNotes(int slideNumber);
		virtual void onNextSlide(RelativeSlideEvent * evt);
		virtual void onPrevSlide(RelativeSlideEvent * evt);
		virtual void onGotoSlide(AbsoluteSlideEvent * evt);
		virtual void onTimeout(TimerEvent * evt);
		virtual void onStartPresentation(StartPresentationEvent * evt);
		virtual void onStopPresentation(StopPresentationEvent * evt);
		virtual void onSlideRendered(SlideRenderedEvent * evt);
		virtual void onResetPresentation(ResetPresentationEvent * evt);
		virtual void onSwapScreens(SwapScreensEvent * evt);
		int start();
	private:
		int currentSlideNumber;
		int elapsedTime;
		PresenterConfiguration * configuration;
		bool hasStarted;

		QEventBus * bus;
		Timer * timer;

		ConsoleView * currentConsoleView;
		PowerManagement * powerManagement;

		QWidget * widgets[2];

		void fireSlideChangedEvent();
		void buildViews();
		void buildControllers();
		void setUpViews();
		void setWindowPositions();
		void printAvailableTransitions();
		void updateWidgetSizes();

	private: // views
		PresenterConsoleViewImpl * presenterConsoleView;
		CurrentNextSlideConsoleViewImpl * currentNextView;
		CurrentNextSlideNotesConsoleViewImpl * currentNextNotesView;
		SlideGridConsoleViewImpl * slideGridView;
		MainSlideViewImpl * mainSlideView;
		MainWindowViewImpl * mainConsoleWindow, * mainSlideWindow;

	private: // controllers
		PresenterConsoleControllerImpl * presenterConsoleController;
		CurrentNextSlideConsoleViewControllerImpl * currentNextController;
		CurrentNextSlideNotesConsoleViewControllerImpl * currentNextNotesController;
		SlideGridConsoleViewControllerImpl * slideGridController;
		MainSlideViewControllerImpl * mainSlideController;
		MainWindowViewControllerImpl * mainConsoleWindowController, * mainSlideWindowController;

};

#endif
