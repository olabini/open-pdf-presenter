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

#include <QWidget>
#include <QPixmap>
#include <QVBoxLayout>
#include <QLabel>

class MainSlideViewImpl : public QWidget, public MainSlideView {
	public:
		MainSlideViewImpl(int usableWidth, QWidget * parent = NULL);
		virtual void setCurrentSlide(QPixmap slide, bool scale = false);
		virtual void setController(MainSlideViewController * controller) { }
                virtual QWidget * asWidget();
	private:
		int usableWidth;
		QVBoxLayout * layout;
		QLabel * slideLabel;
};

#endif
