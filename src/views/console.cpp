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

void PresenterConsoleViewImpl::setRemainingTime(int hours, int minutes, int seconds) {
	this->controlBarUi->remainingTimeLabel->setText(QString("%1:%2:%3").arg(hours,2,10,QChar('0')).arg(minutes,2,10,QChar('0')).arg(seconds,2,10,QChar('0')));
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

void CurrentNextSlideConsoleViewImpl::setCurrentSlide(QPixmap slide) {
	this->currentNextSlideUi.leftSlideFrame->setContent(slide, this->geometry().width() * 0.6);
}

void CurrentNextSlideConsoleViewImpl::setNextSlide(QPixmap slide) {
	this->currentNextSlideUi.rightSlideFrame->setContent(slide, this->geometry().width() * 0.25);
}

void CurrentNextSlideConsoleViewImpl::setController(CurrentNextSlideConsoleViewController * slide) {
}

QWidget * CurrentNextSlideConsoleViewImpl::asWidget() {
	return this;
}
