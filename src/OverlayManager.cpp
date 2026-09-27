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

void OverlayManager::addOverlay(const std::function<std::unique_ptr<Overlay>(QWidget *widget)> &factory)
{
    for (const auto &[_, widget] : m_widgets) {
        widget->addOverlay(factory(widget.get()));
    }
    for (const auto &[_, widget] : m_widgets) {
        if (!widget->isVisible()) {
            widget->show();
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