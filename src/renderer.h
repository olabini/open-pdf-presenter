#ifndef _RENDERER_H_
#define _RENDERER_H_

#include <QString>
#include <QMutex>
#include <QWaitCondition>
#include <poppler-qt4.h>

#include "events/event.h"
#include "presenter.h"
#include "utils.h"

class SlideRenderedEventHandler;

class SlideRenderedEvent : public Event {
        public:
                SlideRenderedEvent(int slideNumber, Slide * slide);
                virtual Type * getAssociatedType();
                virtual void dispatch(IEventHandler * handler);
                static Type TYPE;
                int getSlideNumber();
                Slide * getSlide();
        private:
                int slideNumber;
                Slide * slide;
};

class SlideRenderedEventHandler : public IEventHandler {
        public:
                virtual void onSlideRendered(SlideRenderedEvent * evt) = 0;
        protected:
                ~SlideRenderedEventHandler() {}
};

class ScaleFactor;

class RenderedSlide {
        public:
                RenderedSlide(ScaleFactor * factor, Slide * slide);
                ~RenderedSlide();
                ScaleFactor * getFactor();
                Slide * getSlide();
        private:
                ScaleFactor * factor;
                Slide * slide;
};

class RendererThread;

class Renderer {
        public:
                Renderer(IEventBus * bus, Poppler::Document * document, ScaleFactor * currentFactor);
                ~Renderer();
                void setScaleFactor(ScaleFactor * factor);
                Slide * getSlide(int slideNumber);
                void run();
        private:
                RenderedSlide * loadingSlide;
                IEventBus * bus;
                Poppler::Document * document;
                ScaleFactor * currentFactor;
                QList<RenderedSlide*> * slides;
                RendererThread * thread;

                QMutex * mutex;
                QWaitCondition * factorChanged;
                bool stopThread;

                RenderedSlide * renderSlide(int slideNumber, ScaleFactor * factor);
};

class RendererThread : public QThread {
        public:
                RendererThread(Renderer * renderer);
                void run();
        private:
                Renderer * renderer;
};

#endif // _RENDERER_H_
