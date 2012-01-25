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
#include "views/startscreen.h"

#include <QCoreApplication>
#include <QApplication>
#include <QDesktopWidget>
#include <iostream>

#include <tclap/CmdLine.h>

OpenPdfPresenter::OpenPdfPresenter() {

	this->hasStarted = false;
	
	this->elapsedTime = 0;
	this->currentSlideNumber = 0;
	this->bus = new QEventBus();
	this->configuration = new PresenterConfiguration(this->bus);
	this->bus->subscribe(&RelativeSlideEvent::TYPE,(SlideEventHandler*)this);
	this->bus->subscribe(&AbsoluteSlideEvent::TYPE,(SlideEventHandler*)this);
	this->bus->subscribe(&TimerEvent::TYPE,(ITimerEventHandler*)this);
	this->bus->subscribe(&StartPresentationEvent::TYPE,(StartStopPresentationEventHandler*)this);
	this->bus->subscribe(&StopPresentationEvent::TYPE,(StartStopPresentationEventHandler*)this);
	this->bus->subscribe(&SlideRenderedEvent::TYPE,(SlideRenderedEventHandler*)this);
	this->bus->subscribe(&ResetPresentationEvent::TYPE,(ResetPresentationEventHandler*)this);
	this->bus->subscribe(&SwapScreensEvent::TYPE,(SwapScreensEventHandler*)this);

	this->timer = new Timer(this->bus);

	this->currentConsoleView = this->currentNextView;

	this->powerManagement = new PowerManagement(this->bus);
}

void OpenPdfPresenter::buildViews() {
	this->currentNextView = new CurrentNextSlideConsoleViewImpl();
	this->currentNextNotesView = new CurrentNextSlideNotesConsoleViewImpl();
	this->slideGridView = new SlideGridConsoleViewImpl();
	this->presenterConsoleView = new PresenterConsoleViewImpl();
	this->mainConsoleWindow = new MainWindowViewImpl();
	this->mainSlideWindow = new MainWindowViewImpl();
	this->mainSlideView = new MainSlideViewImpl(QApplication::desktop()->screen(this->configuration->getMainScreen())->geometry().width());
}

void OpenPdfPresenter::buildControllers() {
	this->currentNextController = new CurrentNextSlideConsoleViewControllerImpl(this->bus,this->currentNextView,this);
	this->currentNextNotesController = new CurrentNextSlideNotesConsoleViewControllerImpl(this->bus,this->currentNextNotesView,this);
	this->slideGridController = new SlideGridConsoleViewControllerImpl(this->bus,this->slideGridView,this);
	this->presenterConsoleController = new PresenterConsoleControllerImpl(this->bus, this->presenterConsoleView, this->currentNextController, this->slideGridController, this->currentNextNotesController, this, this->configuration->getTotalSlides(), this->configuration->getTotalTime());
	this->mainConsoleWindowController = new MainWindowViewControllerImpl(this->bus,this->mainConsoleWindow);
	this->mainSlideWindowController = new MainWindowViewControllerImpl(this->bus,this->mainSlideWindow);
	this->mainSlideController = new MainSlideViewControllerImpl(this->bus, this->mainSlideView, this);
}

void OpenPdfPresenter::setUpViews() {
	this->presenterConsoleView->setController(this->presenterConsoleController);
	this->currentNextView->setController(this->currentNextController);
	this->currentNextNotesView->setController(this->currentNextNotesController);
	this->slideGridView->setController(this->slideGridController);
	
	this->mainConsoleWindow->setController(this->mainConsoleWindowController);
	this->mainSlideWindow->setController(this->mainSlideWindowController);
	
	this->mainConsoleWindow->setContent(this->presenterConsoleView->asWidget());
	this->mainSlideWindow->setContent(this->mainSlideView->asWidget());
}

int OpenPdfPresenter::start(int argc, char ** argv) {
	StartScreenViewImpl * startScreenView;
	StartScreenViewControllerImpl * startScreenController;

	startScreenView = new StartScreenViewImpl();
	startScreenController = new StartScreenViewControllerImpl(startScreenView,this->bus);
	startScreenView->setController(startScreenController);

	this->configuration->parseArguments(argc, argv);
	startScreenController->setConfiguration(this->configuration);

	if (!this->configuration->isSkipStartScreen()) {
		startScreenView->show();
	} else {
		if (this->configuration->getDocument() != NULL)
			this->bus->fire(new StartPresentationEvent());
		else
			// TODO: print error
			exit(1);
	}


	int ret = QApplication::instance()->exec();

	delete startScreenController;
	delete startScreenView;

	return ret;
}

void OpenPdfPresenter::onStartPresentation(StartPresentationEvent * evt) {
	QDesktopWidget * desktopWidget = QApplication::desktop();

	this->buildViews();
	this->buildControllers();
	this->setUpViews();
	this->hasStarted = true;

	QRect geometry = desktopWidget->screenGeometry(this->configuration->getAuxScreen());
	this->currentNextController->setGeometry(geometry.width(),geometry.height());
	this->currentNextNotesController->setGeometry(geometry.width(),geometry.height());
	this->slideGridController->setGeometry(geometry.width(),geometry.height());
	this->mainConsoleWindow->move(geometry.topLeft());
	this->mainConsoleWindow->showFullScreen();

	if (!this->configuration->isRehearseMode()) {
		this->mainSlideWindow->move(desktopWidget->screenGeometry(this->configuration->getMainScreen()).topLeft());
		this->mainSlideWindow->showFullScreen();
	}

	this->bus->fire(new SlideChangedEvent(0));
	this->timer->start();
}

OpenPdfPresenter::~OpenPdfPresenter() {
	if (this->hasStarted) {
		// Views
		delete this->currentNextView;
		delete this->slideGridView;
		delete this->presenterConsoleView;
		delete this->mainSlideView;
		delete this->mainConsoleWindow;

		// Controllers
		delete this->presenterConsoleController;
		delete this->currentNextController;
		delete this->mainConsoleWindowController;
		delete this->mainSlideWindowController;
		delete this->mainSlideController;
	}
	delete this->timer;
	delete this->powerManagement;

	delete this->configuration;
	
	delete this->bus;
}


int OpenPdfPresenter::getCurrentSlide() {
	return this->currentSlideNumber;
}

void OpenPdfPresenter::fireSlideChangedEvent() {
	SlideChangedEvent * event = new SlideChangedEvent(
		this->getCurrentSlide());

	this->bus->fire(event);
}

void OpenPdfPresenter::onNextSlide(RelativeSlideEvent * evt) {
	if (this->currentSlideNumber < this->configuration->getTotalSlides() - 1) {
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

	if (toSlide >= 0 && toSlide < this->configuration->getTotalSlides()) {
		this->currentSlideNumber = evt->getSlideNumber();
		this->fireSlideChangedEvent();
	}
}

void OpenPdfPresenter::onTimeout(TimerEvent * evt) {
	this->elapsedTime++;
	this->bus->fire(new TimeChangedEvent(this->elapsedTime,this->configuration->getTotalTime() - this->elapsedTime));
}

Slide OpenPdfPresenter::getSlide(int slideNumber) {
	return this->configuration->getRenderer()->getSlide(slideNumber);
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
	this->bus->fire(new TimeChangedEvent(this->elapsedTime,this->configuration->getTotalTime() - this->elapsedTime));
	this->fireSlideChangedEvent();
}

void OpenPdfPresenter::onSwapScreens(SwapScreensEvent *evt) {
	this->configuration->swapScreens();

	QDesktopWidget * desktopWidget = QApplication::desktop();
	this->mainConsoleWindow->showNormal();
	QRect geometry = desktopWidget->screenGeometry(this->configuration->getAuxScreen());
	this->currentNextController->setGeometry(geometry.width(),geometry.height());
	this->slideGridController->setGeometry(geometry.width(),geometry.height());
	this->mainConsoleWindow->move(geometry.topLeft());
	this->mainConsoleWindow->showFullScreen();
	if (!this->configuration->isRehearseMode()) {
		this->mainSlideWindow->showNormal();
		this->mainSlideWindow->move(desktopWidget->screenGeometry(this->configuration->getMainScreen()).topLeft());
		this->mainSlideWindow->showFullScreen();
	}

	this->configuration->getRenderer()->setGeometry(desktopWidget->screenGeometry(this->configuration->getMainScreen()));
}

QString OpenPdfPresenter::getNotes(int slideNumber) {
	return this->configuration->getParser()->getNotes(slideNumber);
}

PresenterConfiguration * OpenPdfPresenter::getConfiguration() {
	return this->configuration;
}

PresenterConfiguration::PresenterConfiguration(IEventBus * bus) {
	QDesktopWidget * desktopWidget = QApplication::desktop();
	this->mainScreen = desktopWidget->primaryScreen();
	this->auxScreen = desktopWidget->primaryScreen();

	for (int i = 0 ; i < desktopWidget->numScreens() ; i++) {
		if (i != this->mainScreen) {
			this->auxScreen = i;
			break;
		}
	}

	this->parser = NULL;
	this->document = NULL;
	this->renderer = NULL;
	this->bus = bus;
}

PresenterConfiguration::~PresenterConfiguration() {
	delete this->renderer;
	delete this->document;
	delete this->parser;
}

void PresenterConfiguration::parseArguments(int argc, char ** argv) {
	TCLAP::CmdLine cmd("open-pdf-presenter",' ',OPP_VERSION);

	TCLAP::ValueArg<std::string> notesArg("n","notes","Notes file",false,"","XML file");
	TCLAP::ValueArg<int> durationArg("d","duration","Presentation's duration, in seconds",false,0,"Duration");
	TCLAP::SwitchArg rehearseSwitch("r","rehearse","Enable rehearse mode");
	TCLAP::SwitchArg skipStartScreenSwitch("s","skip","Skip start screen");
	TCLAP::UnlabeledValueArg<std::string> pdfFileArg("Presentation","The PDF file with the presentation's slides",false,"","PDF file");

	cmd.add(rehearseSwitch);
	cmd.add(skipStartScreenSwitch);
	cmd.add(durationArg);
	cmd.add(notesArg);
	cmd.add(pdfFileArg);

	try {
		cmd.parse(QCoreApplication::argc(),QCoreApplication::argv());

		this->setRehearseMode(rehearseSwitch.getValue());
		this->skipStartScreen = skipStartScreenSwitch.getValue();
		this->setTotalTime(durationArg.getValue());

		this->document = NULL;
		this->pdfFileName = QString::fromLocal8Bit(pdfFileArg.getValue().c_str());
		if (pdfFileArg.isSet()) {
			// Trigger PDF parsing
			this->setPdfFileName(this->pdfFileName);

			this->notesFileName = QString::fromLocal8Bit(notesArg.getValue().c_str());
			if (notesArg.isSet())
				// Trigger notes parsing
				this->setNotesFileName(this->notesFileName);
		}

	} catch (TCLAP::ArgException &e) {
		std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
	}


}

void PresenterConfiguration::setPdfFileName(QString fileName) {
	if (this->renderer)
		delete this->renderer;

	if (this->document)
		delete this->document;

	this->document = Poppler::Document::load(fileName);
	if (!this->document)
		// TODO: print error
		exit(1);

	this->pdfFileName = fileName;
	this->totalSlides = this->document->numPages();
	//this->document->setRenderHint(Poppler::Document::Antialiasing, true);
	this->document->setRenderHint(Poppler::Document::TextAntialiasing, true);

	this->renderer = new Renderer(this->bus,this->document, QApplication::desktop()->screen(this->mainScreen)->geometry());
	this->renderer->start();

	if (this->parser)
		delete this->parser;

	this->parser = new NotesParser(this->document->numPages());
	this->notesFileName = QString("");
}

void PresenterConfiguration::setNotesFileName(QString fileName) {
	if (!this->document)
		return;

	if (this->parser)
		delete this->parser;

	this->parser = new NotesParser(this->document->numPages());

	if (!this->parser->validateAndParse(fileName))
		// TODO: print error
		exit(1);

	this->notesFileName = fileName;
}

void PresenterConfiguration::setRehearseMode(bool rehearseMode) {
	this->rehearseMode = rehearseMode;
}

void PresenterConfiguration::setTotalTime(int totalTime) {
	this->totalTime = totalTime;
	if (this->totalTime < 0)
		// TODO: print error
		exit(1);
}

void PresenterConfiguration::setMainScreen(int screen) {
	this->mainScreen = screen;
}

void PresenterConfiguration::setAuxScreen(int screen) {
	this->auxScreen = screen;
}

QString PresenterConfiguration::getPdfFileName() {
	return this->pdfFileName;
}

QString PresenterConfiguration::getNotesFileName() {
	return this->notesFileName;
}

Poppler::Document * PresenterConfiguration::getDocument() {
	return this->document;
}

int PresenterConfiguration::getTotalSlides() {
	return this->totalSlides;
}

int PresenterConfiguration::getTotalTime() {
	return this->totalTime;
}

int PresenterConfiguration::getMainScreen() {
	return this->mainScreen;
}

int PresenterConfiguration::getAuxScreen() {
	return this->auxScreen;
}

NotesParser * PresenterConfiguration::getParser() {
	return this->parser;
}

bool PresenterConfiguration::isRehearseMode() {
	return this->rehearseMode;
}

void PresenterConfiguration::swapScreens() {
	int tmp = this->mainScreen;
	this->mainScreen = this->auxScreen;
	this->auxScreen = tmp;
}

bool PresenterConfiguration::isSkipStartScreen() {
	return this->skipStartScreen;
}

Renderer * PresenterConfiguration::getRenderer() {
	return this->renderer;
}
