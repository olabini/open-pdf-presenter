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

#include <iostream>

ControlBarController::ControlBarController(IEventBus * bus, ControlBarView * view, OpenPdfPresenter * presenter, int totalSlideCount, int durationSeconds) {
		this->duration = durationSeconds;
    this->presenter = presenter;
    this->bus = bus;
    this->bus->subscribe(&SlideChangedEvent::TYPE, (SlideChangedEventHandler*)this);
    this->bus->subscribe(&TimeChangedEvent::TYPE, (ITimeChangedEventHandler*)this);
    this->view = view;
    this->view->setController(this);
    this->view->setTotalSlideCount(totalSlideCount+1);
        this->totalSlideCount = totalSlideCount;
}

void ControlBarController::onNextSlideButton() {
    this->fireSlideEvent(1);
}

void ControlBarController::onPrevSlideButton() {
    this->fireSlideEvent(-1);
}

void ControlBarController::fireSlideEvent(int delta) {
    RelativeSlideEvent * event = new RelativeSlideEvent(delta);
    this->bus->fire(event);
}

void ControlBarController::onSlideChanged(SlideChangedEvent * evt) {
        this->view->setCurrentSlideNumber(evt->getCurrentSlideNumber()+1);
        this->view->setSlidePercentage(evt->getCurrentSlideNumber()*100 / this->totalSlideCount);
}

void ControlBarController::onTimeChanged(TimeChangedEvent * evt) {
    int time = evt->getElapsedTime();
		this->view->setTimePercentage(time * 100 / this->duration);
    int hours =  time / 3600;
    time %= 3600;
    int minutes = time / 60;
    time %= 60;
    int seconds = time;
    this->view->setElapsedTime(hours, minutes, seconds);
    time = evt->getRemainingTime();
    hours =  time / 3600;
    time %= 3600;
    minutes = time / 60;
    time %= 60;
    seconds = time;
    this->view->setRemainingTime(hours, minutes, seconds);
}

CurrentNextSlideConsoleViewControllerImpl::CurrentNextSlideConsoleViewControllerImpl(IEventBus * bus, CurrentNextSlideConsoleView * view, OpenPdfPresenter * presenter) {
    this->presenter = presenter;
	this->bus = bus;
	this->bus->subscribe(&SlideChangedEvent::TYPE, (SlideChangedEventHandler*)this);
	this->view = view;
}

void CurrentNextSlideConsoleViewControllerImpl::onSlideChanged(SlideChangedEvent * evt) {
        this->view->setCurrentSlide(this->presenter->getSlide(evt->getCurrentSlideNumber()).asPixmap());

    if (evt->getCurrentSlideNumber() < this->presenter->getTotalSlides())
        this->view->setNextSlide(presenter->getSlide(evt->getCurrentSlideNumber()+1).asPixmap());
}

MainSlideViewControllerImpl::MainSlideViewControllerImpl(IEventBus * bus, MainSlideView * view, OpenPdfPresenter * presenter) {
  this->presenter = presenter;
	this->bus = bus;
	this->bus->subscribe(&SlideChangedEvent::TYPE, (SlideChangedEventHandler*)this);
	this->view = view;
}

void MainSlideViewControllerImpl::onSlideChanged(SlideChangedEvent * evt) {
        this->view->setCurrentSlide(this->presenter->getSlide(evt->getCurrentSlideNumber()).asPixmap());
}
