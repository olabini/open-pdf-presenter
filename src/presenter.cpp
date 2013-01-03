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
#include "views/transitions.h"

#include <QCoreApplication>
#include <QApplication>
#include <QDesktopWidget>
#include <QDebug>
#include <QApplication>

#include <vector>

#include <tclap/CmdLine.h>

#define MAIN_WINDOW_IDX 0
#define AUX_WINDOW_IDX 1

QTextStream cout(stdout);
QTextStream cerr(stderr);

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
	this->mainSlideView = new MainSlideViewImpl(QApplication::desktop()->screen(this->configuration->getMainScreen())->geometry().width(), TransitionFactory::getInstance()->getTransition(this->configuration->getTransitionEffect()));
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
}

int OpenPdfPresenter::start() {
	StartScreenViewImpl * startScreenView;
	StartScreenViewControllerImpl * startScreenController;

	startScreenView = new StartScreenViewImpl();
	startScreenController = new StartScreenViewControllerImpl(startScreenView,this->bus);
	startScreenView->setController(startScreenController);

	this->configuration->parseArguments();

	TransitionFactory::getInstance()->registerTransition(new NoTransition());
	TransitionFactory::getInstance()->registerTransition(new CrossFadingTransition(this->configuration->getTransitionDuration()));

	if (this->configuration->isListTransitions()) {
		this->printAvailableTransitions();
		return 0;
	}

	SlideTransition * transition = TransitionFactory::getInstance()->getTransition(this->configuration->getTransitionEffect());

	if (transition == NULL) {
		cout << "Unknown transition : \"" << this->configuration->getTransitionEffect() << "\"\n";
		this->printAvailableTransitions();
		exit(1);
	}

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

void OpenPdfPresenter::printAvailableTransitions() {
	cout << "Possible transitions:\n";

	foreach(SlideTransition* transition, TransitionFactory::getInstance()->getAllTransitions())
		cout << "\t" << transition->getName() << "\t" << transition->getDesc() << "\n";
}

void OpenPdfPresenter::onStartPresentation(StartPresentationEvent * evt) {
	QDesktopWidget * desktopWidget = QApplication::desktop();

	this->buildViews();
	this->buildControllers();
	this->setUpViews();
	this->hasStarted = true;

	// Assign widgets to windows
	this->widgets[MAIN_WINDOW_IDX] = this->mainSlideView->asWidget();
	this->widgets[AUX_WINDOW_IDX] = this->presenterConsoleView->asWidget();

	this->mainSlideWindow->setContent(this->widgets[MAIN_WINDOW_IDX]);
	this->mainConsoleWindow->setContent(this->widgets[AUX_WINDOW_IDX]);

	this->setWindowPositions();

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
	this->configuration->getRenderer()->setGeometry(desktopWidget->screenGeometry(this->configuration->getMainScreen()));
	this->updateWidgetSizes();
}

void OpenPdfPresenter::updateWidgetSizes() {

	this->widgets[0]->hide();
	this->widgets[1]->hide();

	qApp->processEvents();

	// Set minimal widget size to avoid windows from growing
	this->currentNextController->setGeometry(10, 10);
	this->currentNextNotesController->setGeometry(10, 10);
	this->slideGridController->setGeometry(10, 10);
	this->mainSlideController->setGeometry(10, 10);

	// Fire slide changed event so that all widgets resize accordingly
	this->bus->fire(new SlideChangedEvent(this->currentSlideNumber));

	// Allow windows to resize
	// If this line is ommitted, the smaller window will grow larger than the screen
	// Later, it will not shrink the the right size and will display with incorrect placement
	qApp->processEvents();

	// Swap widgets
	QWidget * tmp = this->widgets[0];
	this->widgets[0] = this->widgets[1];
	this->widgets[1] = tmp;

	// Reassign widgets to windows
	this->mainSlideWindow->setContent(this->widgets[MAIN_WINDOW_IDX]);
	this->mainConsoleWindow->setContent(this->widgets[AUX_WINDOW_IDX]);

	// Fire slide changed event so that all widgets resize accordingly
	this->bus->fire(new SlideChangedEvent(this->currentSlideNumber));

	// Resize widgets using controllers
	QDesktopWidget * desktopWidget = QApplication::desktop();
	QRect geometry = desktopWidget->screenGeometry(this->configuration->getAuxScreen());
	this->currentNextController->setGeometry(geometry.width(),geometry.height());
	this->currentNextNotesController->setGeometry(geometry.width(),geometry.height());
	this->slideGridController->setGeometry(geometry.width(),geometry.height());
	geometry = desktopWidget->screenGeometry(this->configuration->getMainScreen());
	this->mainSlideController->setGeometry(geometry.width(),geometry.height());

	// Fire slide changed event so that all widgets resize accordingly
	this->bus->fire(new SlideChangedEvent(this->currentSlideNumber));

	qDebug() << "Main widget size " << this->mainSlideView->size();
	qDebug() << "Console widget size " << this->presenterConsoleView->size();

	qApp->processEvents();

	this->widgets[0]->show();
	this->widgets[1]->show();
}

void OpenPdfPresenter::setWindowPositions() {
	QDesktopWidget * desktopWidget = QApplication::desktop();
	this->mainConsoleWindow->showNormal();
	QRect geometry = desktopWidget->screenGeometry(this->configuration->getAuxScreen());
	this->mainConsoleWindow->move(geometry.x(), geometry.y());
	this->mainConsoleWindow->setGeometry(geometry);
	qDebug() << "Moved aux window to position " << this->mainConsoleWindow ->geometry();
	this->mainConsoleWindow->showFullScreen();
	if (!this->configuration->isRehearseMode()) {
		this->mainSlideWindow->showNormal();
		geometry = desktopWidget->screenGeometry(this->configuration->getMainScreen());
		this->mainSlideWindow->move(geometry.x(), geometry.y());
		this->mainSlideWindow->setGeometry(geometry);
		qDebug() << "Moved main window to position " << this->mainSlideWindow->geometry();
		this->mainSlideWindow->showFullScreen();
	}
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

void PresenterConfiguration::parseArguments() {
	TCLAP::CmdLine cmd("open-pdf-presenter",' ',OPP_VERSION);

	TCLAP::ValueArg<std::string> notesArg("n","notes","Notes file",false,"","XML file");
	TCLAP::ValueArg<int> durationArg("d","duration","Presentation's duration, in seconds",false,0,"Duration");
	TCLAP::ValueArg<int> transitionDuration("t","transition","Duration of the transition effect between slides, in mseconds",false,200,"Transition duration");
	TCLAP::ValueArg<std::string> transitionEffect("e","effect","Transition effect to use during presentation",false,"crossfade","Transition Effect");
	TCLAP::SwitchArg listAvailableTransitions("l","list","List available transitions and exit");
	TCLAP::SwitchArg rehearseSwitch("r","rehearse","Enable rehearse mode");
	TCLAP::SwitchArg skipStartScreenSwitch("s","skip","Skip start screen");
	TCLAP::UnlabeledValueArg<std::string> pdfFileArg("Presentation","The PDF file with the presentation's slides",false,"","PDF file");

	cmd.add(rehearseSwitch);
	cmd.add(skipStartScreenSwitch);
	cmd.add(durationArg);
	cmd.add(transitionDuration);
	cmd.add(transitionEffect);
	cmd.add(listAvailableTransitions);
	cmd.add(notesArg);
	cmd.add(pdfFileArg);

	// Convert arguments from QStringList to std::vector<std::string>
	std::vector<std::string> arguments;
	foreach (QString s, QCoreApplication::arguments()) {
		arguments.push_back(s.toStdString());
	}

	try {
		cmd.parse(arguments);

		this->setListTransitions(listAvailableTransitions.getValue());
		this->setRehearseMode(rehearseSwitch.getValue());
		this->skipStartScreen = skipStartScreenSwitch.getValue();
		this->setTotalTime(durationArg.getValue());
		this->setTransitionDuration(transitionDuration.getValue());
		this->setTransitionEffect(QString::fromStdString(transitionEffect.getValue()));

		this->document = NULL;
		this->pdfFileName = QString::fromStdString(pdfFileArg.getValue());
		if (pdfFileArg.isSet()) {
			// Trigger PDF parsing
			this->setPdfFileName(this->pdfFileName);

			this->notesFileName = QString::fromStdString(notesArg.getValue());
			if (notesArg.isSet())
				// Trigger notes parsing
				this->setNotesFileName(this->notesFileName);
		}

	} catch (TCLAP::ArgException &e) {
		cerr << "error: " << QString::fromStdString(e.error()) << " for arg " << QString::fromStdString(e.argId()) << endl;
	}


}
void PresenterConfiguration::setListTransitions(bool list) {
	this->listTransitions = list;
}

bool PresenterConfiguration::isListTransitions() {
	return this->listTransitions;
}


void PresenterConfiguration::setTransitionEffect(QString effect) {
	this->transitionEffect = effect;
}

QString PresenterConfiguration::getTransitionEffect() {
	return this->transitionEffect;
}

void PresenterConfiguration::setTransitionDuration(int duration) {
	this->transitionDuration = duration;
}

int PresenterConfiguration::getTransitionDuration() {
	return this->transitionDuration;
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
	this->document->setRenderHint(Poppler::Document::Antialiasing, true);
	this->document->setRenderHint(Poppler::Document::TextAntialiasing, true);

	this->renderer = new Renderer(this->bus,this->document, QApplication::desktop()->screenGeometry(this->mainScreen));
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
