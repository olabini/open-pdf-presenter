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
#include "console.h"

#include <math.h>
#include <QLabel>
#include <QPushButton>
#include <QDesktopWidget>

#define PRESENTER_USEFUL_HEIGHT_DECREMENT 200
#define PRESENTER_USEFUL_WIDTH_PERCENTAGE 0.8

PresenterConsoleViewImpl::PresenterConsoleViewImpl(QWidget * parent) : QWidget(parent) {
	this->controlBarWrapper = new QWidget(this);
	this->controlBarWrapper->setStyleSheet("background: none;");

	this->controlBarUi = new Ui::ControlBar();
	this->controlBarUi->setupUi(this->controlBarWrapper);
	connect(this->controlBarUi->nextButtonLabel, SIGNAL(clicked()), this, SLOT(onNextButtonClick()));
	connect(this->controlBarUi->prevButtonLabel, SIGNAL(clicked()), this, SLOT(onPrevButtonClick()));
	connect(this->controlBarUi->slideGridButton, SIGNAL(clicked()), this, SLOT(onSlideGridButtonClick()));
	connect(this->controlBarUi->notesButton, SIGNAL(clicked()), this, SLOT(onNotesButtonClick()));
	this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	this->innerLayout = new QVBoxLayout();
	this->outerLayout = new QVBoxLayout();
	this->outerLayout->setSpacing(0);
	this->outerLayout->setMargin(0);
	this->innerLayout->setSpacing(0);
	this->innerLayout->setMargin(0);
	this->setLayout(this->outerLayout);
	this->content = new QWidget(this);
	this->content->setLayout(this->innerLayout);
	this->outerLayout->addWidget(this->content);

	this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	this->confirmExit = new QLabel(this);
	this->confirmExit->setText("Really exit?\n\n(Again to confirm, other key to cancel)");
	this->confirmExit->setVisible(false);
	this->confirmExit->setStyleSheet("color: #FFFFFF; font-weight: bold; font-size: 32px;");
	this->confirmExit->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
	this->confirmExit->setAlignment(Qt::AlignCenter);
	this->outerLayout->addWidget(this->confirmExit);

	this->outerLayout->addWidget(this->controlBarWrapper);
	this->outerLayout->setAlignment(this->controlBarWrapper,Qt::AlignHCenter | Qt::AlignBottom);
}

void PresenterConsoleViewImpl::setConfirmExitVisible(bool isVisible) {
	// Always hide one before showing the other to avoid window resizing
	if (isVisible) {
		this->content->setVisible(false);
		this->confirmExit->setVisible(true);
	} else {
		this->confirmExit->setVisible(false);
		this->content->setVisible(true);
	}
}

void PresenterConsoleViewImpl::setController(PresenterConsoleViewController * controller) {
	this->controller = controller;
}

void PresenterConsoleViewImpl::addContent(QWidget * view) {
	this->innerLayout->addWidget(view, Qt::AlignCenter);
}

QWidget * PresenterConsoleViewImpl::asWidget() {
	return this;
}

void PresenterConsoleViewImpl::onNextButtonClick() {
	this->controller->onNextSlideButton();
}

void PresenterConsoleViewImpl::onPrevButtonClick() {
	this->controller->onPrevSlideButton();
}

void PresenterConsoleViewImpl::onSlideGridButtonClick() {
	this->controller->onSlideGridButton();
}

void PresenterConsoleViewImpl::onNotesButtonClick() {
	this->controller->onNotesButton();
}

void PresenterConsoleViewImpl::setElapsedTime(int hours, int minutes, int seconds) {
	this->controlBarUi->elapsedTimeLabel->setText(QString("%1:%2:%3").arg(hours,2,10,QChar('0')).arg(minutes,2,10,QChar('0')).arg(seconds,2,10,QChar('0')));
}

void PresenterConsoleViewImpl::setRemainingTime(int hours, int minutes, int seconds, bool overtime) {
	this->controlBarUi->remainingTimeLabel->setStyleSheet(QString("background: none; color: %1;").arg((overtime ? "red" : "white")));
	this->controlBarUi->remainingTimeLabel->setText(QString("%1%2:%3:%4").arg((overtime ? "-" : " ")).arg(hours,2,10,QChar('0')).arg(minutes,2,10,QChar('0')).arg(seconds,2,10,QChar('0')));
}

void PresenterConsoleViewImpl::setSlidePercentage (int percentage) {
	this->controlBarUi->slideSlider->setSliderPosition(percentage);
}

void PresenterConsoleViewImpl::setTimePercentage (int percentage) {
	this->controlBarUi->timeSlider->setSliderPosition(percentage);
}

void PresenterConsoleViewImpl::setTotalSlideCount(int count) {
	this->controlBarUi->totalSlidesLabel->setText(QString("%1").arg(count));
}

void PresenterConsoleViewImpl::setCurrentSlideNumber(int currentSlide) {
	this->controlBarUi->currentSlideLabel->setText(QString("%1").arg(currentSlide));
}

CurrentNextSlideConsoleViewImpl::CurrentNextSlideConsoleViewImpl(QWidget * parent) : QWidget(parent) {
	currentNextSlideUi.setupUi(this);
	this->width = 1;
	this->height = 1;
}

void CurrentNextSlideConsoleViewImpl::setGeometry(int width, int height) {
	this->width = width;
	this->height = height - PRESENTER_USEFUL_HEIGHT_DECREMENT;
}

void CurrentNextSlideConsoleViewImpl::setCurrentSlide(Slide slide) {
	QRect area = QRect(0,0,this->width * 0.6, this->height);
	this->currentNextSlideUi.leftSlideFrame->setContent(slide.asImage(), slide.computeUsableArea(area));
}

void CurrentNextSlideConsoleViewImpl::setNextSlide(Slide slide) {
	QRect area = QRect(0,0,this->width * 0.25, this->height - PRESENTER_USEFUL_HEIGHT_DECREMENT);
	this->currentNextSlideUi.rightSlideFrame->setContent(slide.asImage(), slide.computeUsableArea(area));
}

void CurrentNextSlideConsoleViewImpl::setController(CurrentNextSlideConsoleViewController * slide) {
}

QWidget * CurrentNextSlideConsoleViewImpl::asWidget() {
	return this;
}

CurrentNextSlideNotesConsoleViewImpl::CurrentNextSlideNotesConsoleViewImpl(QWidget *parent) : QWidget(parent) {
	currentNextSlideNotesUi.setupUi(this);
	this->width = 1;
	this->height = 1;
	this->currentNextSlideNotesUi.notesLabel->setWordWrap(true);
	connect(this->currentNextSlideNotesUi.zoomInButton, SIGNAL(clicked()), this, SLOT(onZoomInButtonClick()));
	connect(this->currentNextSlideNotesUi.zoomOutButton, SIGNAL(clicked()), this, SLOT(onZoomOutButtonClick()));
}

void CurrentNextSlideNotesConsoleViewImpl::setGeometry(int width, int height) {
	this->width = width;
	this->height = height - PRESENTER_USEFUL_HEIGHT_DECREMENT;
}

void CurrentNextSlideNotesConsoleViewImpl::setCurrentSlide(Slide slide) {
	QRect area = QRect(0,0,this->width * 0.4, this->height * 0.5);
	this->currentNextSlideNotesUi.currentSlideFrame->setContent(slide.asImage(), slide.computeUsableArea(area));
}

void CurrentNextSlideNotesConsoleViewImpl::setNextSlide(Slide slide) {
	QRect area = QRect(0,0,this->width * 0.25, this->height * 0.5);
	this->currentNextSlideNotesUi.nextSlideFrame->setContent(slide.asImage(), slide.computeUsableArea(area));
}

void CurrentNextSlideNotesConsoleViewImpl::setController(CurrentNextSlideNotesConsoleViewController * controller) {
	this->controller = controller;
}

void CurrentNextSlideNotesConsoleViewImpl::setNotes(QString notes) {
	this->currentNextSlideNotesUi.notesLabel->setText(notes);
}

QWidget * CurrentNextSlideNotesConsoleViewImpl::asWidget() {
	return this;
}

void CurrentNextSlideNotesConsoleViewImpl::onZoomInButtonClick() {
	this->controller->onIncNoteFontSizeButton();
}

void CurrentNextSlideNotesConsoleViewImpl::onZoomOutButtonClick() {
	this->controller->onDecNoteFontSizeButton();
}

int CurrentNextSlideNotesConsoleViewImpl::getNotesFontSize() {
	return this->currentNextSlideNotesUi.notesLabel->font().pointSize();
}

void CurrentNextSlideNotesConsoleViewImpl::setNotesFontSize(int size) {
	QFont font;
	font.setPointSize(size);
	font.setBold(true);
	font.setWeight(75);
	this->currentNextSlideNotesUi.notesLabel->setFont(font);
}

SlideGridConsoleViewImpl::SlideGridConsoleViewImpl(QWidget * parent) : Frame(parent) {
	QWidget * content = new QWidget(this);
	this->layout = new QGridLayout(content);
	this->layout->setMargin(0);
	this->layout->setSpacing(0);
	content->setLayout(this->layout);
	content->setStyleSheet("background-color: #2F3438;");
	this->setContent(content);
	this->slides = new QList<QPushButton*>();
	this->selectedSlide = -1;
	this->rows = this->cols = 0;
}

SlideGridConsoleViewImpl::~SlideGridConsoleViewImpl() {
	this->deleteSlides();
	delete this->layout;
	delete this->slides;
}

void SlideGridConsoleViewImpl::deleteSlides() {
	while (!this->slides->isEmpty()) {
		QPushButton * firstSlide = this->slides->takeFirst();
		this->layout->removeWidget(firstSlide);
		delete firstSlide;
	}
}

void SlideGridConsoleViewImpl::setTotalNumberOfSlides(int total) {
	this->deleteSlides();

	this->cols = ceil(sqrt((double)total));
	this->rows = floor(sqrt((double)total));

	if ((this->cols * this->rows) < total)
		(this->rows)++;

	QRect area =  QApplication::desktop()->screenGeometry(this);
	area.setWidth((area.width() * PRESENTER_USEFUL_WIDTH_PERCENTAGE) / this->cols);
	area.setHeight((area.height() - PRESENTER_USEFUL_HEIGHT_DECREMENT) / this->rows);

	for (int i = 0 ; i < total ; i++) {
		QPushButton * frame = new QPushButton();
		frame->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
		frame->setFlat(true);
		frame->setFocusPolicy(Qt::NoFocus);
		frame->setToolTip(QString("%1").arg(i+1));
		connect(frame, SIGNAL(clicked()), this, SLOT(onSlideClick()));
		this->slides->append(frame);
		frame->setIcon(QIcon(QPixmap::fromImage(QImage(QString(":/presenter/pastlastslide.svg")).scaledToWidth(area.width(),Qt::SmoothTransformation))));
	}

	int totalDisplayed = 0;
	for (int row = 0; row < this->rows ; row++) {
		for (int col = 0 ; col  < this->cols ; col++, totalDisplayed++) {
			if (totalDisplayed < total) {
				this->layout->addWidget(this->slides->at(totalDisplayed),row,col,Qt::AlignCenter);
			} else
				return;
		}
	}
}

void SlideGridConsoleViewImpl::setGeometry(int width, int height) {
	this->width = width;
	this->height = height - PRESENTER_USEFUL_HEIGHT_DECREMENT;
}

void SlideGridConsoleViewImpl::setSlide(int slideNumber, Slide slide) {
	QRect area =  QRect(0,0,((width * PRESENTER_USEFUL_WIDTH_PERCENTAGE) / this->cols),height / this->rows);
	area = slide.computeUsableArea(QRect(0,0,area.width()-4,area.height()-4));

	this->slides->at(slideNumber)->setIconSize(QSize(area.width(),area.height()));
	this->slides->at(slideNumber)->setIcon(slide.asPixmap());
}

void SlideGridConsoleViewImpl::setCurrentSlide(int slideNumber) {
}

void SlideGridConsoleViewImpl::setController(SlideGridConsoleViewController *controller) {
	this->controller = controller;
}

QWidget * SlideGridConsoleViewImpl::asWidget() {
	return this;
}

void SlideGridConsoleViewImpl::onSlideClick() {
	for (int i = 0 ; i < this->slides->size() ; i++) {
		if (QObject::sender() == this->slides->at(i)) {
			this->controller->onSelectSlide(i);
			return;
		}
	}
}


