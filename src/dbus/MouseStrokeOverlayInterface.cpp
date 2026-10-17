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

#include "MouseStrokeOverlayInterface.h"
#include "OverlayManager.h"
#include "overlays/MouseStrokeOverlay.h"

namespace InputActions::Overlay
{

MouseStrokeOverlayInterface::MouseStrokeOverlayInterface(OverlayManager &overlayManager, QDBusConnection &bus)
    : m_overlayManager(overlayManager)
{
    bus.registerObject("/org/inputactions/overlay/MouseStrokeOverlay", this, QDBusConnection::ExportAllSlots);
}

void MouseStrokeOverlayInterface::show()
{
    m_overlayManager.addOverlay([](auto *widget) {
        return std::make_unique<MouseStrokeOverlay>(widget);
    });
}

void MouseStrokeOverlayInterface::hide()
{
    m_overlayManager.removeOverlay<MouseStrokeOverlay>();
}

}