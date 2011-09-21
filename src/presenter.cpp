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
#include <QApplication>
#include <QDesktopWidget>
#include <iostream>

#define TEST_DPI 96.0

OpenPdfPresenter::OpenPdfPresenter(int argc, char ** argv, IEventBus * bus) {
		this->parseArguments(argc, argv);

		QList<ScaleFactor*> * factors = this->computeScaleFactors();

		this->scaleFactor = NULL;

		for(int i = 0 ; i < factors->size() ; i++) {
			ScaleFactor * factor = factors->at(i);
			if (this->scaleFactor == NULL || factor->usableArea < this->scaleFactor->usableArea)
				this->scaleFactor = factor;
		}

		for(int i = 0 ; i < factors->size() ; i++) {
			ScaleFactor * factor = factors->at(i);
			if (this->scaleFactor != factor)
				delete factor;
		}

		delete factors;

    this->elapsedTime = 0;
    this->currentSlideNumber = 0;
    this->bus = bus;
    this->bus->subscribe(&RelativeSlideEvent::TYPE,(SlideEventHandler*)this);
    this->bus->subscribe(&AbsoluteSlideEvent::TYPE,(SlideEventHandler*)this);
    this->bus->subscribe(&TimerEvent::TYPE,(ITimerEventHandler*)this);

        this->renderer = new Renderer(this->bus,this->document,this->scaleFactor);
}

OpenPdfPresenter::~OpenPdfPresenter() {
        delete this->renderer;
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

		this->document->setRenderHint(Poppler::Document::TextAntialiasing, true);
    //this->document->setRenderHint(Poppler::Document::Antialiasing, true);

                this->totalSlides = this->document->numPages() - 1;


}


QList<ScaleFactor*> * OpenPdfPresenter::computeScaleFactors() {

		Poppler::Page * page = this->document->page(0);
		QImage image = page->renderToImage(TEST_DPI, TEST_DPI);
		delete page;
			
		double ratio = ((double) image.width()) / ((double)image.height());

		QDesktopWidget * desktopWidget = QApplication::desktop();

		QList<ScaleFactor*> * ret = new QList<ScaleFactor*>();

                for (int i = 0 ; i < desktopWidget->screenCount(); i++) {

			ScaleFactor * scaleFactor = new ScaleFactor();

			scaleFactor->screen = i;

                        QRect screen = desktopWidget->screenGeometry(i);
                        double screenRatio = ((double) screen.width()) / ((double) screen.height());
                        if (screenRatio < ratio) {
                                scaleFactor->usableWidth = screen.width();
                                scaleFactor->usableHeight = ((double) screen.width()) / ratio;
                        }
                        else  if (screenRatio > ratio) {
                                scaleFactor->usableHeight = screen.height();
                                scaleFactor->usableWidth = ((double) screen.height()) * ratio;
                        }
                        else {
                                scaleFactor->usableWidth = screen.width();
                                scaleFactor->usableHeight = screen.height();
                        }

			scaleFactor->usableArea = scaleFactor->usableWidth * scaleFactor->usableHeight;
                        scaleFactor->xScaleFactor = ((double) scaleFactor->usableWidth / ((double) image.width())) * TEST_DPI;
                        scaleFactor->yScaleFactor = ((double) scaleFactor->usableHeight / ((double) image.height())) * TEST_DPI;


			ret->append(scaleFactor);
		}

		return ret;
}

int OpenPdfPresenter::getCurrentSlide() {
    return this->currentSlideNumber;
}

int OpenPdfPresenter::getTotalSlides() {
    return this->totalSlides;
}

void OpenPdfPresenter::fireSlideChangedEvent() {
	SlideChangedEvent * event = new SlideChangedEvent(
		this->getCurrentSlide());

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

Slide * OpenPdfPresenter::getSlide(int slideNumber) {
        return this->renderer->getSlide(slideNumber);
}

ScaleFactor * OpenPdfPresenter::getScaleFactor() {
	return this->scaleFactor;
}
