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

#include <LayerShellQt/Window>
#include <QEnterEvent>
#include <QWidget>

namespace InputActions::Overlay
{

class Overlay;

class OverlayWidget : public QWidget
{
    Q_OBJECT

public:
    OverlayWidget(QScreen *screen);
    ~OverlayWidget() override;

    /**
     * Do not call directly, use OverlayManager instead.
     */
    void addOverlay(std::unique_ptr<Overlay> overlay);
    /**
     * Do not call directly, use OverlayManager instead.
     */
    void removeOverlay(const Overlay *overlay);
    /**
     * Do not call directly, use OverlayManager instead.
     */
    template<typename T>
        requires std::is_base_of_v<Overlay, T>
    void removeOverlay()
    {
        std::vector<const Overlay *> overlaysToRemove;
        for (const auto &overlay : m_overlays) {
            if (dynamic_cast<T *>(overlay.get())) {
                overlaysToRemove.push_back(overlay.get());
            }
        }

        for (const auto *overlay : overlaysToRemove) {
            removeOverlay(overlay);
        }
    }

    bool hasOverlays() const;
    template<typename T>
        requires std::is_base_of_v<Overlay, T>
    bool hasOverlay() const
    {
        return std::ranges::any_of(m_overlays, [](const auto &overlay) {
            return dynamic_cast<T *>(overlay.get());
        });
    }

signals:
    void paintEventReceived();

protected:
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *) override;

private slots:
    void onScreenGeometryChanged(const QRect &geometry);

private:
    void overlaysChanged();

    QScreen *m_screen;
    LayerShellQt::Window *m_window;
    std::vector<std::unique_ptr<Overlay>> m_overlays;

    bool m_wantsKeyboardInput{};
    bool m_wantsMouseInput{};
};

}