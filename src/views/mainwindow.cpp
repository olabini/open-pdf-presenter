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
#include "mainwindow.h"

#include <QKeyEvent>
#include <iostream>

MainWindowViewImpl::MainWindowViewImpl(QWidget * parent)  : QWidget(parent) {
	this->layout = new QVBoxLayout(this);
  this->layout->setSpacing(0);
  this->layout->setMargin(0);
  this->layout->setAlignment(Qt::AlignCenter);
	this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	this->content = NULL;
  this->setStyleSheet(QString("background-color: #3B4146;"));
}

void MainWindowViewImpl::setContent(QWidget * content) {
	if (this->content != NULL)
		this->layout->removeWidget(this->content);

	this->content = content;

	this->layout->addWidget(this->content);
}

void MainWindowViewImpl::setController(MainWindowViewController * controller) {
	this->controller = controller;
}

QWidget * MainWindowViewImpl::asWidget() {
	return this;
}

void MainWindowViewImpl::keyPressEvent(QKeyEvent *event) {
    switch(event->key()) {
        case Qt::Key_Escape:
        case Qt::Key_Q:
            this->controller->onKeyExit();
            break;
        case Qt::Key_Left:
            this->controller->onKeyPrev();
            break;
        case Qt::Key_Right:
            this->controller->onKeyNext();
            break;
    }
}
