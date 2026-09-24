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

#include "DesktopEnvironment.h"
#include "MouseStrokeOverlay.h"
#include <QApplication>
#include <QTimer>
#include <csignal>

using namespace InputActions::Overlay;

void handleSignal(int signal)
{
    if (signal == SIGINT) {
        QCoreApplication::quit();
        std::signal(SIGINT, SIG_DFL);
    }
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    std::signal(SIGINT, handleSignal);

    auto bus = QDBusConnection::sessionBus();
    bus.registerService("org.inputactions.overlay");

    std::optional<MouseStrokeOverlay> mouseStrokeOverlay;
    if (desktopEnvironment() != DesktopEnvironment::GNOME) {
        mouseStrokeOverlay.emplace(bus);
    }

    return app.exec();
}