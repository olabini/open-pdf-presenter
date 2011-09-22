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
#ifndef _EVENT_H_
#define _EVENT_H_

#include <QMultiMap>
#include <QList>
#include <QMutex>
#include <QWaitCondition>
#include <QThread>

class Type {
};

class IEventHandler {
	protected:
		~IEventHandler() { }
};

class Event {
	public:
		virtual Type * getAssociatedType() = 0;
		virtual void dispatch(IEventHandler * handler) = 0;
};

class IEventBus {
	public:
		virtual void subscribe(Type * type,IEventHandler * handler) = 0;
		virtual void fire(Event * event) = 0;

	protected:
				~IEventBus() {};
};

/* If an event B is fired while processing another event A,
 * this bus fires B instantly and then finishes firing A */
class SimpleEventBus : public IEventBus {
	public:
		SimpleEventBus();
		~SimpleEventBus();
		virtual void subscribe(Type * type,IEventHandler * handler);
		virtual void fire(Event * event);
	protected:
		QMultiMap<Type*, IEventHandler*> * map;
};

class OrderedEvent {
	public:
		OrderedEvent(Event * event, QList<IEventHandler*> * handlers);
		void notifyNextHandler();
		bool hasHandlersNotNotified();
	private:
		Event * realEvent;
		QList<IEventHandler*> * handlers;
		int lastNotifiedHandler;
};

/* If an event B is fired while processing another event A,
 * this bus first finishes firing event A before firing event B */
class OrderedEventBus : public SimpleEventBus {
	public:
		OrderedEventBus();
		~OrderedEventBus();
		virtual void fire(Event * event);
	private:
		void resumeFiring();
		OrderedEvent * currentEvent;
		QList<Event*> * subEvents;
};

/* EventBus that supports firing concurrent events */
class ConcurrentEventBus : public SimpleEventBus, public QThread {
	public:
		ConcurrentEventBus();
		~ConcurrentEventBus();
		virtual void fire(Event * event);
		void run();
		void stop();
	private:
		QList<Event*> * pendingEvents;
		QMutex * mutex;
		QWaitCondition * pendingEventsNotEmpty;
		bool mustStop;
		QThread * eventBusThread;
};

class QEventBus : public QObject, public SimpleEventBus {
  Q_OBJECT
	
	public:
		QEventBus();
		virtual void fire(Event * event);
  private slots:
		void eventFired(Event * event);
  signals:
		void fireEvent(Event * event);
};

#endif
