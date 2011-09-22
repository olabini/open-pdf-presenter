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
#include "event.h"

SimpleEventBus::SimpleEventBus() {
	this->map = new QMultiMap<Type*, IEventHandler*>();
}

SimpleEventBus::~SimpleEventBus() {
	delete this->map;
}

void SimpleEventBus::subscribe(Type * type, IEventHandler * handler) {
	this->map->insert(type,handler);
}

void SimpleEventBus::fire(Event * event) {
	QList<IEventHandler*> values = this->map->values(event->getAssociatedType());
	for (int i = 0 ; i < values.size() ; i++) {
	 event->dispatch(values.at(i));
	}

	delete event;
}

OrderedEvent::OrderedEvent(Event * event, QList<IEventHandler*> * handlers) {
	this->handlers = handlers;
	this->lastNotifiedHandler = 0;
	this->realEvent = event;
}

bool OrderedEvent::hasHandlersNotNotified() {
	return this->lastNotifiedHandler < this->handlers->size();
}

void OrderedEvent::notifyNextHandler() {
	if (!this->hasHandlersNotNotified())
		return;

	this->realEvent->dispatch(this->handlers->at(this->lastNotifiedHandler++));
}

OrderedEventBus::OrderedEventBus() {
	this->currentEvent = NULL;
	this->subEvents = new QList<Event*>();
}

OrderedEventBus::~OrderedEventBus() {
	delete this->subEvents;
}

void OrderedEventBus::fire(Event * evt) {
	if (this->currentEvent != NULL) {
		this->subEvents->append(evt);
		return;
	}

	this->subEvents->append(evt);

	while (!this->subEvents->isEmpty()) {
		Event * evt = this->subEvents->takeFirst();
		QList<IEventHandler*> values = this->map->values(evt->getAssociatedType());
		OrderedEvent * event = new OrderedEvent(evt, &values);
		this->currentEvent = event;
		while (this->currentEvent->hasHandlersNotNotified()) {
			this->currentEvent->notifyNextHandler();
		}

		delete evt;
	}

	this->currentEvent = NULL;
}

ConcurrentEventBus::ConcurrentEventBus() {
	this->mustStop = false;
	this->pendingEvents = new QList<Event*>();
	this->mutex = new QMutex();
	this->pendingEventsNotEmpty = new QWaitCondition();
	this->start();
}

ConcurrentEventBus::~ConcurrentEventBus() {
	this->stop();
	this->wait();
	delete this->pendingEvents;
	delete this->mutex;
	delete this->pendingEventsNotEmpty;
}

void ConcurrentEventBus::fire(Event * event) {

	this->mutex->lock();
	this->pendingEvents->append(event);
	this->pendingEventsNotEmpty->wakeAll();
	this->mutex->unlock();
}

void ConcurrentEventBus::run() {
	this->eventBusThread = QThread::currentThread();
	while(1) {
		this->mutex->lock();
		if (this->pendingEvents->isEmpty()) {
			this->pendingEventsNotEmpty->wait(this->mutex);
			if (this->mustStop) {
				this->mutex->unlock();
				return;
			}
		}
		Event * event = this->pendingEvents->takeFirst();
		this->mutex->unlock();
		SimpleEventBus::fire(event);
	}
}

void ConcurrentEventBus::stop() {
	this->mutex->lock();
	this->mustStop = true;
	this->pendingEventsNotEmpty->wakeAll();
	this->mutex->unlock();
}

QEventBus::QEventBus() {
	connect(this,SIGNAL(fireEvent(Event*)),this,SLOT(eventFired(Event*)));
}

void QEventBus::fire(Event * event) {
	emit fireEvent(event);
}

void QEventBus::eventFired(Event * event) {
	SimpleEventBus::fire(event);
}
