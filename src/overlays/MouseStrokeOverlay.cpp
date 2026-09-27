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

#include "MouseStrokeOverlay.h"
#include <QGuiApplication>
#include <QPainter>
#include <QPalette>
#include <QWidget>

namespace InputActions::Overlay
{

MouseStrokeOverlay::MouseStrokeOverlay(QWidget *widget)
    : Overlay(widget)
    , m_pen(QGuiApplication::palette().accent().color())
{
    m_pen.setWidthF(4.0);
    m_pen.setCapStyle(Qt::RoundCap);
    m_pen.setJoinStyle(Qt::RoundJoin);
}

void MouseStrokeOverlay::enterEvent(QEnterEvent *event)
{
    m_path.moveTo(event->position());
}

void MouseStrokeOverlay::mouseMoveEvent(QMouseEvent *event)
{
    m_path.lineTo(event->position());
    widget()->update();
}

void MouseStrokeOverlay::paintEvent(QPaintEvent *event)
{
    QPainter painter(widget());
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(m_pen);
    painter.setBrush(Qt::NoBrush);

    painter.drawPath(m_path);
}

}