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

#include "ContextMenuOverlayInterface.h"
#include "OverlayManager.h"
#include "overlays/ContextMenuOverlay.h"

namespace InputActions::Overlay
{

ContextMenuOverlayInterface::ContextMenuOverlayInterface(OverlayManager &overlayManager, QDBusConnection &bus)
    : m_overlayManager(overlayManager)
    , m_bus(bus)
{
    bus.registerObject("/org/inputactions/overlay/ContextMenuOverlay", this, QDBusConnection::ExportAllSlots);
}

ContextMenuOverlayInterface::~ContextMenuOverlayInterface() = default;

void ContextMenuOverlayInterface::showMenu(const QString &json, const QDBusMessage &message)
{
    auto reply = message.createReply();
    if (!m_overlayManager.hasOverlay<ContextMenuOverlay>()) {
        reply << "";
        m_bus.send(reply);
        return;
    }

    message.setDelayedReply(true);
    m_shared->addMenu(json).then([this, reply = std::move(reply)](const QString &actionId) mutable {
        reply << actionId;
        m_bus.send(reply);
    });
}

void ContextMenuOverlayInterface::showOverlay(const QDBusMessage &message)
{
    auto reply = message.createReply();
    if (m_overlayManager.hasOverlay<ContextMenuOverlay>()) {
        reply << true;
        m_bus.send(reply);
        return;
    }

    message.setDelayedReply(true);
    m_shared = std::make_unique<ContextMenuOverlayShared>();
    m_overlayManager
        .addOverlay([this](auto *widget) {
            return std::make_unique<ContextMenuOverlay>(*m_shared, m_overlayManager, widget);
        })
        .then([this, reply = std::move(reply)]() mutable {
            reply << false;
            m_bus.send(reply);
        });
}

}