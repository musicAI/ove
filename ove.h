//=============================================================================
//  MusE Score
//  Linux Music Score Editor
//
//  Copyright (C) 2002-2009 Werner Schweer and others
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License version 2.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//=============================================================================

#ifndef OVE_DATA_H
#define OVE_DATA_H

#include "OveSerialize.h"

namespace OVE {

const int TWELVE_TONE = 12 ;
const int INVALID_NOTE = -1 ;
const int OCTAVE = 7 ;

}

////////////////////////////////////////////////////////////////////////////////


class MeasureToTick {
public:
      MeasureToTick();
      ~MeasureToTick();

public:
      void build(OVE::OveSong* ove, int quarter);

      int getTick(int measure, int tick_pos);
      static int unitToTick(int unit, int quarter);

      struct TimeTick   {
            int numerator_;
            int denominator_;
            int measure_;
            int tick_;
            bool isSymbol_;

            TimeTick():numerator_(4), denominator_(4), measure_(0), tick_(0), isSymbol_(false){}
            };
      QList<TimeTick> getTimeTicks() const;

private:
      int quarter_;
      OVE::OveSong* ove_;
      QList<TimeTick> tts_;
};

int getMeasureTick(int quarter, int num, int den);

OVE::Staff* getStaff(const OVE::OveSong* ove, int track);

const OVE::Tuplet* getTuplet(const QList<OVE::MusicData*>& tuplets, int unit);

int ContainerToTick(OVE::NoteContainer* container, int quarter);

int accidentalToAlter(OVE::AccidentalType type);

void getMiddleToneOctave(OVE::ClefType clef, OVE::ToneType& tone, int& octave);

OVE::ClefType getClefType(OVE::MeasureData* measure, int tick);

int getGraceLevel(const QList<OVE::NoteContainer*>& containers, int tick, int unit);

bool isRestDefaultLine(OVE::Note* rest, OVE::NoteType noteType);

QString OveDynamics_To_Dynamics(OVE::DynamicsType type);

#endif
