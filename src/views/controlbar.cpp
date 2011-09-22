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
#include "controlbar.h"

#include <iostream>

ControlBarViewImpl::ControlBarViewImpl(QWidget * parent) : QWidget(parent) {
	ui.setupUi(this);

	connect(ui.nextButtonLabel, SIGNAL(clicked()), this, SLOT(onNextButtonClick()));
	connect(ui.prevButtonLabel, SIGNAL(clicked()), this, SLOT(onPrevButtonClick()));
}

void ControlBarViewImpl::onNextButtonClick() {
	this->controller->onNextSlideButton();
}

void ControlBarViewImpl::onPrevButtonClick() {
	this->controller->onPrevSlideButton();
}

void ControlBarViewImpl::setElapsedTime(int hours, int minutes, int seconds) {
	this->ui.elapsedTimeLabel->setText(QString("%1:%2:%3").arg(hours,2,10,QChar('0')).arg(minutes,2,10,QChar('0')).arg(seconds,2,10,QChar('0')));
}

void ControlBarViewImpl::setRemainingTime(int hours, int minutes, int seconds) {
	this->ui.remainingTimeLabel->setText(QString("%1:%2:%3").arg(hours,2,10,QChar('0')).arg(minutes,2,10,QChar('0')).arg(seconds,2,10,QChar('0')));
}

void ControlBarViewImpl::setSlidePercentage (int percentage) {
	this->ui.slideSlider->setSliderPosition(percentage);
}

void ControlBarViewImpl::setTimePercentage (int percentage) {
	this->ui.timeSlider->setSliderPosition(percentage);
}

void ControlBarViewImpl::setTotalSlideCount(int count) {
	this->ui.totalSlidesLabel->setText(QString("%1").arg(count));
}

void ControlBarViewImpl::setCurrentSlideNumber(int currentSlide) {
	this->ui.currentSlideLabel->setText(QString("%1").arg(currentSlide));
}

void ControlBarViewImpl::setController(ControlBarViewController * controller) {
	this->controller = controller;
}

QWidget * ControlBarViewImpl::asWidget() {
	return this;
}
