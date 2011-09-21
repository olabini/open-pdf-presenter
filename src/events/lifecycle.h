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
#ifndef _LIFECICLE_H_
#define _LIFECICLE_H_

#include "timer.h"
#include "slide.h"

class SlideChangedEvent : public SlideEvent {
	public:
        SlideChangedEvent(int currentSlideNumber);
		virtual Type * getAssociatedType();
		virtual void dispatch(IEventHandler * handler);
		static Type TYPE;
        int getCurrentSlideNumber();
    private:
        int currentSlideNumber;
};

class SlideChangedEventHandler : public IEventHandler {
	public:
		virtual void onSlideChanged(SlideChangedEvent * evt) = 0;
	protected:
		~SlideChangedEventHandler() {}
};


class TimeChangedEvent : public Event {
	public:
		TimeChangedEvent(int elapsedTime, int remainingTime);
		virtual Type * getAssociatedType();
		virtual void dispatch(IEventHandler * handler);
    int getElapsedTime();
    int getRemainingTime();
    static Type TYPE;
	private:
		int elapsedTime;
		int remainingTime;
};

class ITimeChangedEventHandler : public IEventHandler {
	public:
		virtual void onTimeChanged(TimeChangedEvent * evt) = 0;
	protected:
		~ITimeChangedEventHandler() { }
};

class StartPresentationEvent : public Event {
	public:
		virtual Type * getAssociatedType();
		virtual void dispatch(IEventHandler * handler);
		static Type TYPE;
};

class StopPresentationEvent : public Event {
	public:
		virtual Type * getAssociatedType();
		virtual void dispatch(IEventHandler * handler);
		static Type TYPE;
};

class StartStopPresentationEventHandler : public IEventHandler {
    public:
        virtual void onStartPresentation(StartPresentationEvent * event) = 0;
        virtual void onStopPresentation(StopPresentationEvent * event) = 0;
    protected:
        ~StartStopPresentationEventHandler() { }
};

#endif
