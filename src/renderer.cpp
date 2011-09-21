#include "renderer.h"

Type SlideRenderedEvent::TYPE = Type();

SlideRenderedEvent::SlideRenderedEvent(int slideNumber, Slide slide) : slide(slide) {
        this->slideNumber = slideNumber;
}

Type * SlideRenderedEvent::getAssociatedType() {
        return &SlideRenderedEvent::TYPE;
}

void SlideRenderedEvent::dispatch(IEventHandler *handler) {
        ((SlideRenderedEventHandler*)handler)->onSlideRendered(this);
}

int SlideRenderedEvent::getSlideNumber() {
        return this->slideNumber;
}

Slide SlideRenderedEvent::getSlide() {
        return this->slide;
}

Renderer::Renderer(IEventBus * bus, Poppler::Document * document, ScaleFactor * currentFactor) :
    loadingSlide(&ScaleFactor::DUMMY, QImage(QString(":/presenter/loadingslide.svg")).scaledToWidth(currentFactor->usableWidth,Qt::SmoothTransformation)) {
        this->bus = bus;
        this->document = document;
        this->currentFactor = currentFactor;

        this->slides = new QList<Slide>();
        for (int i = 0 ; i < this->document->numPages() ; i++)
                this->slides->append(this->loadingSlide);

        this->mutex = new QMutex();
        this->factorChanged = new QWaitCondition();

        this->thread = new RendererThread(this);
        this->stopThread = false;
        this->thread->start();
}

Renderer::~Renderer() {
        this->mutex->lock();
        this->stopThread = true;
        this->factorChanged->wakeAll();
        this->mutex->unlock();

        this->thread->wait();
        delete this->thread;
        delete this->mutex;
        delete this->factorChanged;

        delete this->slides;
}

void Renderer::setScaleFactor(ScaleFactor * factor) {
        this->mutex->lock();
        this->currentFactor = factor;
        this->factorChanged->wakeAll();
        this->mutex->unlock();
}

Slide Renderer::getSlide(int slideNumber) {
        this->mutex->lock();
        Slide ret = this->slides->at(slideNumber);
        this->mutex->unlock();

        return ret;
}

void Renderer::run() {
        this->mutex->lock();
        while(1) {
                bool renderedAny = false;
                for (int i = 0 ; i < this->document->numPages() ; i++) {
                        if (this->stopThread) {
                                this->mutex->unlock();
                                return;
                        }
                        Slide slide = this->slides->at(i);
                        if (slide.getFactor() == &ScaleFactor::DUMMY || slide.getFactor() != this->currentFactor) {
                                ScaleFactor * factor = this->currentFactor;
                                this->mutex->unlock();
                                renderedAny = true;
                                Slide  newSlide = this->renderSlide(i,factor);
                                this->mutex->lock();
                                this->slides->replace(i,newSlide);
                                this->bus->fire(new SlideRenderedEvent(i,newSlide));
                        }
                }
                if (!renderedAny)
                        this->factorChanged->wait(this->mutex);
        }
}

Slide Renderer::renderSlide(int slideNumber, ScaleFactor * factor) {
        Poppler::Page * pdfPage = this->document->page(slideNumber);

        QImage image = pdfPage->renderToImage(factor->xScaleFactor,factor->yScaleFactor);

        delete pdfPage;

        return Slide(factor, image);
}

RendererThread::RendererThread(Renderer *renderer) {
        this->renderer = renderer;
}

void RendererThread::run() {
        this->renderer->run();
}
