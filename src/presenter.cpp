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

#include <QCoreApplication>

OpenPdfPresenter::OpenPdfPresenter(int argc, char ** argv, IEventBus * bus) {
		this->parseArguments(argc, argv);
    this->elapsedTime = 0;
    this->currentSlideNumber = 0;
		this->loadingSlide = new QPixmap(QString(":/presenter/loadingslide.svg"));
    this->bus = bus;
    this->bus->subscribe(&RelativeSlideEvent::TYPE,(SlideEventHandler*)this);
    this->bus->subscribe(&AbsoluteSlideEvent::TYPE,(SlideEventHandler*)this);
    this->bus->subscribe(&TimerEvent::TYPE,(ITimerEventHandler*)this);
}

OpenPdfPresenter::~OpenPdfPresenter() {
	delete this->document;
}

void OpenPdfPresenter::parseArguments(int argc, char ** argv) {
    this->totalTime = 120;

		QStringList args = QCoreApplication::arguments();
		if (args.size() < 3)
			// TODO: print error
			exit(1);

		bool ok;
		this->totalTime = args.at(1).toInt(&ok);
		if (!ok)
			// TODO: print error
			exit(1);

		this->document = Poppler::Document::load(args.at(2));
		if (!this->document)
			// TODO: print error
			exit(1);

		this->totalSlides = this->document->numPages() - 1;
}

int OpenPdfPresenter::getCurrentSlide() {
    return this->currentSlideNumber + 1;
}

int OpenPdfPresenter::getTotalSlides() {
    return this->totalSlides + 1;
}

void OpenPdfPresenter::fireSlideChangedEvent() {
	Poppler::Page * pdfPage = this->document->page(this->currentSlideNumber);
	SlideChangedEvent * event = new SlideChangedEvent(
		QPixmap::fromImage(pdfPage->renderToImage(500,500)),
		this->getCurrentSlide());
	delete pdfPage;

	this->bus->fire(event);
}

void OpenPdfPresenter::onNextSlide(RelativeSlideEvent * evt) {
    if (this->currentSlideNumber < this->totalSlides) {
			this->currentSlideNumber += 1;
			this->fireSlideChangedEvent();
    }
}

void OpenPdfPresenter::onPrevSlide(RelativeSlideEvent * evt) {
    if (this->currentSlideNumber > 0) {
      this->currentSlideNumber -= 1;
			this->fireSlideChangedEvent();
    }
}

void OpenPdfPresenter::onGotoSlide(AbsoluteSlideEvent * evt) {
    int toSlide = evt->getSlideNumber();

    if (toSlide >= 0 && toSlide < this->totalSlides) {
        this->currentSlideNumber = evt->getSlideNumber();
				this->fireSlideChangedEvent();
    }
}

void OpenPdfPresenter::onTimeout(TimerEvent * evt) {
    this->elapsedTime++;
    this->bus->fire(new TimeChangedEvent(this->elapsedTime,this->totalTime - this->elapsedTime));
}

int OpenPdfPresenter::getTotalTimeSeconds() {
	return this->totalTime;
}
