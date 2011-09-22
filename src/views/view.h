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
	protected:
		~MainWindowViewController() {}
};

class PresenterConsoleView;

class MainWindowView : public View<MainWindowViewController> {
	public:
		virtual void setContent(QWidget * content) = 0;
	protected:
		~MainWindowView() {}
};

class ControlBarView;

class PresenterConsoleViewController : public Controller {
	protected:
		~PresenterConsoleViewController() {}
};

class PresenterConsoleView : public View<PresenterConsoleViewController> {
	public:
		virtual void setContent(QWidget * view) = 0;
		virtual void setControlBarView(ControlBarView * view) = 0;
	protected:
		~PresenterConsoleView() {}
};

class ControlBarViewController: public Controller {
	public:
		virtual void onNextSlideButton() = 0;
		virtual void onPrevSlideButton() = 0;
	protected:
		~ControlBarViewController() {}
};

class ControlBarView : public View<ControlBarViewController> {
	public:
		virtual void setElapsedTime(int hours, int minutes, int seconds) = 0;
		virtual void setSlidePercentage (int percentage) = 0;
		virtual void setRemainingTime(int hours, int minutes, int seconds) = 0;
		virtual void setTimePercentage (int percentage) = 0;
		virtual void setTotalSlideCount(int count) = 0;
		virtual void setCurrentSlideNumber(int currentSlide) = 0;
	protected:
		~ControlBarView() {}
};

class CurrentNextSlideConsoleViewController : public Controller {
};

class CurrentNextSlideConsoleView : public View<CurrentNextSlideConsoleViewController> {

	public:
		virtual void setCurrentSlide(QPixmap slide) = 0;
		virtual void setNextSlide(QPixmap slide) = 0;

	protected:
		~CurrentNextSlideConsoleView() { }
};


class MainSlideViewController : public Controller {
};

class MainSlideView : public View<MainSlideViewController> {
	public:
		virtual void setCurrentSlide(QPixmap slide, bool scale = false) = 0;
	protected:
		~MainSlideView() { }
};

#endif
