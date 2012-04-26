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
#ifndef _MAINSLIDE_H_
#define _MAINSLIDE_H_

#include "view.h"
#include "transitions.h"

#include <QWidget>
#include <QPixmap>
#include <QVBoxLayout>
#include <QLabel>
#include <QTimeLine>

class MainSlideViewImpl : public QWidget, public MainSlideView {

	Q_OBJECT

	public:
		MainSlideViewImpl(int usableWidth, SlideTransition * transition, QWidget * parent = NULL);
		~MainSlideViewImpl();
		virtual void setCurrentSlide(QPixmap slide);
		virtual void setBlackBlankScreen();
		virtual void setWhiteBlankScreen();
		virtual void setController(MainSlideViewController * controller) { }
		virtual QWidget * asWidget();

	private:
		int usableWidth;
		QVBoxLayout * layout;
		QLabel * slideLabel;
		QWidget * blackBlankScreen;
		QWidget * whiteBlankScreen;

		SlideTransition * transition;

	private slots:
		void frameChanged (QPixmap frame);
};

#endif
