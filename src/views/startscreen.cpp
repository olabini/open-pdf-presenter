#include "startscreen.h"

StartScreenViewImpl::StartScreenViewImpl(QWidget *parent) : QWidget(parent) {
	ui.setupUi(this);

	connect(this->ui.startButton, SIGNAL(clicked()), this, SLOT(onStartButtonClick()));
	connect(this->ui.quitButton, SIGNAL(clicked()), this, SLOT(onCancelButtonClick()));
	connect(this->ui.openPresentationButtonBox, SIGNAL(accepted()), this, SLOT(onPdfFileBrowseButtonClick()));
	connect(this->ui.openNotesButtonBox, SIGNAL(accepted()), this, SLOT(onNotesFileBrowseButtonClick()));
	connect(this->ui.openNotesButtonBox, SIGNAL(destroyed()), this, SLOT(onNotesFileDiscard()));
	connect(this->ui.openNotesButtonBox, SIGNAL(rejected()), this, SLOT(onNotesFileDiscard()));
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
}

