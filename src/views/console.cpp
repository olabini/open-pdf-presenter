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
	this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	this->layout = new QVBoxLayout(this);
  this->layout->setSpacing(0);
  this->layout->setMargin(0);
	this->setLayout(this->layout);

	this->content = NULL;
  this->barWidget = NULL;
	this->refresh();

}

void PresenterConsoleViewImpl::setController(PresenterConsoleViewController * controller) {
	this->content = content;
	this->refresh();
}

void PresenterConsoleViewImpl::refresh() {
	if (this->content != NULL)
	  this->layout->removeWidget(this->content);

	if (this->barWidget != NULL)
	  this->layout->removeWidget(this->barWidget);


	if (this->content != NULL)
		this->layout->addWidget(this->content);

	if (this->barWidget != NULL) {
	  this->layout->addWidget(this->barWidget);
		this->layout->setAlignment(this->barWidget,Qt::AlignHCenter | Qt::AlignBottom);
	}
}

void PresenterConsoleViewImpl::setControlBarView(ControlBarView * view) {
	view->asWidget()->setParent(this);
  this->barWidget = view->asWidget();

	this->refresh();
}

void PresenterConsoleViewImpl::setContent(QWidget * view) {
	this->content = view;
	this->refresh();
}

QWidget * PresenterConsoleViewImpl::asWidget() {
	return this;
}

CurrentNextSlideConsoleViewImpl::CurrentNextSlideConsoleViewImpl(QWidget * parent) : QWidget(parent) {
	ui.setupUi(this);
}

void CurrentNextSlideConsoleViewImpl::setCurrentSlide(QPixmap * slide) {
	this->ui.leftSlideFrame->setContent(slide, this->geometry().width() * 0.6);
}

void CurrentNextSlideConsoleViewImpl::setNextSlide(QPixmap * slide) {
	this->ui.rightSlideFrame->setContent(slide, this->geometry().width() * 0.25);
}

void CurrentNextSlideConsoleViewImpl::setController(CurrentNextSlideConsoleViewController * slide) {
}

QWidget * CurrentNextSlideConsoleViewImpl::asWidget() {
	return this;
}
