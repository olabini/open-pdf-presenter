#ifndef _TIMER_H_
#define _TIMER_H_

#include "event.h"
#include <QWidget>
#include <QTimer>

class ITimerEventHandler;

class TimerEvent : public Event {
	public:
		virtual Type * getAssociatedType();
		virtual void dispatch(IEventHandler * handler);
        static Type TYPE;
};

class ITimerEventHandler : public IEventHandler {
    public:
        virtual void onTimeout(TimerEvent * evt) = 0;
    protected:
        ~ITimerEventHandler() {}
};

class Timer : public QWidget {

    Q_OBJECT

    public:
        Timer(IEventBus * bus, QWidget * parent = 0);
        ~Timer();

    private:
        QTimer * timer;
        IEventBus * bus;

    private slots:
        void lol();
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

#endif
