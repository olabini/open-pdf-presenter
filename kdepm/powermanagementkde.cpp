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

#include "powermanagementkde.h"

#include <QtDebug>
#include <Solid/PowerManagement>

PowerManagementKDE::PowerManagementKDE() : screenSupressCookie(-1), sleepSupressCookie(-1) {
	onStartPresentation();
	bool willWork = isActive();
	onStopPresentation();

	QTextStream(stdout) << "KDE power management plugin loaded "
		<< (willWork ? "successfully" : "but will not work, possibly not running under KDE") << endl;
}

void PowerManagementKDE::onStartPresentation() {
	QString reason = "Ongoing presentation in open-pdf-presenter";
	screenSupressCookie = Solid::PowerManagement::beginSuppressingScreenPowerManagement(reason);
	sleepSupressCookie = Solid::PowerManagement::beginSuppressingSleep(reason);
}

void PowerManagementKDE::onStopPresentation() {
	Solid::PowerManagement::stopSuppressingScreenPowerManagement(screenSupressCookie);
	Solid::PowerManagement::stopSuppressingSleep(sleepSupressCookie);

	screenSupressCookie = sleepSupressCookie = -1;
}

bool PowerManagementKDE::isActive() {
	return (screenSupressCookie != -1) && (sleepSupressCookie != -1);
}

Q_EXPORT_PLUGIN2(powermanagementkde, PowerManagementKDE);
