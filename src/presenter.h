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
#ifndef _PRESENTER_H_
#define _PRESENTER_H_

#include "events/event.h"
#include "events/slideevents.h"
#include "events/timer.h"

class OpenPdfPresenter : public SlideEventHandler, public ITimerEventHandler {
    public:
        OpenPdfPresenter(int totalTime, int totalSlides, IEventBus * bus);
        int getCurrentSlide();
        int getTotalSlides();
		virtual void onNextSlide(RelativeSlideEvent * evt);
		virtual void onPrevSlide(RelativeSlideEvent * evt);
		virtual void onGotoSlide(AbsoluteSlideEvent * evt);
        virtual void onTimeout(TimerEvent * evt);
    private:
        int currentSlide;
        int totalSlides;
        int elapsedTime;
        int totalTime;
        IEventBus * bus;
};

#endif
