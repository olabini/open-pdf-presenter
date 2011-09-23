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

#include "lifecycle.h"

Type SlideChangedEvent::TYPE;
Type TimeChangedEvent::TYPE;

SlideChangedEvent::SlideChangedEvent(int currentSlideNumber) {
	this->currentSlideNumber = currentSlideNumber;
}

int SlideChangedEvent::getCurrentSlideNumber() {
	return this->currentSlideNumber;
}

Type * SlideChangedEvent::getAssociatedType() {
	return &SlideChangedEvent::TYPE;
}

void SlideChangedEvent::dispatch(IEventHandler * handler) {
	((SlideChangedEventHandler*)handler)->onSlideChanged(this);
}

TimeChangedEvent::TimeChangedEvent(int elapsedTime, int remainingTime) {
	this->elapsedTime = elapsedTime;
	this->remainingTime = remainingTime;
}

Type * TimeChangedEvent::getAssociatedType() {
	return &(TimeChangedEvent::TYPE);
}

void TimeChangedEvent::dispatch(IEventHandler * handler) {
	((ITimeChangedEventHandler*)handler)->onTimeChanged(this);
}

int TimeChangedEvent::getElapsedTime() {
	return this->elapsedTime;
}

int TimeChangedEvent::getRemainingTime() {
	return this->remainingTime;
}

Type StartPresentationEvent::TYPE = Type();
Type StopPresentationEvent::TYPE = Type();

Type * StartPresentationEvent::getAssociatedType() {
	return &StartPresentationEvent::TYPE;
}

Type * StopPresentationEvent::getAssociatedType() {
	return &StopPresentationEvent::TYPE;
}

void StartPresentationEvent::dispatch(IEventHandler * handler) {
	((StartStopPresentationEventHandler*)handler)->onStartPresentation(this);
}

void StopPresentationEvent::dispatch(IEventHandler * handler) {
	((StartStopPresentationEventHandler*)handler)->onStopPresentation(this);
}

Type ResetPresentationEvent::TYPE = Type();

Type * ResetPresentationEvent::getAssociatedType() {
	return &ResetPresentationEvent::TYPE;
}

void ResetPresentationEvent::dispatch(IEventHandler * handler) {
	((ResetPresentationEventHandler*)handler)->onResetPresentation(this);
}
