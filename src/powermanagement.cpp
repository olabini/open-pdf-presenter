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
#include <QCursor>
#include <QApplication>
#include <QDir>
#include <QPluginLoader>

PowerManagement::PowerManagement(IEventBus * bus) : pmpInstance(0) {
	bus->subscribe(&StartPresentationEvent::TYPE,(StartStopPresentationEventHandler*)this);
	bus->subscribe(&StopPresentationEvent::TYPE,(StartStopPresentationEventHandler*)this);
	bus->subscribe(&TimeChangedEvent::TYPE,(ITimeChangedEventHandler*)this);

#if defined(Q_OS_LINUX) // There is currently only one pm plugin: KDE
	loadPlugin();
#endif
}

PowerManagement::~PowerManagement() {
	onStopPresentation();
}

void PowerManagement::onStartPresentation(StartPresentationEvent * evt) {
	Q_UNUSED (evt);

	if (pmpInstance) pmpInstance->onStartPresentation();
}

void PowerManagement::onStopPresentation(StopPresentationEvent * evt) {
	Q_UNUSED (evt);

	if (pmpInstance) pmpInstance->onStopPresentation();
}

void PowerManagement::onTimeChanged(TimeChangedEvent *evt) {
	if (!pmpInstance || !pmpInstance->isActive()) {
		// No power management plugin was provided, or plugin not working for some reason
		// Falling back to moving mouse around
		QPoint cursorPosition = QCursor::pos();
		int direction = cursorPosition.y() == 0 ? 1 : -1;
		QCursor::setPos(cursorPosition.x(),cursorPosition.y() + direction);
		qApp->processEvents();
		QCursor::setPos(cursorPosition.x(),cursorPosition.y());
	}

}

// Attempt to load a power management plugin
void PowerManagement::loadPlugin() {
	QFileInfoList possiblePluginFiles = QDir(qApp->applicationDirPath()).entryInfoList(QDir::Files);

#if defined(OPP_PLUGIN_PATH)
	possiblePluginFiles = QDir(OPP_PLUGIN_PATH).entryInfoList(QDir::Files) + possiblePluginFiles;
#endif

	foreach (QFileInfo fileName, possiblePluginFiles) {
		QPluginLoader pluginLoader(fileName.absoluteFilePath());
		QObject *plugin = pluginLoader.instance();
		if (plugin) {
			PMPluginInterface *instance = qobject_cast<PMPluginInterface *>(plugin);
			if (instance) {
				pmpInstance = instance;
				return;
			}
		}
	}
}
