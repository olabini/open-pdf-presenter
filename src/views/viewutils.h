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
#ifndef _VIEWUTILS_H_
#define _VIEWUTILS_H_

#include <QWidget>
#include "ui_frame.h"

class Frame : public QWidget {
	Q_OBJECT

	public:
		Frame(QWidget * parent = 0);
		void setContent(QWidget * content);

	private:
		Ui::Frame ui;
};

class SlideFrame : public Frame {
	public:
		SlideFrame(QWidget * parent = 0);
		void setContent(QImage content, QRect area);

	private:
		void setContent(QWidget * content);

	private:
		QLabel * contentLabel;
};

#endif // _VIEWUTILS_H_
