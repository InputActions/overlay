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

#include "OverlayWidget.h"
#include "overlays/Overlay.h"
#include <QFuture>
#include <QObject>
#include <QScreen>

namespace InputActions::Overlay
{

class OverlayManager : public QObject
{
    Q_OBJECT

public:
    OverlayManager();
    ~OverlayManager() override;

    /**
     * Adds the specified overlay to all windows and shows them.
     * @returns A future that is finished when all overlay windows are shown.
     */
    QFuture<void> addOverlay(const std::function<std::unique_ptr<Overlay>(QWidget *widget)> &factory);

    /**
     * Removes the specified overlay and hides all windows if there are no overlays remaining.
     */
    void removeOverlay(const Overlay *overlay);
    /**
     * Removes the specified overlay from all windows and hides them if there are no overlays remaining.
     */
    template<typename T>
        requires std::is_base_of_v<Overlay, T>
    void removeOverlay()
    {
        for (const auto &[_, widget] : m_widgets) {
            widget->removeOverlay<T>();
        }
        hideWidgetsIfNoOverlaysPresent();
    }

    template<typename T>
        requires std::is_base_of_v<Overlay, T>
    bool hasOverlay() const
    {
        return std::ranges::any_of(m_widgets, [](const auto &pair) {
            return pair.second->template hasOverlay<T>();
        });
    }

private slots:
    void onScreenAdded(QScreen *screen);
    void onScreenRemoved(QScreen *screen);

private:
    void hideWidgetsIfNoOverlaysPresent();

    std::map<QScreen *, std::unique_ptr<OverlayWidget>> m_widgets;
};

}