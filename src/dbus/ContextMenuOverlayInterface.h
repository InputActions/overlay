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

#include "overlays/ContextMenuOverlay.h"
#include <QDBusConnection>
#include <QObject>

namespace InputActions::Overlay
{

class ContextMenuOverlayShared;
class OverlayManager;

class ContextMenuOverlayInterface : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.inputactions.overlay.ContextMenuOverlay")

public:
    ContextMenuOverlayInterface(OverlayManager &overlayManager, QDBusConnection &bus);
    ~ContextMenuOverlayInterface() override;

public slots:
    Q_NOREPLY void showMenu(const QString &json, const QDBusMessage &message);
    Q_NOREPLY void showOverlay(const QDBusMessage &message);

private:
    std::unique_ptr<ContextMenuOverlayShared> m_shared;

    OverlayManager &m_overlayManager;
    QDBusConnection &m_bus;
};

}