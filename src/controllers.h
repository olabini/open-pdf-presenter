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
#ifndef _OPEN_PDF_PRESENTER_CONTROLLERS_H_
#define _OPEN_PDF_PRESENTER_CONTROLLERS_H_

#include "renderer.h"
#include "views/view.h"
#include "events/lifecycle.h"

class OpenPdfPresenter;

class ConsoleViewControllerImpl {
	private:
		bool visible;

	public:
		void setVisible(bool isVisible);
	
	protected:
		ConsoleViewControllerImpl();
		void refresh();
		virtual void doRefresh() = 0;
		virtual void doSetVisible(bool isVisible) = 0;
};

class CurrentNextSlideConsoleViewControllerImpl : public ConsoleViewControllerImpl, public CurrentNextSlideConsoleViewController, public SlideChangedEventHandler, public SlideRenderedEventHandler {
	public:
		CurrentNextSlideConsoleViewControllerImpl(IEventBus * bus, CurrentNextSlideConsoleView * view, OpenPdfPresenter * presenter);
		virtual void onSlideChanged(SlideChangedEvent * evt);
		virtual void onSlideRendered(SlideRenderedEvent *evt);
		virtual void setGeometry(int width, int height);
		QWidget * content();
	protected:
		virtual void doRefresh();
		virtual void doSetVisible(bool isVisible);
	private:
		OpenPdfPresenter * presenter;
		CurrentNextSlideConsoleView * view;
		IEventBus * bus;
		Slide pastLastSlide;
		int currentSlideNumber;
};

class CurrentNextSlideNotesConsoleViewControllerImpl : public ConsoleViewControllerImpl, public CurrentNextSlideNotesConsoleViewController, public SlideChangedEventHandler, public SlideRenderedEventHandler, public ChangeNoteFontSizeEventHandler {
	public:
		CurrentNextSlideNotesConsoleViewControllerImpl(IEventBus * bus, CurrentNextSlideNotesConsoleView * view, OpenPdfPresenter * presenter);
		virtual void onSlideChanged(SlideChangedEvent * evt);
		virtual void onSlideRendered(SlideRenderedEvent *evt);
		virtual void setGeometry(int width, int height);
		QWidget * content();
		virtual void onIncNoteFontSizeButton();
		virtual void onDecNoteFontSizeButton();
		virtual void onChangeNoteFontSize(ChangeNoteFontSizeEvent *event);
	protected:
		virtual void doRefresh();
		virtual void doSetVisible(bool isVisible);
	private:
		OpenPdfPresenter * presenter;
		CurrentNextSlideNotesConsoleView * view;
		IEventBus * bus;
		Slide pastLastSlide;
		int currentSlideNumber;
};

class SlideGridConsoleViewControllerImpl : public ConsoleViewControllerImpl, public SlideGridConsoleViewController, public SlideChangedEventHandler, public SlideRenderedEventHandler {
	public:
		SlideGridConsoleViewControllerImpl(IEventBus * bus, SlideGridConsoleView * view, OpenPdfPresenter * presenter);
		virtual void onSlideChanged(SlideChangedEvent * evt);
		virtual void onSlideRendered(SlideRenderedEvent *evt);
		virtual void onSelectSlide(int slideNumber);
		virtual void setGeometry(int width, int height);
		QWidget * content();
	protected:
		virtual void doRefresh();
		virtual void doSetVisible(bool isVisible);
	private:
		OpenPdfPresenter * presenter;
		SlideGridConsoleView * view;
		IEventBus * bus;
};

class MainSlideViewControllerImpl : public MainSlideViewController, public SlideChangedEventHandler, public ShowBlankScreenEventHandler {
	public:
		MainSlideViewControllerImpl(IEventBus * bus, MainSlideView * view, OpenPdfPresenter * presenter);
		virtual void onSlideChanged(SlideChangedEvent * evt);
		virtual void onBlackScreen(BlankScreenEvent *evt);
		virtual void onWhiteScreen(BlankScreenEvent *evt);
		virtual void setGeometry(int width, int height);
	private:
		OpenPdfPresenter * presenter;
		MainSlideView * view;
		IEventBus * bus;
		int currentSlide;
		bool blackBlank, whiteBlank;
		QRect currentGeometry;
};

class MainWindowViewControllerImpl : public MainWindowViewController {
	public:
		MainWindowViewControllerImpl(IEventBus * bus, MainWindowView * view);
		virtual void onKeyExit();
		virtual void onKeyPrev();
		virtual void onKeyNext();
		virtual void onKeyReset();
		virtual void onKeyToggleSlideGrid();
		virtual void onKeyToggleNotes();
		virtual void onKeySwapScreens();
		virtual void onKeyBlackScreen();
		virtual void onKeyWhiteScreen();
		virtual void onKeyIncFontSize();
		virtual void onKeyDecFontSize();
	private:
		bool signalExit(bool isExit);
	private:
		MainWindowView * view;
		IEventBus * bus;
		bool exit;
};

class PresenterConsoleState {
	public:
		virtual PresenterConsoleState * onToggleSlideView() = 0;
		virtual PresenterConsoleState * onToggleNotesView() = 0;
		virtual void apply(CurrentNextSlideConsoleViewControllerImpl * currentNext, CurrentNextSlideNotesConsoleViewControllerImpl * notes, SlideGridConsoleViewControllerImpl * grid) = 0;
};

class PresenterConsoleControllerImpl : public PresenterConsoleViewController, public SlideChangedEventHandler, public ITimeChangedEventHandler, public ToggleConsoleViewEventHandler {
	public:
		PresenterConsoleControllerImpl(IEventBus * bus, PresenterConsoleView * view, CurrentNextSlideConsoleViewControllerImpl * currentNextController, SlideGridConsoleViewControllerImpl * slideGridController, CurrentNextSlideNotesConsoleViewControllerImpl * currentNextNotesController, OpenPdfPresenter * presenter, int totalSlideCount, int durationSeconds);
		~PresenterConsoleControllerImpl();
		virtual void onNextSlideButton();
		virtual void onPrevSlideButton();
		virtual void onSlideGridButton();
		virtual void onNotesButton();
		virtual void onSlideChanged(SlideChangedEvent * evt);
		virtual void onTimeChanged(TimeChangedEvent * evt);
		virtual void onToggleSlideView(ToggleConsoleViewEvent *event);
		virtual void onToggleNotesView(ToggleConsoleViewEvent *event);
		virtual void onConfirmExit(ToggleConsoleViewEvent *event);
	private:
		void fireSlideEvent(int delta);
		PresenterConsoleView * view;
		CurrentNextSlideConsoleViewControllerImpl * currentNext;
		SlideGridConsoleViewControllerImpl * slideGrid;
		CurrentNextSlideNotesConsoleViewControllerImpl * currentNextNotes;
		IEventBus * bus;
		int totalSlideCount;
		int duration;
		OpenPdfPresenter * presenter;
		PresenterConsoleState * currentState;
};

class DefaultConsoleState : public PresenterConsoleState {
	public:
		PresenterConsoleState * onToggleSlideView();
		PresenterConsoleState * onToggleNotesView();
		void apply(CurrentNextSlideConsoleViewControllerImpl * currentNext, CurrentNextSlideNotesConsoleViewControllerImpl * notes, SlideGridConsoleViewControllerImpl * grid);
};

class SlideGridConsoleState : public PresenterConsoleState {
	public:
		SlideGridConsoleState(PresenterConsoleState * previousState);
		~SlideGridConsoleState();
		PresenterConsoleState * onToggleSlideView();
		PresenterConsoleState * onToggleNotesView();
		void apply(CurrentNextSlideConsoleViewControllerImpl * currentNext, CurrentNextSlideNotesConsoleViewControllerImpl * notes, SlideGridConsoleViewControllerImpl * grid);
	private:
		PresenterConsoleState * previousState;
};

class NotesConsoleState : public PresenterConsoleState {
	public:
		PresenterConsoleState * onToggleSlideView();
		PresenterConsoleState * onToggleNotesView();
		void apply(CurrentNextSlideConsoleViewControllerImpl * currentNext, CurrentNextSlideNotesConsoleViewControllerImpl * notes, SlideGridConsoleViewControllerImpl * grid);
};

class PresenterConfiguration;

class StartScreenViewControllerImpl : public StartScreenViewController, public SlideRenderedEventHandler {
	public:
		StartScreenViewControllerImpl(StartScreenView * view, IEventBus * bus);
		virtual void browsePresentation();
		virtual void browseNotes();
		virtual void ok();
		virtual void quit();
		virtual void discardNotes();
		virtual void about();
		virtual void setSlidePreview(int slide);
		virtual void onSlideRendered(SlideRenderedEvent *evt);
		void setConfiguration(PresenterConfiguration * configuration);

	private:
		void setPdfDetails();

	private:
		StartScreenView * view;
		IEventBus * bus;
		PresenterConfiguration * configuration;
		int currentSlide;
};

#endif
