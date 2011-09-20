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

#define TEST_DPI 96

OpenPdfPresenter::OpenPdfPresenter(int argc, char ** argv, IEventBus * bus) {
		this->parseArguments(argc, argv);
		this->computeScaleFactors();
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

		this->document->setRenderHint(Poppler::Document::TextAntialiasing, true);
    //this->document->setRenderHint(Poppler::Document::Antialiasing, true);

		this->totalSlides = this->document->numPages() - 1;

}


void OpenPdfPresenter::computeScaleFactors() {

		this->xScaleFactor = TEST_DPI;
		this->yScaleFactor = TEST_DPI;

		Poppler::Page * page = this->document->page(0);
		QImage image = page->renderToImage(this->xScaleFactor,this->yScaleFactor);
		delete page;

		double ratio = ((double) image.width()) / ((double)image.height());
		double area = ((double) image.width()) * ((double)image.height());

		QDesktopWidget * desktopWidget = QApplication::desktop();

		double maxUsableArea = 0, maxUsableWidth, maxUsableHeight;

		for (int i = 0 ; i < desktopWidget->screenCount(); i++) {

			double usableArea;
			QRect screen = desktopWidget->screenGeometry(i);
			double screenRatio = ((double) screen.width()) / ((double) screen.height());

			double usableHeight, usableWidth;

			if (screenRatio < ratio) {
				usableWidth = screen.width();
				usableHeight = ((double) screen.height()) / ratio;
			}
			else if (screenRatio > ratio) {
				usableHeight = screen.height();
				usableWidth = ((double) screen.width()) / ratio;
			}
			else {
				usableWidth = screen.width();
				usableHeight = screen.height();
			}

			usableArea = usableWidth * usableHeight;

			if (usableArea > maxUsableArea) {
				maxUsableWidth = usableWidth;
				maxUsableHeight = usableHeight;
				maxUsableArea = usableArea;
			}
		}

		this->xScaleFactor *= (maxUsableWidth / image.width());
		this->yScaleFactor *= (maxUsableHeight / image.height());
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

QPixmap OpenPdfPresenter::getSlide(int slideNumber) {
	Poppler::Page * pdfPage = this->document->page(slideNumber);

	QPixmap pixmap = QPixmap::fromImage(pdfPage->renderToImage(this->xScaleFactor,this->yScaleFactor));

	delete pdfPage;
    return pixmap;
}
