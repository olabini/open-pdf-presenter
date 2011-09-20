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
#include "presenter.h"

#include "events/lifecycle.h"

OpenPdfPresenter::OpenPdfPresenter(int totalTime, int totalSlides, IEventBus * bus) {
    this->totalSlides = totalSlides;
    this->totalTime = totalTime;
    this->elapsedTime = 0;
    this->currentSlideNumber = 0;
		this->loadingSlide = new QPixmap(QString(":/presenter/loadingslide.svg"));
    this->bus = bus;
    this->bus->subscribe(&RelativeSlideEvent::TYPE,(SlideEventHandler*)this);
    this->bus->subscribe(&AbsoluteSlideEvent::TYPE,(SlideEventHandler*)this);
    this->bus->subscribe(&TimerEvent::TYPE,(ITimerEventHandler*)this);
}

int OpenPdfPresenter::getCurrentSlide() {
    return this->currentSlideNumber + 1;
}

int OpenPdfPresenter::getTotalSlides() {
    return this->totalSlides + 1;
}

void OpenPdfPresenter::onNextSlide(RelativeSlideEvent * evt) {
    if (this->currentSlideNumber < this->totalSlides) {
        this->currentSlideNumber += 1;
        SlideChangedEvent * event = new SlideChangedEvent(this->loadingSlide, this->getCurrentSlide());
        this->bus->fire(event);
    }
}

void OpenPdfPresenter::onPrevSlide(RelativeSlideEvent * evt) {
    if (this->currentSlideNumber > 0) {
        this->currentSlideNumber -= 1;
        SlideChangedEvent * event = new SlideChangedEvent(this->loadingSlide, this->getCurrentSlide());
        this->bus->fire(event);
    }
}

void OpenPdfPresenter::onGotoSlide(AbsoluteSlideEvent * evt) {
    int toSlide = evt->getSlideNumber();

    if (toSlide >= 0 && toSlide < this->totalSlides) {
        this->currentSlideNumber = evt->getSlideNumber();
        SlideChangedEvent * event = new SlideChangedEvent(this->loadingSlide, this->getCurrentSlide());
        this->bus->fire(event);
    }
}

void OpenPdfPresenter::onTimeout(TimerEvent * evt) {
    this->elapsedTime++;
    this->bus->fire(new TimeChangedEvent(this->elapsedTime,this->totalTime - this->elapsedTime));
}

int OpenPdfPresenter::getTotalTimeSeconds() {
	return this->totalTime;
}
