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
#include "slideevents.h"

Type AbsoluteSlideEvent::TYPE;
Type RelativeSlideEvent::TYPE;
Type SlideChangedEvent::TYPE;

RelativeSlideEvent::RelativeSlideEvent(int delta) {
	this->delta = delta;
}

Type * RelativeSlideEvent::getAssociatedType() {
	return &(RelativeSlideEvent::TYPE);
}

void RelativeSlideEvent::dispatch(IEventHandler * h) {
	SlideEventHandler* handler = (SlideEventHandler*)h;

	if (this->delta > 0)
		handler->onNextSlide(this);
	 else if (this->delta < 0)
		handler->onPrevSlide(this);
}

AbsoluteSlideEvent::AbsoluteSlideEvent(int slideNumber) {
	this->slideNumber = slideNumber;
}

Type * AbsoluteSlideEvent::getAssociatedType() {
	return &(AbsoluteSlideEvent::TYPE);
}

void AbsoluteSlideEvent::dispatch(IEventHandler * handler) {
	((SlideEventHandler*)handler)->onGotoSlide(this);
}

int AbsoluteSlideEvent::getSlideNumber() {
	return this->slideNumber;
}

SlideChangedEvent::SlideChangedEvent(int currentSlide) {
    this->currentSlide = currentSlide;
}

int SlideChangedEvent::getCurrentSlide() {
    return this->currentSlide;
}

Type * SlideChangedEvent::getAssociatedType() {
    return &SlideChangedEvent::TYPE;
}

void SlideChangedEvent::dispatch(IEventHandler * handler) {
	((SlideChangedEventHandler*)handler)->onSlideChanged(this);
}
