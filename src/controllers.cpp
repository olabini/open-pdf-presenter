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

#include <algorithm>
#include <QFileDialog>
#include <QMessageBox>
#include <QDesktopWidget>
#include <QDebug>
#include "events/slide.h"
#include "presenter.h"

static void durationToTime(int duration, int *hours, int *minutes, int *seconds) {
	*hours =  duration / 3600;
	duration %= 3600;
	*minutes = duration / 60;
	duration %= 60;
	*seconds = duration;
}

static int timeToDuration(int hours, int minutes, int seconds) {
	return (hours * 3600) + (minutes * 60) + (seconds);
}

PresenterConsoleState * DefaultConsoleState::onToggleNotesView() {
	delete this;
	return new NotesConsoleState();
}

PresenterConsoleState * DefaultConsoleState::onToggleSlideView() {
	return new SlideGridConsoleState(this);
}

void DefaultConsoleState::apply(CurrentNextSlideConsoleViewControllerImpl *currentNext, CurrentNextSlideNotesConsoleViewControllerImpl *notes, SlideGridConsoleViewControllerImpl *grid) {
	notes->setVisible(false);
	grid->setVisible(false);
	currentNext->setVisible(true);
}

PresenterConsoleState * NotesConsoleState::onToggleNotesView() {
	delete this;
	return new DefaultConsoleState();
}

PresenterConsoleState * NotesConsoleState::onToggleSlideView() {
	return new SlideGridConsoleState(this);
}

void NotesConsoleState::apply(CurrentNextSlideConsoleViewControllerImpl *currentNext, CurrentNextSlideNotesConsoleViewControllerImpl *notes, SlideGridConsoleViewControllerImpl *grid) {
	grid->setVisible(false);
	currentNext->setVisible(false);
	notes->setVisible(true);
}

SlideGridConsoleState::SlideGridConsoleState(PresenterConsoleState * previous) : previousState(previous) {
}

SlideGridConsoleState::~SlideGridConsoleState() {
	delete this->previousState;
}

PresenterConsoleState * SlideGridConsoleState::onToggleNotesView() {
	delete this;
	return new NotesConsoleState();
}

PresenterConsoleState * SlideGridConsoleState::onToggleSlideView() {
	PresenterConsoleState * ret = this->previousState;
	this->previousState = NULL;
	delete this;
	return ret;
}

void SlideGridConsoleState::apply(CurrentNextSlideConsoleViewControllerImpl *currentNext, CurrentNextSlideNotesConsoleViewControllerImpl *notes, SlideGridConsoleViewControllerImpl *grid) {
	currentNext->setVisible(false);
	notes->setVisible(false);
	grid->setVisible(true);
}

ConsoleViewControllerImpl::ConsoleViewControllerImpl() {
	this->visible = false;
}

void ConsoleViewControllerImpl::refresh() {
	if (!this->visible)
		return;

	this->doRefresh();
}

void ConsoleViewControllerImpl::setVisible(bool isVisible) {
	this->visible = isVisible;
	this->doSetVisible(isVisible);
	this->refresh();
}

PresenterConsoleControllerImpl::PresenterConsoleControllerImpl(IEventBus * bus, PresenterConsoleView * view, CurrentNextSlideConsoleViewControllerImpl * currentNext, SlideGridConsoleViewControllerImpl * slideGrid, CurrentNextSlideNotesConsoleViewControllerImpl * currentNextNotes, OpenPdfPresenter * presenter, int totalSlideCount, int durationSeconds) {
	this->duration = durationSeconds;
	this->presenter = presenter;
	this->bus = bus;
	this->bus->subscribe(&SlideChangedEvent::TYPE, (SlideChangedEventHandler*)this);
	this->bus->subscribe(&TimeChangedEvent::TYPE, (ITimeChangedEventHandler*)this);
	this->bus->subscribe(&ToggleConsoleViewEvent::TYPE, (ToggleConsoleViewEventHandler*)this);
	this->view = view;
	this->currentNext= currentNext;
	this->slideGrid= slideGrid;
	this->currentNextNotes= currentNextNotes;
	this->view->setController(this);
	this->view->setTotalSlideCount(totalSlideCount);
	this->currentState = new DefaultConsoleState();
	this->currentState->apply(this->currentNext, this->currentNextNotes, this->slideGrid);
	this->view->addContent(this->slideGrid->content());
	this->view->addContent(this->currentNextNotes->content());
	this->view->addContent(this->currentNext->content());
	this->totalSlideCount = totalSlideCount;
}

PresenterConsoleControllerImpl::~PresenterConsoleControllerImpl() {
	delete this->currentState;
}

void PresenterConsoleControllerImpl::onNextSlideButton() {
	this->fireSlideEvent(1);
}

void PresenterConsoleControllerImpl::onPrevSlideButton() {
	this->fireSlideEvent(-1);
}

void PresenterConsoleControllerImpl::onSlideGridButton() {
	this->currentState = this->currentState->onToggleSlideView();
	this->currentState->apply(this->currentNext, this->currentNextNotes, this->slideGrid);
}

void PresenterConsoleControllerImpl::onNotesButton() {
	this->currentState = this->currentState->onToggleNotesView();
	this->currentState->apply(this->currentNext, this->currentNextNotes, this->slideGrid);
}

void PresenterConsoleControllerImpl::fireSlideEvent(int delta) {
	this->bus->fire(new RelativeSlideEvent(delta));
}

void PresenterConsoleControllerImpl::onSlideChanged(SlideChangedEvent * evt) {
	this->view->setCurrentSlideNumber(evt->getCurrentSlideNumber()+1);
	this->view->setSlidePercentage(evt->getCurrentSlideNumber()*100 / std::max((this->totalSlideCount - 1),1));
}

void PresenterConsoleControllerImpl::onTimeChanged(TimeChangedEvent * evt) {
	int time = evt->getElapsedTime();
	if (this->duration > 0)
		this->view->setTimePercentage(time * 100 / this->duration);
	else
		this->view->setTimePercentage(100);
	int hours, minutes, seconds;
	durationToTime(time,&hours,&minutes,&seconds);
	this->view->setElapsedTime(hours, minutes, seconds);
	time = evt->getRemainingTime();
	bool overtime = time < 0;
	if (overtime)
		time *= -1;
	durationToTime(time,&hours,&minutes,&seconds);
	this->view->setRemainingTime(hours, minutes, seconds, overtime);
}

void PresenterConsoleControllerImpl::onToggleSlideView(ToggleConsoleViewEvent *event) {
	this->onSlideGridButton();
}

void PresenterConsoleControllerImpl::onToggleNotesView(ToggleConsoleViewEvent *event) {
	this->onNotesButton();
}

void PresenterConsoleControllerImpl::onConfirmExit(ToggleConsoleViewEvent *event) {
	this->view->setConfirmExitVisible(((ConfirmExitEvent*)event)->isShow());
}


CurrentNextSlideConsoleViewControllerImpl::CurrentNextSlideConsoleViewControllerImpl(IEventBus * bus, CurrentNextSlideConsoleView * view, OpenPdfPresenter * presenter) :pastLastSlide(QImage(QString(":/presenter/pastlastslide.svg")), QRect()) {
	this->presenter = presenter;
	this->bus = bus;
	this->bus->subscribe(&SlideChangedEvent::TYPE, (SlideChangedEventHandler*)this);
	this->bus->subscribe(&SlideRenderedEvent::TYPE, (SlideRenderedEventHandler*)this);
	this->view = view;
	this->currentSlideNumber = 0;
}

void CurrentNextSlideConsoleViewControllerImpl::doRefresh() {
	if (!this->view->asWidget()->isVisible())
		return;

	this->view->setCurrentSlide(this->presenter->getSlide(this->currentSlideNumber));

	if (this->currentSlideNumber < this->presenter->getConfiguration()->getTotalSlides() - 1)
		this->view->setNextSlide(presenter->getSlide(this->currentSlideNumber + 1));
	else
		this->view->setNextSlide(pastLastSlide);
}

QWidget * CurrentNextSlideConsoleViewControllerImpl::content() {
	return this->view->asWidget();
}

void CurrentNextSlideConsoleViewControllerImpl::doSetVisible(bool isVisible) {
	this->view->asWidget()->setVisible(isVisible);
}

void CurrentNextSlideConsoleViewControllerImpl::onSlideChanged(SlideChangedEvent * evt) {
	this->currentSlideNumber = evt->getCurrentSlideNumber();
	this->refresh();
}

void CurrentNextSlideConsoleViewControllerImpl::onSlideRendered(SlideRenderedEvent *evt) {
	if (evt->getSlideNumber() == this->presenter->getCurrentSlide() + 1) {
		this->view->setNextSlide(evt->getSlide());
		this->refresh();
	}
}

void CurrentNextSlideConsoleViewControllerImpl::setGeometry(int width, int height) {
	this->view->setGeometry(width, height);
	this->refresh();
}


CurrentNextSlideNotesConsoleViewControllerImpl::CurrentNextSlideNotesConsoleViewControllerImpl(IEventBus * bus, CurrentNextSlideNotesConsoleView * view, OpenPdfPresenter * presenter) :pastLastSlide(QImage(QString(":/presenter/pastlastslide.svg")), QRect()) {
	this->presenter = presenter;
	this->bus = bus;
	this->bus->subscribe(&SlideChangedEvent::TYPE, (SlideChangedEventHandler*)this);
	this->bus->subscribe(&SlideRenderedEvent::TYPE, (SlideRenderedEventHandler*)this);
	this->bus->subscribe(&ChangeNoteFontSizeEvent::TYPE, (ChangeNoteFontSizeEventHandler*)this);
	this->view = view;
	this->currentSlideNumber = 0;
}

void CurrentNextSlideNotesConsoleViewControllerImpl::doRefresh() {
	if (!this->view->asWidget()->isVisible())
		return;

	this->view->setCurrentSlide(this->presenter->getSlide(this->currentSlideNumber));

	if (this->currentSlideNumber < this->presenter->getConfiguration()->getTotalSlides() - 1)
		this->view->setNextSlide(presenter->getSlide(this->currentSlideNumber + 1));
	else
		this->view->setNextSlide(pastLastSlide);

	this->view->setNotes(this->presenter->getNotes(this->currentSlideNumber));
}

QWidget * CurrentNextSlideNotesConsoleViewControllerImpl::content() {
	return this->view->asWidget();
}

void CurrentNextSlideNotesConsoleViewControllerImpl::doSetVisible(bool isVisible) {
	this->view->asWidget()->setVisible(isVisible);
}

void CurrentNextSlideNotesConsoleViewControllerImpl::onSlideChanged(SlideChangedEvent * evt) {
	this->currentSlideNumber = evt->getCurrentSlideNumber();
	this->refresh();
}

void CurrentNextSlideNotesConsoleViewControllerImpl::onSlideRendered(SlideRenderedEvent *evt) {
	if (evt->getSlideNumber() == this->presenter->getCurrentSlide() + 1) {
		this->refresh();
	}
}

void CurrentNextSlideNotesConsoleViewControllerImpl::setGeometry(int width, int height) {
	this->view->setGeometry(width, height);
	this->refresh();
}

void CurrentNextSlideNotesConsoleViewControllerImpl::onIncNoteFontSizeButton() {
	this->bus->fire(new ChangeNoteFontSizeEvent(true));
}

void CurrentNextSlideNotesConsoleViewControllerImpl::onDecNoteFontSizeButton() {
	this->bus->fire(new ChangeNoteFontSizeEvent(false));
}

void CurrentNextSlideNotesConsoleViewControllerImpl::onChangeNoteFontSize(ChangeNoteFontSizeEvent *event) {
	this->view->setNotesFontSize(this->view->getNotesFontSize()+(event->isIncrease() ? 2 : -2));
}

SlideGridConsoleViewControllerImpl::SlideGridConsoleViewControllerImpl(IEventBus *bus, SlideGridConsoleView *view, OpenPdfPresenter *presenter) {
	this->bus = bus;
	this->view = view;
	this->presenter = presenter;
	this->bus->subscribe(&SlideChangedEvent::TYPE, (SlideChangedEventHandler*)this);
	this->bus->subscribe(&SlideRenderedEvent::TYPE, (SlideRenderedEventHandler*)this);

	this->view->setTotalNumberOfSlides(this->presenter->getConfiguration()->getTotalSlides());
	for (int i = 0 ; i < this->presenter->getConfiguration()->getTotalSlides() ; i++) {
		this->view->setSlide(i,this->presenter->getSlide(i));
	}
}

QWidget * SlideGridConsoleViewControllerImpl::content() {
	return this->view->asWidget();
}

void SlideGridConsoleViewControllerImpl::doSetVisible(bool isVisible) {
	this->view->asWidget()->setVisible(isVisible);
}


void SlideGridConsoleViewControllerImpl::doRefresh() {
	for (int i = 0 ; i < this->presenter->getConfiguration()->getTotalSlides() ; i++)
		this->view->setSlide(i,this->presenter->getSlide(i));
}

void SlideGridConsoleViewControllerImpl::onSlideChanged(SlideChangedEvent *evt) {
}

void SlideGridConsoleViewControllerImpl::onSlideRendered(SlideRenderedEvent *evt) {
	this->view->setSlide(evt->getSlideNumber(),evt->getSlide());
}

void SlideGridConsoleViewControllerImpl::onSelectSlide(int slideNumber) {
	this->bus->fire(new AbsoluteSlideEvent(slideNumber));
}

void SlideGridConsoleViewControllerImpl::setGeometry(int width, int height) {
	this->view->setGeometry(width, height);
	this->refresh();
}

MainSlideViewControllerImpl::MainSlideViewControllerImpl(IEventBus * bus, MainSlideView * view, OpenPdfPresenter * presenter) {
  this->presenter = presenter;
	this->bus = bus;
	this->bus->subscribe(&SlideChangedEvent::TYPE, (SlideChangedEventHandler*)this);
	this->bus->subscribe(&BlankScreenEvent::TYPE, (ShowBlankScreenEventHandler*)this);
	this->view = view;
	this->currentSlide = 0;
	this->blackBlank = this->whiteBlank = false;
}

void MainSlideViewControllerImpl::setGeometry(int width, int height) {
	this->currentGeometry = QRect(0, 0, width, height);
}

void MainSlideViewControllerImpl::onSlideChanged(SlideChangedEvent * evt) {
	Slide slide = this->presenter->getSlide(evt->getCurrentSlideNumber());
	QPixmap pixmap = slide.asPixmap();
	if (this->currentGeometry.width() != slide.getGeometry().width() ||
					this->currentGeometry.height() != slide.getGeometry().height()) {
		QRect usableArea = slide.computeUsableArea(this->currentGeometry);
		pixmap = pixmap.scaled(usableArea.width(), usableArea.height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
	}

	qDebug() << "Showing slide number " << evt->getCurrentSlideNumber() << " at " << pixmap.width() << "x" << pixmap.height();
	this->view->setCurrentSlide(pixmap);
	this->currentSlide = evt->getCurrentSlideNumber();
	this->blackBlank = this->whiteBlank = false;
}

void MainSlideViewControllerImpl::onBlackScreen(BlankScreenEvent *evt) {
	if (this->blackBlank) {
		this->blackBlank = false;
		this->view->setCurrentSlide(this->presenter->getSlide(this->currentSlide).asPixmap());
		return;
	}
	this->blackBlank = true;
	this->whiteBlank = false;
	this->view->setBlackBlankScreen();
}

void MainSlideViewControllerImpl::onWhiteScreen(BlankScreenEvent *evt) {
	if (this->whiteBlank) {
		this->whiteBlank = false;
		this->view->setCurrentSlide(this->presenter->getSlide(this->currentSlide).asPixmap());
		return;
	}
	this->whiteBlank = true;
	this->blackBlank = false;
	this->view->setWhiteBlankScreen();
}

MainWindowViewControllerImpl::MainWindowViewControllerImpl(IEventBus * bus, MainWindowView * view) {
	this->bus = bus;
	this->view = view;
	this->exit = false;
}

bool MainWindowViewControllerImpl::signalExit(bool isExit) {
	bool ret = true;

	if (isExit && this->exit)
		this->bus->fire(new StopPresentationEvent());

	if (isExit)
		this->bus->fire(new ConfirmExitEvent(true));
	else {
		this->bus->fire(new ConfirmExitEvent(false));
		if (this->exit) // Do not process events that hide the exit confirmation
			ret = false;
	}

	this->exit = isExit;
	return ret;
}

void MainWindowViewControllerImpl::onKeyExit() {
	this->signalExit(true);
}

void MainWindowViewControllerImpl::onKeyPrev() {
	if(this->signalExit(false))
		this->bus->fire(new RelativeSlideEvent(-1));
}

void MainWindowViewControllerImpl::onKeyNext() {
	if(this->signalExit(false))
		this->bus->fire(new RelativeSlideEvent(1));
}

void MainWindowViewControllerImpl::onKeyReset() {
	if(this->signalExit(false))
		this->bus->fire(new ResetPresentationEvent());
}

void MainWindowViewControllerImpl::onKeyToggleSlideGrid() {
	if(this->signalExit(false))
		this->bus->fire(new ToggleSlideGridEvent());
}

void MainWindowViewControllerImpl::onKeyToggleNotes() {
	if(this->signalExit(false))
		this->bus->fire(new ToggleNotesEvent());
}

void MainWindowViewControllerImpl::onKeySwapScreens() {
	if(this->signalExit(false))
		this->bus->fire(new SwapScreensEvent());
}

void MainWindowViewControllerImpl::onKeyWhiteScreen() {
	if(this->signalExit(false))
		this->bus->fire(new WhiteBlankScreenEvent());
}

void MainWindowViewControllerImpl::onKeyBlackScreen() {
	if(this->signalExit(false))
		this->bus->fire(new BlackBlankScreenEvent());
}

void MainWindowViewControllerImpl::onKeyIncFontSize() {
	if(this->signalExit(false))
		this->bus->fire(new ChangeNoteFontSizeEvent(true));
}

void MainWindowViewControllerImpl::onKeyDecFontSize() {
	if(this->signalExit(false))
		this->bus->fire(new ChangeNoteFontSizeEvent(false));
}

StartScreenViewControllerImpl::StartScreenViewControllerImpl(StartScreenView * view, IEventBus * bus) {
	this->view = view;
	this->bus = bus;
	this->bus->subscribe(&SlideRenderedEvent::TYPE, (SlideRenderedEventHandler*)this);

	this->configuration = NULL;
	this->view->setController(this);
	this->currentSlide = 0;
}

void StartScreenViewControllerImpl::setConfiguration(PresenterConfiguration * configuration) {
	this->configuration = configuration;

	int hours, minutes, seconds;

	durationToTime(this->configuration->getTotalTime(),&hours,&minutes,&seconds);

	this->view->setHours(hours);
	this->view->setMinutes(minutes);
	this->view->setSeconds(seconds);

	this->view->setPdfFileName(this->configuration->getPdfFileName());
	this->view->setNotesFileName(this->configuration->getNotesFileName());

	this->view->setRehearse(this->configuration->isRehearseMode());
	this->view->setWindowed(this->configuration->isWindowedMode());

	if (this->configuration->getDocument() != NULL)
		this->setPdfDetails();
}

void StartScreenViewControllerImpl::setPdfDetails() {
	QString title = this->configuration->getDocument()->info("Title");
	if (title.isNull() || title.isEmpty())
		title = this->configuration->getPdfFileName();

	this->view->setPdfTitle(title);
	this->view->setPdfTotalPages(this->configuration->getDocument()->numPages());
	this->currentSlide = 0;
	this->view->setCurrentSlideNumber(this->currentSlide);
}

void StartScreenViewControllerImpl::browsePresentation() {
	QString pdfFileName = QFileDialog::getOpenFileName(this->view->asWidget(),QString("Open Presentation"),QString(this->configuration->getPdfFileName()),QString("Presentation Files (*.pdf)"));

	if (pdfFileName.isNull())
		return;

	this->configuration->setPdfFileName(pdfFileName);
	this->view->setPdfFileName(pdfFileName);
	this->setPdfDetails();
}

void StartScreenViewControllerImpl::browseNotes() {
	if (this->configuration->getPdfFileName().isEmpty())
		return;

	QString notesFileName = QFileDialog::getOpenFileName(this->view->asWidget(),QString("Open Notes"),QString(this->configuration->getPdfFileName()),QString("Presentation Files (*.xml *.txt)"));

	if (notesFileName.isNull())
		return;

	try {
		this->configuration->setNotesFileName(notesFileName);
	} catch(ParserException e) {
		foreach (ParserError error, e.getErrors()) {
			QMessageBox msgBox;
			msgBox.setText(QString("Error parsing notes file at line %1").arg(error.getLocation().line()));
			msgBox.setInformativeText(error.getDescription());
			msgBox.exec();
		}

		return;
	}

	this->view->setNotesFileName(notesFileName);
}

void StartScreenViewControllerImpl::discardNotes() {
	this->configuration->setNotesFileName("");
	this->view->setNotesFileName("");
}

void StartScreenViewControllerImpl::ok() {
	QString fileName = this->configuration->getPdfFileName();

	if (fileName.isNull() || fileName.isEmpty())
		return;

	this->configuration->setTotalTime(timeToDuration(
		this->view->getHours(),
		this->view->getMinutes(),
		this->view->getSeconds()));

	this->configuration->setRehearseMode(this->view->isRehearse());
	this->configuration->setWindowedMode(this->view->isWindowed());

	this->bus->fire(new StartPresentationEvent());
}

void StartScreenViewControllerImpl::quit() {
	this->bus->fire(new StopPresentationEvent());
}

void StartScreenViewControllerImpl::setSlidePreview(int slide) {
	this->currentSlide = slide;
	this->view->setSlidePreview(this->configuration->getRenderer()->getSlide(slide));
	this->view->setCurrentSlideNumber(this->currentSlide);
}

void StartScreenViewControllerImpl::onSlideRendered(SlideRenderedEvent *evt) {
	if (evt->getSlideNumber() == this->currentSlide)
		this->view->setSlidePreview(evt->getSlide());
}

void StartScreenViewControllerImpl::about() {
	QMessageBox::about(this->view->asWidget(),"About",QString("Open-PDF-Presenter version %1").arg(OPP_VERSION));
}
