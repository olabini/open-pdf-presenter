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

#include <QList>
#include <poppler-qt4.h>

class OpenPdfPresenter : public SlideEventHandler, public ITimerEventHandler, public StartStopPresentationEventHandler, public SlideRenderedEventHandler, public ResetPresentationEventHandler {
	public:
		OpenPdfPresenter(int argc, char ** argv);
		~OpenPdfPresenter();
		int getCurrentSlide();
		int getTotalSlides();
		int getTotalTimeSeconds();
		Slide getSlide(int slideNumber);
		virtual void onNextSlide(RelativeSlideEvent * evt);
		virtual void onPrevSlide(RelativeSlideEvent * evt);
		virtual void onGotoSlide(AbsoluteSlideEvent * evt);
		virtual void onTimeout(TimerEvent * evt);
		virtual void onStartPresentation(StartPresentationEvent * evt);
		virtual void onStopPresentation(StopPresentationEvent * evt);
		virtual void onSlideRendered(SlideRenderedEvent * evt);
		virtual void onResetPresentation(ResetPresentationEvent * evt);
		int start();
	private:
		int currentSlideNumber;
		int totalSlides;
		int elapsedTime;
		int totalTime;
		int mainScreen, auxScreen;
				
		QString pdfFileName;
		QEventBus * bus;
		Timer * timer;
		Poppler::Document * document;

		Renderer * renderer;

		void parseArguments(int argc, char ** argv);
		void fireSlideChangedEvent();
		void buildViews();
		void buildControllers();
		void setUpViews();

	private: // views
		PresenterConsoleViewImpl * presenterConsoleView;
		CurrentNextSlideConsoleViewImpl * currentNextView;
		SlideGridConsoleViewImpl * slideGridView;
		MainSlideViewImpl * mainSlideView;
		MainWindowViewImpl * mainConsoleWindow, * mainSlideWindow;

	private: // controllers
		PresenterConsoleControllerImpl * presenterConsoleController;
		CurrentNextSlideConsoleViewControllerImpl * currentNextController;
		SlideGridConsoleViewControllerImpl * slideGridController;
		MainSlideViewControllerImpl * mainSlideController;
		MainWindowViewControllerImpl * mainConsoleWindowController, * mainSlideWindowController;

};

#endif
