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

#include <iostream>
#include <math.h>
#include <QLabel>
#include <QPushButton>

PresenterConsoleViewImpl::PresenterConsoleViewImpl(QWidget * parent) : QWidget(parent) {
	this->controlBarWrapper = new QWidget(this);
	this->controlBarWrapper->setStyleSheet("background: none;");

	this->controlBarUi = new Ui::ControlBar();
	this->controlBarUi->setupUi(this->controlBarWrapper);
	connect(this->controlBarUi->nextButtonLabel, SIGNAL(clicked()), this, SLOT(onNextButtonClick()));
	connect(this->controlBarUi->prevButtonLabel, SIGNAL(clicked()), this, SLOT(onPrevButtonClick()));
	this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	this->layout = new QVBoxLayout(this);
	this->layout->setSpacing(0);
	this->layout->setMargin(0);
	this->setLayout(this->layout);

	this->content = NULL;
	this->refresh();

}

void PresenterConsoleViewImpl::setController(PresenterConsoleViewController * controller) {
	this->controller = controller;
}

void PresenterConsoleViewImpl::refresh() {
	this->layout->removeWidget(this->controlBarWrapper);

	if (this->content != NULL)
		this->layout->removeWidget(this->content);

	if (this->content != NULL)
		this->layout->addWidget(this->content);

	this->layout->addWidget(this->controlBarWrapper);
	this->layout->setAlignment(this->controlBarWrapper,Qt::AlignHCenter | Qt::AlignBottom);
}

void PresenterConsoleViewImpl::setContent(QWidget * view) {
	this->content = view;
	this->refresh();
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
}

void CurrentNextSlideConsoleViewImpl::setCurrentSlide(Slide slide) {
	QRect area = this->geometry();
	area.setHeight(area.height() - 100);
	area.setWidth(area.width() * 0.6);
	this->currentNextSlideUi.leftSlideFrame->setContent(slide.asPixmap(), slide.computeUsableArea(area));
}

void CurrentNextSlideConsoleViewImpl::setNextSlide(Slide slide) {
	QRect area = this->geometry();
	area.setHeight(area.height() - 100);
	area.setWidth(area.width() * 0.25);
	this->currentNextSlideUi.rightSlideFrame->setContent(slide.asPixmap(), slide.computeUsableArea(area));
}

void CurrentNextSlideConsoleViewImpl::setController(CurrentNextSlideConsoleViewController * slide) {
}

QWidget * CurrentNextSlideConsoleViewImpl::asWidget() {
	return this;
}

SlideGridConsoleViewImpl::SlideGridConsoleViewImpl(QWidget * parent) : Frame(parent) {
	QWidget * content = new QWidget(this);
	this->layout = new QGridLayout(content);
	content->setLayout(this->layout);
	content->setStyleSheet("background-color: #2F3438;");
	this->setContent(content);
	this->slides = new QList<QLabel*>();
	this->selectedSlide = -1;
	this->rows = this->cols = 0;
}

SlideGridConsoleViewImpl::~SlideGridConsoleViewImpl() {
	delete this->layout;
	this->deleteSlides();
	delete this->slides;
}

void SlideGridConsoleViewImpl::deleteSlides() {
	while (!this->slides->isEmpty()) {
		QLabel * firstSlide = this->slides->takeFirst();
		this->layout->removeWidget(firstSlide);
		delete firstSlide;
	}
}

void SlideGridConsoleViewImpl::setTotalNumberOfSlides(int total) {
	this->deleteSlides();

	this->cols = ceil(sqrt((double)total));
	this->rows = floor(sqrt((double)total)) + 1;

	QSize area = this->size();
	area.setWidth(area.width() / this->cols);
	area.setHeight((area.height() - 100) / this->rows);

	for (int i = 0 ; i < total ; i++) {
		QLabel * frame = new QLabel();
		frame->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
		this->slides->append(frame);
		frame->setPixmap(QPixmap::fromImage(QImage(QString(":/presenter/pastlastslide.svg")).scaledToWidth(area.width(),Qt::SmoothTransformation)));
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

void SlideGridConsoleViewImpl::setSlide(int slideNumber, Slide slide) {
	QSize area = this->size();
	area.setWidth(area.width() / this->cols);
	area.setHeight((area.height() - 100) / this->rows);

	QRect usableArea = slide.computeUsableArea(QRect(0,0,area.width(),area.height()));

	if (usableArea.width() > usableArea.height()) {
		this->slides->at(slideNumber)->setPixmap(slide.asPixmap().scaledToWidth(usableArea.width(),Qt::SmoothTransformation));
	} else {
		this->slides->at(slideNumber)->setPixmap(slide.asPixmap().scaledToHeight(usableArea.height(),Qt::SmoothTransformation));
	}
}

void SlideGridConsoleViewImpl::setCurrentSlide(int slideNumber) {
}

void SlideGridConsoleViewImpl::setController(SlideGridConsoleViewController *controller) {
	this->controller = controller;
}

QWidget * SlideGridConsoleViewImpl::asWidget() {
	return this;
}


