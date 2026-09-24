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

#include <LayerShellQt/Window>
#include <QDBusConnection>
#include <QPainterPath>
#include <QPen>
#include <QWidget>

namespace InputActions::Overlay
{

class MouseStrokeOverlay : public QWidget
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.inputactions.overlay.MouseStrokeOverlay")

public:
    MouseStrokeOverlay(QDBusConnection &bus);

public slots:
    void hide();
    void show();

protected:
    void mouseMoveEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *) override;

private:
    QPen m_pen;
    QPainterPath m_path;
    bool m_mouseMoved{};
    bool m_ignoreMouseMove{};

    LayerShellQt::Window *m_window;
};

}