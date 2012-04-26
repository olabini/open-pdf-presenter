#ifndef _TRANSITIONS_H_
#define _TRANSITIONS_H_

#include <QTimeLine>
#include <QPixmap>
#include <QMap>

class SlideTransition : public QObject {

	Q_OBJECT

	public:
		virtual void setSlide(QPixmap slide) = 0;
		virtual QString getName() = 0;
		virtual QString getDesc() = 0;


	signals:
		void frameChanged(QPixmap frame);
};

/*singleton*/ class TransitionFactory {
	private:
		TransitionFactory();
		TransitionFactory(TransitionFactory const&);
		void operator=(TransitionFactory const&);

		QMap<QString, SlideTransition*> map;

	public:
		static TransitionFactory * getInstance();
		void registerTransition(SlideTransition * transition);
		SlideTransition * getTransition(QString name);
		QList<SlideTransition*> getAllTransitions();
};

class NoTransition : public SlideTransition {
	public:
		virtual QString getName();
		virtual QString getDesc();
		virtual void setSlide(QPixmap slide);
};

class InterpolationTransition : public SlideTransition {

	Q_OBJECT

	public:
		InterpolationTransition(int transitionDuration);
		virtual void setSlide(QPixmap slide);

	protected:
		virtual QPixmap animateFrame(int currentFrame, QPixmap from, QPixmap to) = 0;

	private:
		QPixmap from;
		QPixmap to;
		QTimeLine animationTimeLine;


	private slots:
		void timeLineFrameChanged(int frame);

};

class CrossFadingTransition : public InterpolationTransition {
	public:
		CrossFadingTransition(int transitionDuration);
		virtual QPixmap animateFrame(int currentFrame, QPixmap from, QPixmap to);
		virtual QString getName();
		virtual QString getDesc();
};

#endif // _TRANSITIONS_H_
