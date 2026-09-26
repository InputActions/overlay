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

#include "MouseStrokeOverlay.h"
#include "DesktopEnvironment.h"
#include <LayerShellQt/Window>
#include <QGuiApplication>
#include <QMouseEvent>
#include <QPainter>

namespace InputActions::Overlay
{

MouseStrokeOverlay::MouseStrokeOverlay(QDBusConnection &bus)
{
    bus.registerObject("/org/inputactions/overlay/MouseStrokeOverlay", this, QDBusConnection::ExportAllSlots);

    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_NoSystemBackground);
    setWindowFlag(Qt::FramelessWindowHint);
    setMouseTracking(true);

    createWinId();
    m_window = LayerShellQt::Window::get(windowHandle());
    if (desktopEnvironment() == DesktopEnvironment::Plasma) {
        // Prevent the scale/glide animation. There's still a fade-in animation, but the workaround for that requires creating a window for every screen.
        m_window->setScope("tooltip");
    }

    m_window->setLayer(LayerShellQt::Window::Layer::LayerOverlay);
    m_window->setAnchors(static_cast<LayerShellQt::Window::Anchors>(LayerShellQt::Window::Anchor::AnchorTop));
    m_window->setExclusiveZone(-1);
    m_window->setKeyboardInteractivity(LayerShellQt::Window::KeyboardInteractivity::KeyboardInteractivityNone);
    m_window->setWantsToBeOnActiveScreen(true);

    m_pen = {QGuiApplication::palette().accent().color()};
    m_pen.setWidthF(4.0);
    m_pen.setCapStyle(Qt::RoundCap);
    m_pen.setJoinStyle(Qt::RoundJoin);
}

void MouseStrokeOverlay::hide()
{
    m_ignoreMouseMove = true;
    m_path.clear();
    repaint();
    QWidget::hide();
}

void MouseStrokeOverlay::show()
{
    QWidget::show();
    m_window->setDesiredSize(screen()->size());
    m_ignoreMouseMove = false;
    m_mouseMoved = false;
}

void MouseStrokeOverlay::mouseMoveEvent(QMouseEvent *event)
{
    if (m_ignoreMouseMove) {
        return;
    }

    if (!m_mouseMoved) {
        m_path.moveTo(event->position());
        m_mouseMoved = true;
        return;
    }

    m_path.lineTo(event->position());
    update();
}

void MouseStrokeOverlay::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(m_pen);
    painter.setBrush(Qt::NoBrush);

    painter.drawPath(m_path);
}

}