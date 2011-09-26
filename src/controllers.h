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
#ifndef _OPEN_PDF_PRESENTER_CONTROLLERS_H_
#define _OPEN_PDF_PRESENTER_CONTROLLERS_H_

#include "renderer.h"
#include "views/view.h"
#include "events/lifecycle.h"

class OpenPdfPresenter;

class PresenterConsoleControllerImpl : public PresenterConsoleViewController, public SlideChangedEventHandler, public ITimeChangedEventHandler {
	public:
		PresenterConsoleControllerImpl(IEventBus * bus, PresenterConsoleView * view, CurrentNextSlideConsoleView * currentNextView, SlideGridConsoleView * slideGridView, OpenPdfPresenter * presenter, int totalSlideCount, int durationSeconds);
		virtual void onNextSlideButton();
		virtual void onPrevSlideButton();
		virtual void onSlideGridButton();
		virtual void onSlideChanged(SlideChangedEvent * evt);
		virtual void onTimeChanged(TimeChangedEvent * evt);
	private:
		void computeTime(int time, int * hours, int * mins, int * secs);
	private:
		void fireSlideEvent(int delta);
		PresenterConsoleView * view;
		CurrentNextSlideConsoleView * currentNextView;
		SlideGridConsoleView * slideGridView;
		IEventBus * bus;
		int totalSlideCount;
		int duration;
		OpenPdfPresenter * presenter;
};

class CurrentNextSlideConsoleViewControllerImpl : public CurrentNextSlideConsoleViewController, public SlideChangedEventHandler, public SlideRenderedEventHandler {
	public:
		CurrentNextSlideConsoleViewControllerImpl(IEventBus * bus, CurrentNextSlideConsoleView * view, OpenPdfPresenter * presenter);
		virtual void onSlideChanged(SlideChangedEvent * evt);
		virtual void onSlideRendered(SlideRenderedEvent *evt);
	private:
		OpenPdfPresenter * presenter;
		CurrentNextSlideConsoleView * view;
		IEventBus * bus;
		Slide pastLastSlide;
};

class SlideGridConsoleViewControllerImpl : public SlideGridConsoleViewController, public SlideChangedEventHandler, public SlideRenderedEventHandler {
	public:
		SlideGridConsoleViewControllerImpl(IEventBus * bus, SlideGridConsoleView * view, OpenPdfPresenter * presenter);
		virtual void onSlideChanged(SlideChangedEvent * evt);
		virtual void onSlideRendered(SlideRenderedEvent *evt);
		virtual void onSelectSlide(int slideNumber);
	private:
		OpenPdfPresenter * presenter;
		SlideGridConsoleView * view;
		IEventBus * bus;
};

class MainSlideViewControllerImpl : public MainSlideViewController, public SlideChangedEventHandler {
	public:
		MainSlideViewControllerImpl(IEventBus * bus, MainSlideView * view, OpenPdfPresenter * presenter);
		virtual void onSlideChanged(SlideChangedEvent * evt);
	private:
		OpenPdfPresenter * presenter;
		MainSlideView * view;
		IEventBus * bus;
};

class MainWindowViewControllerImpl : public MainWindowViewController {
	public:
		MainWindowViewControllerImpl(IEventBus * bus, MainWindowView * view);
		virtual void onKeyExit();
		virtual void onKeyPrev();
		virtual void onKeyNext();
		virtual void onKeyReset();
	private:
		MainWindowView * view;
		IEventBus * bus;
};

#endif
