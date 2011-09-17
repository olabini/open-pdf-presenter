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

class Type {
};

class IEventHandler {
	protected:
		~IEventHandler() { }
};

class IEvent {
	public:
		virtual Type * getAssociatedType() = 0;
		virtual void dispatch(IEventHandler * handler) = 0;
	protected:
		~IEvent() { }
};

class IEventBus {
	public:
		virtual void subscribe(Type * type,IEventHandler * handler) = 0;
		virtual void fire(IEvent * event) = 0;

	protected:
		virtual ~IEventBus() {};
};

class SimpleEventBus : public IEventBus {
	public:
		SimpleEventBus();
		~SimpleEventBus();
		virtual void subscribe(Type * type,IEventHandler * handler);
		virtual void fire(IEvent * event);
	private:
		QMultiMap<Type*, IEventHandler*> * map;
};

#endif
