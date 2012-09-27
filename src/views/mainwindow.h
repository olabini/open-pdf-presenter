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
#ifndef _MAINWINDOW_H_
#define _MAINWINDOW_H_

#include "view.h"

#include <QVBoxLayout>

class MainWindowViewImpl : public QWidget, public MainWindowView {

	Q_OBJECT

	public:
		MainWindowViewImpl(QWidget * parent = 0);
		virtual void setContent(QWidget * content);
		virtual void clearContent();
		virtual void setController(MainWindowViewController * controller);
		virtual QWidget * asWidget();

	protected:
		void keyPressEvent(QKeyEvent * event);
		void mousePressEvent(QMouseEvent * event);
		void wheelEvent(QWheelEvent * event);

	private:
		QWidget * content;
		QVBoxLayout * layout;
		MainWindowViewController * controller;
};

#endif
