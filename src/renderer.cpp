#include "renderer.h"

Type SlideRenderedEvent::TYPE;

SlideRenderedEvent::SlideRenderedEvent(int slideNumber, Slide * slide) {
        this->slideNumber = slideNumber;
        this->slide = slide;
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

Slide * SlideRenderedEvent::getSlide() {
        return this->slide;
}

Renderer::Renderer(IEventBus * bus, Poppler::Document * document, ScaleFactor * currentFactor) {
        this->bus = bus;
        this->document = document;
        this->currentFactor = currentFactor;

        this->loadingSlide = new RenderedSlide(
                    this->currentFactor,
                    new Slide(QImage(
                                      QString(":/presenter/loadingslide.svg")).scaledToWidth(this->currentFactor->usableWidth,Qt::SmoothTransformation)));

        this->slides = new QList<RenderedSlide*>();
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

        delete this->loadingSlide;

        for (int i = 0 ; i < this->document->numPages() ; i++)
                delete this->slides->takeFirst();

        delete this->slides;
}

void Renderer::setScaleFactor(ScaleFactor * factor) {
        this->mutex->lock();
        this->currentFactor = factor;
        this->factorChanged->wakeAll();
        this->mutex->unlock();
}

Slide * Renderer::getSlide(int slideNumber) {
        Slide * ret;
        this->mutex->lock();
        ret = this->slides->at(slideNumber)->getSlide();
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
                        RenderedSlide * renderedSlide = this->slides->at(i);
                        if (renderedSlide == this->loadingSlide || renderedSlide->getFactor() != this->currentFactor) {
                                ScaleFactor * factor = this->currentFactor;
                                this->mutex->unlock();
                                renderedAny = true;
                                RenderedSlide * newRenderedSlide = this->renderSlide(i,factor);
                                this->mutex->lock();
                                this->slides->replace(i,newRenderedSlide);
                                if (renderedSlide != this->loadingSlide)
                                        delete renderedSlide;
                                this->bus->fire(new SlideRenderedEvent(i,newRenderedSlide->getSlide()));
                        }
                }
                if (!renderedAny)
                        this->factorChanged->wait(this->mutex);
        }
}

RenderedSlide * Renderer::renderSlide(int slideNumber, ScaleFactor * factor) {
        Poppler::Page * pdfPage = this->document->page(slideNumber);

        QImage image = pdfPage->renderToImage(factor->xScaleFactor,factor->yScaleFactor);

        RenderedSlide * ret = new RenderedSlide(factor, new Slide(image));

        delete pdfPage;

        return ret;
}

RendererThread::RendererThread(Renderer *renderer) {
        this->renderer = renderer;
}

void RendererThread::run() {
        this->renderer->run();
}

RenderedSlide::RenderedSlide(ScaleFactor *factor, Slide *slide) {
        this->factor = factor;
        this->slide = slide;
}

RenderedSlide::~RenderedSlide() {
        delete this->slide;
}

ScaleFactor * RenderedSlide::getFactor() {
        return this->factor;
}

Slide * RenderedSlide::getSlide() {
        return this->slide;
}
