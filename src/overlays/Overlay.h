/*
    InputActions overlay - Overlay for drawing on the screen and showing custom context menus
    Copyright (C) 2026 Marcin Woźniak

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#include <QMouseEvent>
#include <QObject>
#include <QPaintEvent>

namespace InputActions::Overlay
{

class OverlayWidget;

class Overlay : public QObject
{
    Q_OBJECT

public:
    /**
     * The return value of this method must not change after the implementation is added.
     */
    virtual bool wantsMouseInput() { return false; }

    /**
     * This function is also called when the overlay is added after the widget has already received pointer focus.
     */
    virtual void pointerEnterEvent(const QPoint &position) {}

    virtual void mouseMoveEvent(QMouseEvent *event) {}
    virtual void paintEvent(QPaintEvent *event) {}

protected:
    Overlay(OverlayWidget *widget);

    OverlayWidget *widget() const { return m_widget; }

private:
    OverlayWidget *m_widget;
};

}