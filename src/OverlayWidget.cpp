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
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QPainter>

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

OverlayWidget::~OverlayWidget()
{
    if (s_overlayWithPointerFocus == this) {
        s_overlayWithPointerFocus = {};
    }
}

QFuture<void> OverlayWidget::addOverlay(std::unique_ptr<Overlay> overlay)
{
    if (m_hideScheduled) {
        cancelHide();
    }

    auto *overlayPtr = overlay.get();
    m_overlays.push_back(std::move(overlay));
    overlaysChanged();

    if (isVisible()) {
        if (hasPointerFocus()) {
            overlayPtr->pointerEnterEvent(QCursor::pos());
        }
        return QtFuture::makeReadyVoidFuture();
    }

    auto promise = std::make_shared<QPromise<void>>();
    promise->start();
    connect(
        this,
        &OverlayWidget::paintEventReceived,
        this,
        [promise]() {
            promise->finish();
        },
        Qt::SingleShotConnection);
    show();
    return promise->future();
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
    bool wantsMouseInput{};

    for (const auto &overlay : m_overlays) {
        wantsMouseInput |= overlay->wantsMouseInput();
    }
    if (wantsMouseInput != m_wantsMouseInput) {
        setMouseTracking(wantsMouseInput);
    }

    m_wantsMouseInput = wantsMouseInput;
}

void OverlayWidget::hide()
{
    if (m_hideScheduled) {
        return;
    }

    // Wipe window contents to hide the close animation
    makeCurrent();
    auto *f = QOpenGLContext::currentContext()->functions();
    f->glClearColor(0, 0, 0, 1);
    f->glClear(GL_COLOR_BUFFER_BIT);
    doneCurrent();

    m_hideScheduled = true;
    // This is terrible but it seems to work
    connect(
        this,
        &OverlayWidget::frameSwapped,
        this,
        [this]() {
            connect(
                this,
                &OverlayWidget::frameSwapped,
                this,
                [this]() {
                    if (s_overlayWithPointerFocus == this) {
                        s_overlayWithPointerFocus = {};
                    }
                    m_hideScheduled = false;
                    QOpenGLWidget::hide();
                },
                Qt::SingleShotConnection);
            repaint();
        },
        Qt::SingleShotConnection);
    repaint();
}

void OverlayWidget::show()
{
    if (m_hideScheduled) {
        cancelHide();
    }
    QOpenGLWidget::show();
}

void OverlayWidget::cancelHide()
{
    m_hideScheduled = false;
    disconnect(this, &OverlayWidget::frameSwapped, this, nullptr);
}

void OverlayWidget::enterEvent(QEnterEvent *event)
{
    s_overlayWithPointerFocus = this;
    for (const auto &overlay : m_overlays) {
        overlay->pointerEnterEvent(event->position().toPoint());
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

    QPainter painter(this);
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.fillRect(rect(), Qt::transparent);
    painter.end();

    for (const auto &overlay : m_overlays) {
        overlay->paintEvent(event);
    }
}

bool OverlayWidget::hasPointerFocus() const
{
    return s_overlayWithPointerFocus == this;
}

void OverlayWidget::onScreenGeometryChanged(const QRect &geometry)
{
    m_window->setDesiredSize(geometry.size());
    repaint();
}

}