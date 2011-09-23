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

OpenPdfPresenter::OpenPdfPresenter(int argc, char ** argv) {
	this->parseArguments(argc, argv);

	QDesktopWidget * desktopWidget = QApplication::desktop();
	this->mainScreen = desktopWidget->primaryScreen();
	this->auxScreen = desktopWidget->primaryScreen();

	for (int i = 0 ; i < desktopWidget->numScreens() ; i++) {
		if (i != this->mainScreen) {
			this->auxScreen = i;
			break;
		}
	}
	
	this->elapsedTime = 0;
	this->currentSlideNumber = 0;
	this->bus = new QEventBus();
	this->bus->subscribe(&RelativeSlideEvent::TYPE,(SlideEventHandler*)this);
	this->bus->subscribe(&AbsoluteSlideEvent::TYPE,(SlideEventHandler*)this);
	this->bus->subscribe(&TimerEvent::TYPE,(ITimerEventHandler*)this);
	this->bus->subscribe(&StopPresentationEvent::TYPE,(StartStopPresentationEventHandler*)this);
	this->bus->subscribe(&SlideRenderedEvent::TYPE,(SlideRenderedEventHandler*)this);
	this->bus->subscribe(&ResetPresentationEvent::TYPE,(ResetPresentationEventHandler*)this);

	this->buildViews();
	this->buildControllers();
	this->setUpViews();
	this->renderer = new Renderer(this->bus,this->document, desktopWidget->screen(this->mainScreen)->geometry());
	this->timer = new Timer(this->bus);
}

void OpenPdfPresenter::buildViews() {
	this->currentNextView = new CurrentNextSlideConsoleViewImpl();
	this->presenterConsoleView = new PresenterConsoleViewImpl();
	this->mainConsoleWindow = new MainWindowViewImpl();
	this->mainSlideWindow = new MainWindowViewImpl();
	this->mainSlideView = new MainSlideViewImpl(QApplication::desktop()->screen(this->mainScreen)->geometry().width());
}

void OpenPdfPresenter::buildControllers() {
	this->presenterConsoleController = new PresenterConsoleControllerImpl(this->bus, this->presenterConsoleView, this, this->totalSlides, this->totalTime);
	this->currentNextController = new CurrentNextSlideConsoleViewControllerImpl(this->bus,this->currentNextView,this);
	this->mainConsoleWindowController = new MainWindowViewControllerImpl(this->bus,this->mainConsoleWindow);
	this->mainSlideWindowController = new MainWindowViewControllerImpl(this->bus,this->mainSlideWindow);
	this->mainSlideController = new MainSlideViewControllerImpl(this->bus, this->mainSlideView, this);
}

void OpenPdfPresenter::setUpViews() {
	this->presenterConsoleView->setController(this->presenterConsoleController);
	this->currentNextView->setController(this->currentNextController);
	this->presenterConsoleView->setContent(this->currentNextView);
	
	this->mainConsoleWindow->setController(this->mainConsoleWindowController);
	this->mainSlideWindow->setController(this->mainSlideWindowController);
	
	this->mainConsoleWindow->setContent(this->presenterConsoleView->asWidget());
	this->mainSlideWindow->setContent(this->mainSlideView->asWidget());
}

int OpenPdfPresenter::start() {
	QDesktopWidget * desktopWidget = QApplication::desktop();
	this->mainConsoleWindow->move(desktopWidget->screenGeometry(this->auxScreen).topLeft());
	this->mainConsoleWindow->showFullScreen();
	this->mainSlideWindow->move(desktopWidget->screenGeometry(this->mainScreen).topLeft());
	this->mainSlideWindow->showFullScreen();

	this->bus->fire(new SlideChangedEvent(0));
	this->renderer->start();

	return QApplication::instance()->exec();
}

OpenPdfPresenter::~OpenPdfPresenter() {
	delete this->renderer;
	delete this->timer;
	delete this->document;
	
	// Views
	delete this->currentNextView;
	delete this->presenterConsoleView;
	delete this->mainSlideView;
	delete this->mainConsoleWindow;
	
	// Controllers
	delete this->presenterConsoleController;
	delete this->currentNextController;
	delete this->mainConsoleWindowController;
	delete this->mainSlideWindowController;
	delete this->mainSlideController;
	
	delete this->bus;
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

Slide OpenPdfPresenter::getSlide(int slideNumber) {
	return this->renderer->getSlide(slideNumber);
}

void OpenPdfPresenter::onStartPresentation(StartPresentationEvent * evt) {
}

void OpenPdfPresenter::onStopPresentation(StopPresentationEvent * evt) {
	QCoreApplication::instance()->exit(0);
}

void OpenPdfPresenter::onSlideRendered(SlideRenderedEvent * evt) {
	if (evt->getSlideNumber() == this->currentSlideNumber)
		this->bus->fire(new SlideChangedEvent(this->currentSlideNumber));
}

void OpenPdfPresenter::onResetPresentation(ResetPresentationEvent * evt) {
	this->currentSlideNumber = 0;
	this->elapsedTime = 0;
	this->bus->fire(new TimeChangedEvent(this->elapsedTime,this->totalTime - this->elapsedTime));
	this->fireSlideChangedEvent();
}
