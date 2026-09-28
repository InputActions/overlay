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

#include "OverlayWidget.h"
#include "DesktopEnvironment.h"
#include "overlays/Overlay.h"

namespace InputActions::Overlay
{

OverlayWidget::OverlayWidget(QScreen *screen)
    : m_screen(screen)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_NoSystemBackground);
    setWindowFlag(Qt::FramelessWindowHint);
    setMouseTracking(false);

    createWinId();
    m_window = LayerShellQt::Window::get(windowHandle());
    if (desktopEnvironment() == DesktopEnvironment::Plasma) {
        // Prevent the scale/glide animation. There's still a fade-in animation, but the workaround for that requires resizing windows instead of hiding them,
        // which causes a bunch of different issues. The "tooltip" scope causes the menu to disappear when moving the pointer. I wonder what side effects this
        // one has...
        m_window->setScope("on-screen-display");
    }

    m_window->setLayer(LayerShellQt::Window::Layer::LayerOverlay);
    m_window->setAnchors(static_cast<LayerShellQt::Window::Anchors>(LayerShellQt::Window::Anchor::AnchorTop));
    m_window->setDesiredSize(m_screen->size());
    m_window->setExclusiveZone(-1);
    m_window->setKeyboardInteractivity(LayerShellQt::Window::KeyboardInteractivity::KeyboardInteractivityNone);
    m_window->setScreen(screen);

    connect(m_screen, &QScreen::geometryChanged, this, &OverlayWidget::onScreenGeometryChanged);
}

OverlayWidget::~OverlayWidget() = default;

void OverlayWidget::addOverlay(std::unique_ptr<Overlay> overlay)
{
    m_overlays.push_back(std::move(overlay));
    overlaysChanged();
}

void OverlayWidget::removeOverlay(const Overlay *overlay)
{
    const auto removed = std::erase_if(m_overlays, [overlay](auto &value) {
        if (value.get() == overlay) {
            value.release()->deleteLater();
            return true;
        }
        return false;
    });
    if (removed) {
        overlaysChanged();
        repaint();
    }
}

bool OverlayWidget::hasOverlays() const
{
    return !m_overlays.empty();
}

void OverlayWidget::overlaysChanged()
{
    bool wantsKeyboardInput{};
    bool wantsMouseInput{};

    for (const auto &overlay : m_overlays) {
        wantsKeyboardInput |= overlay->wantsKeyboardInput();
        wantsMouseInput |= overlay->wantsMouseInput();
    }
    if (wantsKeyboardInput != m_wantsKeyboardInput) {
        m_window->setKeyboardInteractivity(wantsKeyboardInput ? LayerShellQt::Window::KeyboardInteractivity::KeyboardInteractivityExclusive
                                                              : LayerShellQt::Window::KeyboardInteractivity::KeyboardInteractivityNone);
    }
    if (wantsMouseInput != m_wantsMouseInput) {
        setMouseTracking(wantsMouseInput);
    }

    m_wantsKeyboardInput = wantsKeyboardInput;
    m_wantsMouseInput = wantsMouseInput;
}

void OverlayWidget::enterEvent(QEnterEvent *event)
{
    for (const auto &overlay : m_overlays) {
        overlay->enterEvent(event);
    }
}

void OverlayWidget::leaveEvent(QEvent *event)
{
    for (const auto &overlay : m_overlays) {
        overlay->leaveEvent(event);
    }
}

void OverlayWidget::mouseMoveEvent(QMouseEvent *event)
{
    for (const auto &overlay : m_overlays) {
        overlay->mouseMoveEvent(event);
    }
}

void OverlayWidget::paintEvent(QPaintEvent *event)
{
    Q_EMIT paintEventReceived();
    for (const auto &overlay : m_overlays) {
        overlay->paintEvent(event);
    }
}

void OverlayWidget::onScreenGeometryChanged(const QRect &geometry)
{
    m_window->setDesiredSize(geometry.size());
    repaint();
}

}