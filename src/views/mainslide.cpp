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

#include "mainslide.h"
#include <QPainter>

MainSlideViewImpl::MainSlideViewImpl(int usableWidth, QWidget * parent) : QWidget(parent) {
	this->usableWidth = usableWidth;
	this->layout = new QVBoxLayout(this);
	this->setLayout(this->layout);
	this->layout->setSpacing(0);
	this->layout->setMargin(0);
	this->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

	this->blackBlankScreen = new QWidget(this);
	this->blackBlankScreen->setStyleSheet("background-color: black;");
	this->blackBlankScreen->setVisible(false);
	this->blackBlankScreen->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
	this->layout->addWidget(this->blackBlankScreen);
	this->whiteBlankScreen = new QWidget(this);
	this->whiteBlankScreen->setStyleSheet("background-color: white;");
	this->whiteBlankScreen->setVisible(false);
	this->whiteBlankScreen->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
	this->layout->addWidget(this->whiteBlankScreen);

	this->slideLabel = new QLabel(this);
	this->layout->addWidget(this->slideLabel);
	this->slideLabel->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
	this->slideLabel->setAlignment(Qt::AlignCenter);

	this->previous = NULL;
	this->current = NULL;
	this->i = 0;
	this->timer = QBasicTimer();
	this->timer.start(5,this);
}

void MainSlideViewImpl::setCurrentSlide(QPixmap slide) {
	this->blackBlankScreen->setVisible(false);
	this->whiteBlankScreen->setVisible(false);
	this->slideLabel->setVisible(true);

	this->current = slide;
}

void MainSlideViewImpl::timerEvent(QTimerEvent *event) {
	if (!this->current.isNull()) {
		QPixmap result(this->current.size());
		result.fill(Qt::transparent);
		QPainter painter;
		painter.begin(&result);
		painter.drawPixmap(0, 0, this->previous);
		painter.setOpacity((double(this->i))/10.0);
		painter.drawPixmap(0, 0, this->current);
		painter.end();

		this->slideLabel->setPixmap(result);

		this->i = this->i + 1;
		if (this->i > 10) {
			this->i = 0;
			this->previous = this->current;
			this->current = NULL;
		}
	}
}

void MainSlideViewImpl::setBlackBlankScreen() {
	this->whiteBlankScreen->setVisible(false);
	this->slideLabel->setVisible(false);
	this->blackBlankScreen->setVisible(true);
}

void MainSlideViewImpl::setWhiteBlankScreen() {
	this->blackBlankScreen->setVisible(false);
	this->slideLabel->setVisible(false);
	this->whiteBlankScreen->setVisible(true);
}

QWidget * MainSlideViewImpl::asWidget() {
	return this;
}
