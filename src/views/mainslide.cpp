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

MainSlideViewImpl::MainSlideViewImpl(int usableWidth, QWidget * parent) : QWidget(parent) {
	this->usableWidth = usableWidth;
	this->layout = new QVBoxLayout(this);
	this->setLayout(this->layout);
	this->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

	this->slideLabel = new QLabel(this);
	this->layout->addWidget(this->slideLabel);
	this->slideLabel->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
	this->slideLabel->setAlignment(Qt::AlignCenter);
}

void MainSlideViewImpl::setCurrentSlide(QPixmap slide, bool scale) {
	if (scale)
		this->slideLabel->setPixmap(slide.scaledToWidth(this->usableWidth, Qt::SmoothTransformation));
	else
		this->slideLabel->setPixmap(slide);
}

QWidget * MainSlideViewImpl::asWidget() {
	return this;
}
