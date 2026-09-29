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
#include <QEnterEvent>
#include <QOpenGLWidget>

namespace InputActions::Overlay
{

class Overlay;

class OverlayWidget : public QOpenGLWidget
{
public:
    OverlayWidget(QScreen *screen);
    ~OverlayWidget() override;

    void addOverlay(std::unique_ptr<Overlay> overlay);
    template<typename T>
        requires std::is_base_of_v<Overlay, T>
    void removeOverlay()
    {
        const auto removed = std::erase_if(m_overlays, [](const auto &overlay) {
            return dynamic_cast<T *>(overlay.get());
        });
        if (removed) {
            overlaysChanged();
        }
    }
    bool hasOverlays() const;

    void hide();
    void show();

protected:
    void enterEvent(QEnterEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *) override;

private slots:
    void onScreenGeometryChanged(const QRect &geometry);

private:
    void cancelHide();
    void overlaysChanged();

    QScreen *m_screen;
    LayerShellQt::Window *m_window;
    std::vector<std::unique_ptr<Overlay>> m_overlays;
    bool m_hideScheduled{};

    bool m_wantsKeyboardInput{};
    bool m_wantsMouseInput{};
};

}