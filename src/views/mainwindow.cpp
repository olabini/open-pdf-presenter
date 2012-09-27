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

MainWindowViewImpl::MainWindowViewImpl(QWidget * parent)  : QWidget(parent) {
	this->layout = new QVBoxLayout(this);
	this->layout->setSpacing(0);
	this->layout->setMargin(0);
	this->layout->setAlignment(Qt::AlignCenter);
	this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	this->content = NULL;
	this->setStyleSheet(QString("background-color: #3B4146;"));
	this->setAttribute(Qt::WA_AlwaysShowToolTips);
}

void MainWindowViewImpl::setContent(QWidget * content) {
	if (this->content != NULL)
		this->layout->removeWidget(this->content);

	this->content = content;

	this->layout->addWidget(this->content);
}

void MainWindowViewImpl::clearContent() {
	this->layout->removeWidget(this->content);
	this->content = NULL;
}

void MainWindowViewImpl::setController(MainWindowViewController * controller) {
	this->controller = controller;
}

QWidget * MainWindowViewImpl::asWidget() {
	return this;
}

void MainWindowViewImpl::mousePressEvent(QMouseEvent * event) {
	if (event->buttons() & Qt::LeftButton) {
		this->controller->onKeyNext();
	} else if (event->buttons() & Qt::RightButton) {
		this->controller->onKeyPrev();
	}
}

void MainWindowViewImpl::wheelEvent(QWheelEvent *event) {
	if (event->delta() > 0) {
		this->controller->onKeyNext();
	} else {
		this->controller->onKeyPrev();
	}
}

void MainWindowViewImpl::keyPressEvent(QKeyEvent *event) {
	switch(event->key()) {
		case Qt::Key_Escape:
		case Qt::Key_Q:
			this->controller->onKeyExit();
			break;
		case Qt::Key_Left:
		case Qt::Key_Up:
		case Qt::Key_Backspace:
		case Qt::Key_PageUp:
			this->controller->onKeyPrev();
			break;
		case Qt::Key_Right:
		case Qt::Key_Space:
		case Qt::Key_Down:
		case Qt::Key_PageDown:
			this->controller->onKeyNext();
			break;
		case Qt::Key_R:
			this->controller->onKeyReset();
			break;
		case Qt::Key_Tab:
			this->controller->onKeyToggleSlideGrid();
			break;
		case Qt::Key_S:
			this->controller->onKeySwapScreens();
			break;
		case Qt::Key_N:
			this->controller->onKeyToggleNotes();
			break;
		case Qt::Key_W:
			this->controller->onKeyWhiteScreen();
			break;
		case Qt::Key_B:
		case Qt::Key_Period:
			this->controller->onKeyBlackScreen();
			break;
		case Qt::Key_Plus:
			this->controller->onKeyIncFontSize();
			break;
		case Qt::Key_Minus:
			this->controller->onKeyDecFontSize();
			break;
	}
}
