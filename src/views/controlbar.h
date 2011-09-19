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
#ifndef _CONTROLBAR_H_
#define _CONTROLBAR_H_

#include <QWidget>
#include "ui_controlbar.h"
#include "view.h"

class ControlBarViewImpl : public QWidget, public ControlBarView {

    Q_OBJECT

	public:
		ControlBarViewImpl(QWidget * parent = 0);
		virtual void setElapsedTime(int hours, int minutes, int seconds);
		virtual void setRemainingTime(int hours, int minutes, int seconds);
		virtual void setSlidePercentage (int percentage);
		virtual void setTotalSlideCount(int count);
		virtual void setTimePercentage (int currentSlide);
		virtual void setController(ControlBarViewController * controller);
		virtual void setCurrentSlideNumber(int currentSlide);
		virtual QWidget * asWidget();
    
    private:
		Ui::ControlBar ui;
        ControlBarViewController * controller;
        
    private slots:
        void onNextButtonClick();
        void onPrevButtonClick();
    
};

#endif
