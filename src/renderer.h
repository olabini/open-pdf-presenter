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
#ifndef _OPEN_PDF_PRESENTER_RENDERER_H_
#define _OPEN_PDF_PRESENTER_RENDERER_H_

#include <QString>
#include <QMutex>
#include <QWaitCondition>
#include <poppler-qt4.h>

#include "events/event.h"
#include "utils.h"

class SlideRenderedEventHandler;

class SlideRenderedEvent : public Event {
		public:
				SlideRenderedEvent(int slideNumber, Slide slide);
				virtual Type * getAssociatedType();
				virtual void dispatch(IEventHandler * handler);
				static Type TYPE;
				int getSlideNumber();
				Slide getSlide();
		private:
				int slideNumber;
				Slide slide;
};

class SlideRenderedEventHandler : public IEventHandler {
		public:
				virtual void onSlideRendered(SlideRenderedEvent * evt) = 0;
		protected:
				~SlideRenderedEventHandler() {}
};

class RendererThread;

class Renderer {
		public:
				Renderer(IEventBus * bus, Poppler::Document * document, ScaleFactor * currentFactor);
				~Renderer();
				void setScaleFactor(ScaleFactor * factor);
				Slide getSlide(int slideNumber);
				void start();
				void run();
		private:
				Slide loadingSlide;
				IEventBus * bus;
				Poppler::Document * document;
				ScaleFactor * currentFactor;
				QList<Slide> * slides;
				RendererThread * thread;

				QMutex * mutex;
				QWaitCondition * factorChanged;
				bool stopThread;

				Slide renderSlide(int slideNumber, ScaleFactor * factor);
};

class RendererThread : public QThread {
		public:
				RendererThread(Renderer * renderer);
				void run();
		private:
				Renderer * renderer;
};

#endif // _RENDERER_H_
