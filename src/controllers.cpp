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
#include "events/slide.h"
#include "presenter.h"

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

void PresenterConsoleControllerImpl::onToggleNotesView(ToggleConsoleViewEvent *event) {
	this->onNotesButton();
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
	this->currentSlideNumber = 0;
}

void CurrentNextSlideConsoleViewControllerImpl::refresh() {
	if (!this->view->asWidget()->isVisible())
		return;

	this->view->setCurrentSlide(this->presenter->getSlide(this->currentSlideNumber));

	if (this->currentSlideNumber < this->presenter->getTotalSlides() - 1)
		this->view->setNextSlide(presenter->getSlide(this->currentSlideNumber + 1));
	else
		this->view->setNextSlide(pastLastSlide);
}

QWidget * CurrentNextSlideConsoleViewControllerImpl::content() {
	return this->view->asWidget();
}

void CurrentNextSlideConsoleViewControllerImpl::setVisible(bool isVisible) {
	this->view->asWidget()->setVisible(isVisible);
	if (isVisible)
		this->refresh();
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


CurrentNextSlideNotesConsoleViewControllerImpl::CurrentNextSlideNotesConsoleViewControllerImpl(IEventBus * bus, CurrentNextSlideNotesConsoleView * view, OpenPdfPresenter * presenter) :pastLastSlide(QImage(QString(":/presenter/pastlastslide.svg"))) {
	this->presenter = presenter;
	this->bus = bus;
	this->bus->subscribe(&SlideChangedEvent::TYPE, (SlideChangedEventHandler*)this);
	this->bus->subscribe(&SlideRenderedEvent::TYPE, (SlideRenderedEventHandler*)this);
	this->view = view;
	this->currentSlideNumber = 0;
}

void CurrentNextSlideNotesConsoleViewControllerImpl::refresh() {
	if (!this->view->asWidget()->isVisible())
		return;

	this->view->setCurrentSlide(this->presenter->getSlide(this->currentSlideNumber));

	if (this->currentSlideNumber < this->presenter->getTotalSlides() - 1)
		this->view->setNextSlide(presenter->getSlide(this->currentSlideNumber + 1));
	else
		this->view->setNextSlide(pastLastSlide);

	this->view->setNotes(this->presenter->getNotes(this->currentSlideNumber));
}

QWidget * CurrentNextSlideNotesConsoleViewControllerImpl::content() {
	return this->view->asWidget();
}

void CurrentNextSlideNotesConsoleViewControllerImpl::setVisible(bool isVisible) {
	this->view->asWidget()->setVisible(isVisible);
	if (isVisible)
		this->refresh();
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

QWidget * SlideGridConsoleViewControllerImpl::content() {
	return this->view->asWidget();
}

void SlideGridConsoleViewControllerImpl::setVisible(bool isVisible) {
	this->view->asWidget()->setVisible(isVisible);
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
	for (int i = 0 ; i < this->presenter->getTotalSlides() ; i++)
		this->view->setSlide(i,this->presenter->getSlide(i));
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

void MainSlideViewControllerImpl::onSlideChanged(SlideChangedEvent * evt) {
	QPixmap slide = this->presenter->getSlide(evt->getCurrentSlideNumber()).asPixmap();
	this->view->setCurrentSlide(slide);
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

void MainWindowViewControllerImpl::onKeyToggleNotes() {
	this->bus->fire(new ToggleNotesEvent());
}

void MainWindowViewControllerImpl::onKeySwapScreens() {
	this->bus->fire(new SwapScreensEvent());
}

void MainWindowViewControllerImpl::onKeyWhiteScreen() {
	this->bus->fire(new WhiteBlankScreenEvent());
}

void MainWindowViewControllerImpl::onKeyBlackScreen() {
	this->bus->fire(new BlackBlankScreenEvent());
}
