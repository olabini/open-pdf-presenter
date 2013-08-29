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
#include "startscreen.h"

#include <QMenu>

StartScreenViewImpl::StartScreenViewImpl(QWidget *parent) : QWidget(parent), previewArea(0,0,200,200) {
	ui.setupUi(this);

	this->setSlidePreview(Slide(QImage(QString(":/presenter/pastlastslide.svg")), QRect()));

	connect(this->ui.startButton, SIGNAL(clicked()), this, SLOT(onStartButtonClick()));
	connect(this->ui.quitButton, SIGNAL(clicked()), this, SLOT(onCancelButtonClick()));
	connect(this->ui.openPresentationButtonBox, SIGNAL(accepted()), this, SLOT(onPdfFileBrowseButtonClick()));
	connect(this->ui.openNotesButtonBox, SIGNAL(accepted()), this, SLOT(onNotesFileBrowseButtonClick()));
	connect(this->ui.openNotesButtonBox, SIGNAL(destroyed()), this, SLOT(onNotesFileDiscard()));
	connect(this->ui.openNotesButtonBox, SIGNAL(rejected()), this, SLOT(onNotesFileDiscard()));
	connect(this->ui.slideSlider, SIGNAL(valueChanged(int)), this, SLOT(onSliderMove(int)));

	QMenu * optionsMenu = new QMenu();
	optionsMenu->addAction(this->ui.actionRehearse_Mode);
	optionsMenu->addAction(this->ui.actionWindowed_Mode);
	optionsMenu->addAction(this->ui.actionAbout);

	this->ui.optionsButton->setMenu(optionsMenu);

	connect(this->ui.actionAbout, SIGNAL(triggered()), this, SLOT(onAboutClick()));

	this->ui.slideSlider->setTracking(true);
	this->ui.slideSlider->setEnabled(false);
}

void StartScreenViewImpl::setController(StartScreenViewController * controller) {
	this->controller = controller;
}

QWidget * StartScreenViewImpl::asWidget() {
	return this;
}

void StartScreenViewImpl::setPdfFileName(QString fileName) {
	this->ui.presentationFileNameLabel->setText(fileName);
}

void StartScreenViewImpl::setNotesFileName(QString fileName) {
	this->ui.notesFileNameLabel->setText(fileName);
}

void StartScreenViewImpl::setHours(int hours) {
	this->ui.hoursBox->setValue(hours);
}

void StartScreenViewImpl::setMinutes(int minutes) {
	this->ui.minutesBox->setValue(minutes);
}

void StartScreenViewImpl::setSeconds(int seconds) {
	this->ui.secondsBox->setValue(seconds);
}

int StartScreenViewImpl::getHours() {
	return this->ui.hoursBox->value();
}

int StartScreenViewImpl::getMinutes() {
	return this->ui.minutesBox->value();
}

int StartScreenViewImpl::getSeconds() {
	return this->ui.secondsBox->value();
}

void StartScreenViewImpl::onStartButtonClick() {
	this->controller->ok();
}

void StartScreenViewImpl::onCancelButtonClick() {
	this->controller->quit();
}

void StartScreenViewImpl::onPdfFileBrowseButtonClick() {
	this->controller->browsePresentation();
}

void StartScreenViewImpl::onNotesFileBrowseButtonClick() {
	this->controller->browseNotes();
}

void StartScreenViewImpl::onNotesFileDiscard() {
	this->controller->discardNotes();
}

void StartScreenViewImpl::setPdfTitle(QString title) {
	this->ui.titleLabel->setText(title);
}

void StartScreenViewImpl::setPdfTotalPages(int totalPages) {
	this->ui.slideSlider->setEnabled(true);
	this->ui.slideSlider->setSliderPosition(0);
	this->ui.slideSlider->setMaximum(totalPages-1);
	this->ui.totalSlideNumberLabel->setText(QString("%1").arg(totalPages));
}

void StartScreenViewImpl::setSlidePreview(Slide slide) {
	QRect area = slide.computeUsableArea(this->previewArea);

	if (area.width() > area.height())
		this->ui.previewLabel->setPixmap(QPixmap::fromImage(slide.asImage().scaledToWidth(area.width(), Qt::SmoothTransformation)));
	else
		this->ui.previewLabel->setPixmap(QPixmap::fromImage(slide.asImage().scaledToHeight(area.height(), Qt::SmoothTransformation)));
}

void StartScreenViewImpl::setCurrentSlideNumber(int currentSlide) {
	this->ui.currentSlideLabel->setText(QString("%1").arg(currentSlide+1));
}

void StartScreenViewImpl::onSliderMove(int position) {
	this->controller->setSlidePreview(position);
}

void StartScreenViewImpl::onAboutClick() {
	this->controller->about();
}

void StartScreenViewImpl::setRehearse(bool isRehearse) {
	this->ui.actionRehearse_Mode->setChecked(isRehearse);
}

bool StartScreenViewImpl::isRehearse() {
	return this->ui.actionRehearse_Mode->isChecked();
}

void StartScreenViewImpl::setWindowed(bool isWindowed) {
	this->ui.actionWindowed_Mode->setChecked(isWindowed);
}

bool StartScreenViewImpl::isWindowed() {
	return this->ui.actionWindowed_Mode->isChecked();
}
