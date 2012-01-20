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

#include "powermanagement.h"
#include <QtDebug>

#ifdef ENABLE_SOLID
#include <Solid/PowerManagement>
#endif

PowerManagement::PowerManagement(IEventBus * bus) : screenSupressCookie(0), sleepSupressCookie(0) {
	bus->subscribe(&StartPresentationEvent::TYPE,(StartStopPresentationEventHandler*)this);
	bus->subscribe(&StopPresentationEvent::TYPE,(StartStopPresentationEventHandler*)this);

#ifndef ENABLE_SOLID
	qDebug() << "No power management compiled-in. Don't forget to disable your screensaver and computer sleep!";
#endif
}

PowerManagement::~PowerManagement() {
	onStopPresentation();
}

void PowerManagement::onStartPresentation(StartPresentationEvent * evt) {
	Q_UNUSED (evt);
	QString reason = "Ongoing presentation in open-pdf-presenter";

#ifdef ENABLE_SOLID // On a KDE machine near you
	screenSupressCookie = Solid::PowerManagement::beginSuppressingScreenPowerManagement(reason);
	sleepSupressCookie = Solid::PowerManagement::beginSuppressingSleep(reason);
#endif
}

void PowerManagement::onStopPresentation(StopPresentationEvent * evt) {
	Q_UNUSED (evt);

#ifdef ENABLE_SOLID // On a KDE machine near you
	Solid::PowerManagement::stopSuppressingScreenPowerManagement(screenSupressCookie);
	Solid::PowerManagement::stopSuppressingSleep(sleepSupressCookie);
#endif
}
