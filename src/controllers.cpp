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
#include "controllers.h"

ControlBarController::ControlBarController(IEventBus * bus, ControlBarView * view, int totalSlideCount) {
    this->bus = bus;
    this->bus->subscribe(&SlideChangedEvent::TYPE, this);
    this->view = view;
    this->view->setController(this);
    this->view->setTotalSlideCount(totalSlideCount);
}

void ControlBarController::onNextSlideButton() {
    this->fireSlideEvent(1);
}

void ControlBarController::onPrevSlideButton() {
    this->fireSlideEvent(-1);
}

void ControlBarController::fireSlideEvent(int delta) {
    RelativeSlideEvent * event = new RelativeSlideEvent(delta);
    this->bus->fire(event);
    delete event;
}

void ControlBarController::onSlideChanged(SlideChangedEvent * evt) {
    this->view->setCurrentSlideNumber(evt->getCurrentSlide());
}
