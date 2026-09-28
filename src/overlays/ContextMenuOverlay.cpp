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

#include "ContextMenuOverlay.h"
#include "OverlayManager.h"
#include <QGuiApplication>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

namespace InputActions::Overlay
{

ContextMenuOverlay::ContextMenuOverlay(ContextMenuOverlayShared &shared, OverlayManager &overlayManager, QWidget *widget)
    : Overlay(widget)
    , m_shared(shared)
    , m_overlayManager(overlayManager)
{
    connect(&m_shared, &ContextMenuOverlayShared::overlayFocused, this, &ContextMenuOverlay::onOverlayFocused);
    connect(&m_shared, &ContextMenuOverlayShared::menuAdded, this, [this]() {
        if (hasPointerFocus()) {
            showMenuOrHide();
        }
    });
}

void ContextMenuOverlay::enterEvent(QEnterEvent *event)
{
    Overlay::enterEvent(event);

    Q_EMIT m_shared.overlayFocused(this);
    showMenuOrHide();
}

void ContextMenuOverlay::showMenuOrHide(bool hide)
{
    auto &scheduledMenus = m_shared.scheduledMenus();
    if (scheduledMenus.empty()) {
        if (hide) {
            m_overlayManager.removeOverlay<ContextMenuOverlay>();
        }
        return;
    }

    ScheduledContextMenu scheduledMenu = scheduledMenus.front();
    scheduledMenus.pop();
    showMenu(scheduledMenu);
    showMenuOrHide(true);
}

void ContextMenuOverlay::showMenu(ScheduledContextMenu scheduledMenu)
{
    auto *menu = menuFromJson(scheduledMenu.json);
    auto *action = menu->exec(QCursor::pos());

    scheduledMenu.promise->addResult(action ? action->data().toString() : "");
    scheduledMenu.promise->finish();
    menu->deleteLater();
}

QMenu *ContextMenuOverlay::menuFromJson(const QString &json)
{
    auto *menu = new QMenu(widget());
    populateMenu(menu, QJsonDocument::fromJson(json.toUtf8()).array());
    return menu;
}

void ContextMenuOverlay::populateMenu(QMenu *menu, const QJsonArray &items)
{
    for (const auto &item : items) {
        const auto object = item.toObject();
        const auto type = object["type"].toString();

        if (type == "action") {
            const auto id = object["id"].toString();
            const auto text = object["text"].toString();
            const auto icon = object["icon"].toString();

            auto *action = new QAction(QIcon::fromTheme(icon), text, menu);
            action->setData(id);
            menu->addAction(action);
        } else if (type == "menu") {
            const auto text = object["text"].toString();
            const auto icon = object["icon"].toString();

            populateMenu(menu->addMenu(QIcon::fromTheme(icon), text), object["items"].toArray());
        } else if (type == "section") {
            const auto text = object["text"].toString();
            menu->addSection(text);
        } else if (type == "separator") {
            menu->addSeparator();
        }
    }
}

void ContextMenuOverlay::onOverlayFocused(ContextMenuOverlay *overlay)
{
    if (overlay != this) {
        m_overlayManager.removeOverlay(this);
    }
}

QFuture<QString> ContextMenuOverlayShared::addMenu(const QString &json)
{
    auto promise = std::make_shared<QPromise<QString>>();
    promise->start();

    m_scheduledMenus.emplace(json, promise);
    Q_EMIT menuAdded();

    return promise->future();
}

}