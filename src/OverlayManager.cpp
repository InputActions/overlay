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

#include "OverlayManager.h"
#include <QGuiApplication>

namespace InputActions::Overlay
{

OverlayManager::OverlayManager()
{
    connect(qGuiApp, &QGuiApplication::screenAdded, this, &OverlayManager::onScreenAdded);
    connect(qGuiApp, &QGuiApplication::screenRemoved, this, &OverlayManager::onScreenRemoved);

    for (auto *screen : QGuiApplication::screens()) {
        onScreenAdded(screen);
    }
}

OverlayManager::~OverlayManager() = default;

QFuture<void> OverlayManager::addOverlay(const std::function<std::unique_ptr<Overlay>(QWidget *widget)> &factory)
{
    std::vector<OverlayWidget *> hiddenWidgets;
    for (const auto &[_, widget] : m_widgets) {
        widget->addOverlay(factory(widget.get()));
        if (!widget->isVisible()) {
            hiddenWidgets.push_back(widget.get());
        }
    }

    if (hiddenWidgets.empty()) {
        return QtFuture::makeReadyVoidFuture();
    }

    std::vector<QFuture<void>> futures;
    for (auto *widget : hiddenWidgets) {
        auto promise = std::make_shared<QPromise<void>>();
        connect(
            widget,
            &OverlayWidget::paintEventReceived,
            this,
            [promise]() {
                promise->finish();
            },
            Qt::SingleShotConnection);
        futures.push_back(promise->future());

        promise->start();
        widget->show();
    }
    return QtFuture::whenAll(futures.begin(), futures.end());
}

void OverlayManager::removeOverlay(const Overlay *overlay)
{
    for (const auto &[_, widget] : m_widgets) {
        widget->removeOverlay(overlay);
    }
    hideWidgetsIfNoOverlaysPresent();
}

void OverlayManager::hideWidgetsIfNoOverlaysPresent()
{
    // Hiding windows can mess with focus, so only do it when there are no overlays left
    const auto anyWindowHasOverlay = std::ranges::any_of(m_widgets, [](const auto &pair) {
        return pair.second->hasOverlays();
    });
    if (anyWindowHasOverlay) {
        return;
    }

    for (const auto &[_, widget] : m_widgets) {
        if (widget->isVisible()) {
            widget->hide();
        }
    }
}

void OverlayManager::onScreenAdded(QScreen *screen)
{
    m_widgets[screen] = std::make_unique<OverlayWidget>(screen);
}

void OverlayManager::onScreenRemoved(QScreen *screen)
{
    m_widgets.erase(screen);
}

}