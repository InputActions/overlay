/*
    InputActions overlay - Overlay for drawing on the screen
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
#include <QPaintEvent>

namespace InputActions::Overlay
{

class Overlay
{
public:
    virtual ~Overlay() = default;

    /**
     * The return value of this method must not change after the implementation is added.
     */
    virtual bool wantsKeyboardInput() { return false; }
    /**
     * The return value of this method must not change after the implementation is added.
     */
    virtual bool wantsMouseInput() { return false; }

    virtual void enterEvent(QEnterEvent *event) {}
    virtual void mouseMoveEvent(QMouseEvent *event) {}
    virtual void paintEvent(QPaintEvent *event) {}

protected:
    Overlay(QWidget *widget);

    QWidget *widget() const { return m_widget; }

private:
    QWidget *m_widget;
};

}