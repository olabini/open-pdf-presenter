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
#ifndef _OPEN_PDF_PRESENTER_UTILS_H_
#define _OPEN_PDF_PRESENTER_UTILS_H_

#include <QPixmap>
#include <QImage>

class ScaleFactor {
        public:
                ScaleFactor();
                ScaleFactor(int screen, int usableWidth, int usableHeight, int xScaleFactor, int yScaleFactor);
                int screen;
                int usableArea;
                int usableWidth;
                int usableHeight;
                int xScaleFactor;
                int yScaleFactor;
                static ScaleFactor DUMMY;
};


class Slide {
        public:
                Slide(ScaleFactor * factor, QImage image);
                QPixmap asPixmap();
                QImage asImage();
                ScaleFactor * getFactor();
        private:
                QImage image;
                ScaleFactor * factor;
};

#endif // _UTILS_H_
