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
#include "timer.h"

Type TimerEvent::TYPE;

Type * TimerEvent::getAssociatedType() {
	return &(TimerEvent::TYPE);
}

void TimerEvent::dispatch(IEventHandler * handler) {
	((ITimerEventHandler*)handler)->onTimeout(this);
}

Timer::Timer(IEventBus * bus, QWidget * parent) : QWidget(parent) {
	this->timer = new QTimer(this);
	this->bus = bus;
	connect(timer, SIGNAL(timeout()), this, SLOT(onTimeout()));
}

void Timer::start() {
	this->timer->start(1000);
}

Timer::~Timer() {
	delete timer;
}

void Timer::onTimeout() {
	this->bus->fire(new TimerEvent());
}
