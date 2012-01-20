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
#ifndef _POWERMANAGEMENT_H_
#define _POWERMANAGEMENT_H_

#include "events/lifecycle.h"

/** Class that disables display and system power management during a presentation **/
class PowerManagement : public StartStopPresentationEventHandler {
	public:
		PowerManagement(IEventBus * bus);
		~PowerManagement();
		virtual void onStartPresentation(StartPresentationEvent * evt = 0);
		virtual void onStopPresentation(StopPresentationEvent * evt = 0);

	private:
		int screenSupressCookie;
		int sleepSupressCookie;
};

#endif
