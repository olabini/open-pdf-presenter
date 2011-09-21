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
#include "utils.h"

Slide::Slide(ScaleFactor * factor, QImage image) {
        this->factor = factor;
        this->image = image;
}

QImage Slide::asImage() {
        return this->image;
}

QPixmap Slide::asPixmap() {
                return QPixmap::fromImage(this->image);
}

ScaleFactor * Slide::getFactor() {
        return this->factor;
}

ScaleFactor ScaleFactor::DUMMY = ScaleFactor(-1,-1,-1,-1,-1);

ScaleFactor::ScaleFactor() { }

ScaleFactor::ScaleFactor(int screen, int usableWidth, int usableHeight, int xScaleFactor, int yScaleFactor) {
        this->screen = screen;
        this->usableArea = usableWidth * usableHeight;
        this->usableWidth = usableWidth;
        this->usableHeight = usableHeight;
        this->xScaleFactor = xScaleFactor;
        this->yScaleFactor = yScaleFactor;
}
