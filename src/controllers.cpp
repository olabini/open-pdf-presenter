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
#include "controllers.h"

#include "events/slide.h"
#include "presenter.h"

PresenterConsoleControllerImpl::PresenterConsoleControllerImpl(IEventBus * bus, PresenterConsoleView * view, CurrentNextSlideConsoleView * currentNextView, SlideGridConsoleView * slideGridView, OpenPdfPresenter * presenter, int totalSlideCount, int durationSeconds) {
	this->duration = durationSeconds;
	this->presenter = presenter;
	this->bus = bus;
	this->bus->subscribe(&SlideChangedEvent::TYPE, (SlideChangedEventHandler*)this);
	this->bus->subscribe(&TimeChangedEvent::TYPE, (ITimeChangedEventHandler*)this);
	this->bus->subscribe(&ToggleConsoleViewEvent::TYPE, (ToggleConsoleViewEventHandler*)this);
	this->view = view;
	this->currentNextView = currentNextView;
	this->slideGridView = slideGridView;
	this->view->setController(this);
	this->view->setTotalSlideCount(totalSlideCount);
	this->view->addContent(this->slideGridView->asWidget());
	this->slideGridView->asWidget()->setVisible(false);
	this->view->addContent(this->currentNextView->asWidget());
	this->totalSlideCount = totalSlideCount;
}

void PresenterConsoleControllerImpl::onNextSlideButton() {
	this->fireSlideEvent(1);
}

void PresenterConsoleControllerImpl::onPrevSlideButton() {
	this->fireSlideEvent(-1);
}

void PresenterConsoleControllerImpl::onSlideGridButton() {
	if (this->slideGridView->asWidget()->isVisible()) {
		this->slideGridView->asWidget()->setVisible(false);
		this->currentNextView->asWidget()->setVisible(true);
	} else {
		this->currentNextView->asWidget()->setVisible(false);
		this->slideGridView->asWidget()->setVisible(true);
	}
}

void PresenterConsoleControllerImpl::fireSlideEvent(int delta) {
	this->bus->fire(new RelativeSlideEvent(delta));
}

void PresenterConsoleControllerImpl::onSlideChanged(SlideChangedEvent * evt) {
	this->view->setCurrentSlideNumber(evt->getCurrentSlideNumber()+1);
	this->view->setSlidePercentage(evt->getCurrentSlideNumber()*100 / (this->totalSlideCount - 1));
}

void PresenterConsoleControllerImpl::onTimeChanged(TimeChangedEvent * evt) {
	int time = evt->getElapsedTime();
	this->view->setTimePercentage(time * 100 / this->duration);
	int hours, minutes, seconds;
	this->computeTime(time,&hours,&minutes,&seconds);
	this->view->setElapsedTime(hours, minutes, seconds);
	time = evt->getRemainingTime();
	bool overtime = time < 0;
	if (overtime)
		time *= -1;
	this->computeTime(time,&hours,&minutes,&seconds);
	this->view->setRemainingTime(hours, minutes, seconds, overtime);
}

void PresenterConsoleControllerImpl::onToggleSlideView(ToggleConsoleViewEvent *event) {
	this->onSlideGridButton();
}

void PresenterConsoleControllerImpl::computeTime(int time, int *hours, int *minutes, int *seconds) {
	*hours =  time / 3600;
	time %= 3600;
	*minutes = time / 60;
	time %= 60;
	*seconds = time;
}

CurrentNextSlideConsoleViewControllerImpl::CurrentNextSlideConsoleViewControllerImpl(IEventBus * bus, CurrentNextSlideConsoleView * view, OpenPdfPresenter * presenter) :pastLastSlide(QImage(QString(":/presenter/pastlastslide.svg"))) {
	this->presenter = presenter;
	this->bus = bus;
	this->bus->subscribe(&SlideChangedEvent::TYPE, (SlideChangedEventHandler*)this);
	this->bus->subscribe(&SlideRenderedEvent::TYPE, (SlideRenderedEventHandler*)this);
	this->view = view;
}

void CurrentNextSlideConsoleViewControllerImpl::onSlideChanged(SlideChangedEvent * evt) {
	this->view->setCurrentSlide(this->presenter->getSlide(evt->getCurrentSlideNumber()));

	if (evt->getCurrentSlideNumber() < this->presenter->getTotalSlides() - 1)
		this->view->setNextSlide(presenter->getSlide(evt->getCurrentSlideNumber()+1));
	else
		this->view->setNextSlide(pastLastSlide);
}

void CurrentNextSlideConsoleViewControllerImpl::onSlideRendered(SlideRenderedEvent *evt) {
	if (evt->getSlideNumber() == this->presenter->getCurrentSlide() + 1) {
		this->view->setNextSlide(evt->getSlide());
	}
}

SlideGridConsoleViewControllerImpl::SlideGridConsoleViewControllerImpl(IEventBus *bus, SlideGridConsoleView *view, OpenPdfPresenter *presenter) {
	this->bus = bus;
	this->view = view;
	this->presenter = presenter;
	this->bus->subscribe(&SlideChangedEvent::TYPE, (SlideChangedEventHandler*)this);
	this->bus->subscribe(&SlideRenderedEvent::TYPE, (SlideRenderedEventHandler*)this);

	this->view->setTotalNumberOfSlides(this->presenter->getTotalSlides());
	for (int i = 0 ; i < this->presenter->getTotalSlides() ; i++) {
		this->view->setSlide(i,this->presenter->getSlide(i));
	}
}

void SlideGridConsoleViewControllerImpl::onSlideChanged(SlideChangedEvent *evt) {
}

void SlideGridConsoleViewControllerImpl::onSlideRendered(SlideRenderedEvent *evt) {
	this->view->setSlide(evt->getSlideNumber(),evt->getSlide());
}

void SlideGridConsoleViewControllerImpl::onSelectSlide(int slideNumber) {
	this->bus->fire(new AbsoluteSlideEvent(slideNumber));
}

MainSlideViewControllerImpl::MainSlideViewControllerImpl(IEventBus * bus, MainSlideView * view, OpenPdfPresenter * presenter) {
  this->presenter = presenter;
	this->bus = bus;
	this->bus->subscribe(&SlideChangedEvent::TYPE, (SlideChangedEventHandler*)this);
	this->view = view;
}

void MainSlideViewControllerImpl::onSlideChanged(SlideChangedEvent * evt) {
	QPixmap slide = this->presenter->getSlide(evt->getCurrentSlideNumber()).asPixmap();
	this->view->setCurrentSlide(slide);
}

MainWindowViewControllerImpl::MainWindowViewControllerImpl(IEventBus * bus, MainWindowView * view) {
	this->bus = bus;
	this->view = view;
}

void MainWindowViewControllerImpl::onKeyExit() {
	this->bus->fire(new StopPresentationEvent());
}

void MainWindowViewControllerImpl::onKeyPrev() {
	this->bus->fire(new RelativeSlideEvent(-1));
}

void MainWindowViewControllerImpl::onKeyNext() {
	this->bus->fire(new RelativeSlideEvent(1));
}

void MainWindowViewControllerImpl::onKeyReset() {
	this->bus->fire(new ResetPresentationEvent());
}

void MainWindowViewControllerImpl::onKeyToggleSlideGrid() {
	this->bus->fire(new ToggleSlideGridEvent());
}
