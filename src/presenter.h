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
#ifndef _PRESENTER_H_
#define _PRESENTER_H_

#include "events/event.h"
#include "events/slide.h"
#include "events/timer.h"
#include "events/lifecycle.h"
#include "renderer.h"
#include "utils.h"

#include <QList>
#include <poppler-qt4.h>

class Renderer;

class OpenPdfPresenter : public SlideEventHandler, public ITimerEventHandler, public StartStopPresentationEventHandler {
        public:
                OpenPdfPresenter(int argc, char ** argv, IEventBus * bus);
                ~OpenPdfPresenter();
                int getCurrentSlide();
                int getTotalSlides();
                int getTotalTimeSeconds();
                Slide getSlide(int slideNumber);
                virtual void onNextSlide(RelativeSlideEvent * evt);
		virtual void onPrevSlide(RelativeSlideEvent * evt);
                virtual void onGotoSlide(AbsoluteSlideEvent * evt);
                virtual void onTimeout(TimerEvent * evt);
                virtual void onStartPresentation(StartPresentationEvent * evt);
                virtual void onStopPresentation(StopPresentationEvent * evt);
                ScaleFactor * getScaleFactor();
        private:
                int currentSlideNumber;
                int totalSlides;
                int elapsedTime;
                int totalTime;
				
                ScaleFactor * scaleFactor;
                QString pdfFileName;
                IEventBus * bus;
                Poppler::Document * document;

                Renderer * renderer;

                void parseArguments(int argc, char ** argv);
                QList<ScaleFactor*> * computeScaleFactors();
                void fireSlideChangedEvent();
};

#endif
