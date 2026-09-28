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

#include "Overlay.h"
#include <QDBusConnection>
#include <QDBusMessage>
#include <QFuture>
#include <QMenu>
#include <queue>

namespace InputActions::Overlay
{

class ContextMenuOverlay;
class OverlayManager;

struct ScheduledContextMenu
{
    QString json;
    std::shared_ptr<QPromise<QString>> promise;
};

class ContextMenuOverlayShared : public QObject
{
    Q_OBJECT

public:
    ContextMenuOverlayShared() = default;

    std::queue<ScheduledContextMenu> &scheduledMenus() { return m_scheduledMenus; }

    /**
     * Schedules a context menu to be shown.
     * @returns A promise that is finished with the ID of the triggered action or an empty string.
     */
    QFuture<QString> addMenu(const QString &json);

signals:
    void menuAdded();
    void overlayFocused(ContextMenuOverlay *overlay);

private:
    std::queue<ScheduledContextMenu> m_scheduledMenus;
};

class ContextMenuOverlay : public Overlay
{
    Q_OBJECT

public:
    ContextMenuOverlay(ContextMenuOverlayShared &shared, OverlayManager &overlayManager, QWidget *widget);

    bool wantsKeyboardInput() override { return true; }
    bool wantsMouseInput() override { return true; }

    void enterEvent(QEnterEvent *event) override;

private slots:
    void onOverlayFocused(ContextMenuOverlay *overlay);

private:
    void showMenuOrHide(bool hide = false);
    void showMenu(ScheduledContextMenu scheduledMenu);

    QMenu *menuFromJson(const QString &json);
    void populateMenu(QMenu *menu, const QJsonArray &items);

    ContextMenuOverlayShared &m_shared;
    OverlayManager &m_overlayManager;
};

}