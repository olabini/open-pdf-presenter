#include "transitions.h"

#include <QPainter>

// Number of frames per transition
#define N_FRAMES 10

QString NoTransition::getName() {
	return "no-transition";
}

QString NoTransition::getDesc() {
	return "Does not perform any slide transition effect";
}

void NoTransition::setSlide(QPixmap slide) {
	emit frameChanged(slide);
}

InterpolationTransition::InterpolationTransition(int transitionDuration) : animationTimeLine(transitionDuration) {
	this->animationTimeLine.setFrameRange(0,N_FRAMES);
	this->animationTimeLine.setCurveShape(QTimeLine::LinearCurve);

	this->from = NULL;
	this->to = NULL;

	connect(&(this->animationTimeLine), SIGNAL(frameChanged(int)), this, SLOT(timeLineFrameChanged(int)));
}

void InterpolationTransition::setSlide(QPixmap slide) {
	this->from = this->to;
	this->to = slide;

	this->animationTimeLine.start();
}

void InterpolationTransition::timeLineFrameChanged(int frame) {
	QPixmap result = this->animateFrame(frame, this->from, this->to);
	emit frameChanged(result);
}

CrossFadingTransition::CrossFadingTransition(int transitionDuration) : InterpolationTransition(transitionDuration) {
}

QPixmap CrossFadingTransition::animateFrame(int frame, QPixmap from, QPixmap to) {
	QPixmap result(to.size());
	result.fill(Qt::transparent);
	QPainter painter;
	painter.begin(&result);
	painter.drawPixmap(0, 0, from);
	painter.setOpacity((double(frame))/((double)N_FRAMES));
	painter.drawPixmap(0, 0, to);
	painter.end();

	return result;
}

QString CrossFadingTransition::getName() {
	return "crossfading";
}

QString CrossFadingTransition::getDesc() {
	return "Fades the new slide with increasing opacity over the old slide";
}

