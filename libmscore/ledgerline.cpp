//=============================================================================
//  MuseScore
//  Music Composition & Notation
//
//  Copyright (C) 2013 Werner Schweer
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License version 2
//  as published by the Free Software Foundation and appearing in
//  the file LICENCE.GPL
//=============================================================================

#include "ledgerline.h"
#include "chord.h"
#include "measure.h"
#include "system.h"
#include "staff.h"
#include "score.h"

namespace Ms {

//---------------------------------------------------------
//   LedgerLine
//---------------------------------------------------------

LedgerLine::LedgerLine(Score* s)
   : Line(s, false)
      {
      setZ(NOTE * 100 - 50);
      setSelectable(false);
      _staffLine = 0;
      _next = 0;
      }

//---------------------------------------------------------
//   pagePos
//---------------------------------------------------------

QPointF LedgerLine::pagePos() const
      {
      System* system = chord()->measure()->system();
      qreal yp = y() + system->staff(staffIdx())->y() + system->y();
      return QPointF(pageX(), yp);
      }

//---------------------------------------------------------
//   measureXPos
//---------------------------------------------------------

qreal LedgerLine::measureXPos() const
      {
      qreal xp = x();                   // chord relative
      xp += chord()->x();                // segment relative
      xp += chord()->segment()->x();     // measure relative
      return xp;
      }

//---------------------------------------------------------
//   layout
//---------------------------------------------------------

void LedgerLine::layout()
      {
      setLineWidth(score()->styleS(ST_ledgerLineWidth) * chord()->mag());
      Line::layout();
      }

//---------------------------------------------------------
//   draw
//---------------------------------------------------------

void LedgerLine::draw(QPainter* painter) const
      {
      if(chord()->crossMeasure() == CROSSMEASURE_SECOND)
            return;
      Line::draw(painter);

      // Draw interval guide line if requested
      if (!score()->showIntervalGuides())
            return;

      Staff* staff = chord()->staff();
      if (!staff)
            return;

      if (staff->staffType()->group() != STANDARD_STAFF_GROUP)
            return;

      int tick        = chord()->tick();
      ClefType clef   = staff->clef(tick);
      int pitchOffset = ClefInfo::pitchOffset(clef);
      int key         = staff->key(tick).accidentalType();

      // scale degree of the note at this ledger line
      int degree = (((7 - ((key + 7) / 2)) + (3 * ((key + 7) % 2)) + pitchOffset - _staffLine) % 7) + 1;

      qDebug("  staffLine:%d pitchOffset:%d key: %d degree:%d\n", _staffLine, pitchOffset, key, degree);

      QPointF _pos(0.0, 0.0);

      qreal x1 = _pos.x();
      qreal x2 = x1 + width();
      qreal y  = _pos.y();
      qreal lw = score()->styleS(ST_staffLineWidth).val() * spatium();

      switch(degree) {
             case 7:
                   y -= spatium() * 0.25;
                   painter->setPen(QPen(Qt::red, lw, Qt::SolidLine, Qt::FlatCap));
                   painter->drawLine(QLineF(x1, y, x2, y));
                   break;
             case 6:
             case 5:
                   break;
             case 4:
                   y += spatium() * 0.25;
                   painter->setPen(QPen(Qt::cyan, lw, Qt::SolidLine, Qt::FlatCap));
                   painter->drawLine(QLineF(x1, y, x2, y));
                   break;
             case 3:
                   y -= spatium() * 0.25;
                   painter->setPen(QPen(Qt::cyan, lw, Qt::SolidLine, Qt::FlatCap));
                   painter->drawLine(QLineF(x1, y, x2, y));
                   break;
             case 2:
                   break;
             case 1:
                   y += spatium() * 0.25;
                   painter->setPen(QPen(Qt::red, lw, Qt::SolidLine, Qt::FlatCap));
                   painter->drawLine(QLineF(x1, y, x2, y));
                   break;
             default:
                   break;
             }
      }
}
