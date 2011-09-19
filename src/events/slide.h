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
#ifndef _SLIDE_H_
#define _SLIDE_H_

#include "event.h"

class SlideEvent : public Event {
	public:
		virtual Type * getAssociatedType() = 0;
		virtual void dispatch(IEventHandler * handler) = 0;
	protected:
		~SlideEvent() {}
};

class RelativeSlideEvent;
class AbsoluteSlideEvent;

class SlideEventHandler : public IEventHandler {
	public:
		virtual void onNextSlide(RelativeSlideEvent * evt) = 0;
		virtual void onPrevSlide(RelativeSlideEvent * evt) = 0;
		virtual void onGotoSlide(AbsoluteSlideEvent * evt) = 0;
	protected:
		~SlideEventHandler() {}
};

class RelativeSlideEvent : public SlideEvent {
	public:
		RelativeSlideEvent(int delta);
		virtual Type * getAssociatedType();
		virtual void dispatch(IEventHandler * handler);
		static Type TYPE;
        int getDelta();
	private:
		int delta;
};

class AbsoluteSlideEvent : public SlideEvent {
	public:
		AbsoluteSlideEvent(int slideNumber);
		virtual Type * getAssociatedType();
		virtual void dispatch(IEventHandler * handler);
		int getSlideNumber();
		static Type TYPE;
	private:
		int slideNumber;
};

#endif
