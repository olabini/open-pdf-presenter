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
#ifndef _CONTROLLERS_H_
#define _CONTROLLERS_H_

#include "views/view.h"
#include "events/lifecycle.h"
#include "presenter.h"

class OpenPdfPresenter;

class ControlBarController : public ControlBarViewController, public SlideChangedEventHandler, public ITimeChangedEventHandler {
	public:
        ControlBarController(IEventBus * bus, ControlBarView * view, OpenPdfPresenter * presenter, int totalSlideCount, int durationSeconds);
		virtual void onNextSlideButton();
		virtual void onPrevSlideButton();
		virtual void onSlideChanged(SlideChangedEvent * evt);
        virtual void onTimeChanged(TimeChangedEvent * evt);
    private:
        void fireSlideEvent(int delta);
        ControlBarView * view;
        IEventBus * bus;
				int totalSlideCount;
				int duration;
        OpenPdfPresenter * presenter;
};

class CurrentNextSlideConsoleViewControllerImpl : public CurrentNextSlideConsoleViewController, public SlideChangedEventHandler {
	public:
		CurrentNextSlideConsoleViewControllerImpl(IEventBus * bus, CurrentNextSlideConsoleView * view, OpenPdfPresenter * presenter);
		virtual void onSlideChanged(SlideChangedEvent * evt);
	private:
        OpenPdfPresenter * presenter;
		CurrentNextSlideConsoleView * view;
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
    private:
        MainWindowView * view;
        IEventBus * bus;
};

#endif
