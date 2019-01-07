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

#include "ove.h"


//////////////////////////////////////////////////////////////////////////////////////


int getMeasureTick(int quarter, int num, int den){
      return quarter * 4 * num / den;
      }

MeasureToTick::MeasureToTick(){
      quarter_ = 480;
      ove_ = 0;
      }

MeasureToTick::~MeasureToTick(){
      }

void MeasureToTick::build(OVE::OveSong* ove, int quarter){
      unsigned int i;
      int currentTick = 0;
      unsigned int measureCount = ove->getMeasureCount();

      quarter_ = quarter;
      ove_ = ove;
      tts_.clear();

      for(i=0; i<measureCount; ++i) {
            OVE::Measure* measure = ove_->getMeasure(i);
            OVE::TimeSignature* time = measure->getTime();
            TimeTick tt;
            bool change = false;

            tt.tick_ = currentTick;
            tt.numerator_ = time->getNumerator();
            tt.denominator_ = time->getDenominator();
            tt.measure_ = i;
            tt.isSymbol_ = time->getIsSymbol();

            if( i == 0 ){
                  change = true;
                  } else {
                  OVE::TimeSignature* previousTime = ove_->getMeasure(i-1)->getTime();

                  if( time->getNumerator() != previousTime->getNumerator() ||
                      time->getDenominator() != previousTime->getDenominator() ){
                        change = true;
                        } else if(time->getIsSymbol() != previousTime->getIsSymbol()){
                        change = true;
                        }
                  }

            if( change ){
                  tts_.push_back(tt);
                  }

            currentTick += getMeasureTick(quarter_, tt.numerator_, tt.denominator_);
            }
      }

int MeasureToTick::getTick(int measure, int tick_pos){
      TimeTick tt;

      for(int i=0; i<tts_.size(); ++i) {
            if( measure >= tts_[i].measure_ && ( i==tts_.size()-1 || measure < tts_[i+1].measure_ ) ) {
                  int measuresTick = (measure - tts_[i].measure_) * getMeasureTick(quarter_, tts_[i].numerator_, tts_[i].denominator_);
                  return tts_[i].tick_ + measuresTick + tick_pos;
                  }
            }

      return 0;
      }

int MeasureToTick::unitToTick(int unit, int quarter) {
      // 0x100 correspond to quarter tick
      float ratio = (float)unit / (float)256.0;
      int tick = ratio * quarter;
      return tick;
      }

QList<MeasureToTick::TimeTick> MeasureToTick::getTimeTicks() const {
      return tts_;
      }


OVE::Staff* getStaff(const OVE::OveSong* ove, int track) {
      if (ove->getLineCount() > 0) {
            OVE::Line* line = ove->getLine(0);
            if(line != 0 && line->getStaffCount() > 0) {
                  OVE::Staff* staff = line->getStaff(track);
                  return staff;
                  }
            }

      return 0;
}      

const OVE::Tuplet* getTuplet(const QList<OVE::MusicData*>& tuplets, int unit){
      for(int i=0; i<tuplets.size(); ++i){
            const OVE::MusicData* data = tuplets[i];
            if(unit >= data->start()->getOffset() && unit <= data->stop()->getOffset()){
                  const OVE::Tuplet* tuplet = static_cast<OVE::Tuplet*>(tuplets[i]);
                  return tuplet;
                  }
            }
      return 0;
}

int ContainerToTick(OVE::NoteContainer* container, int quarter){
      int tick = OVE::NoteTypeToTick(container->getNoteType(), quarter);

      int dotLength = tick;
      for( int i=0; i<container->getDot(); ++i ) {
            dotLength /= 2;
            }
      dotLength = tick - dotLength;

      if(container->getTuplet() > 0) {
            tick = tick * container->getSpace() / container->getTuplet();
            }

      tick += dotLength;

      return tick;
}

int accidentalToAlter(OVE::AccidentalType type) {
      int alter = 0;

      switch( type ) {
            case OVE::AccidentalType::Normal:
            case OVE::AccidentalType::Natural:
            case OVE::AccidentalType::Natural_Caution: {
                  alter = 0;
                  break;
                  }
            case OVE::AccidentalType::Sharp:
            case OVE::AccidentalType::Sharp_Caution: {
                  alter = 1;
                  break;
                  }
            case OVE::AccidentalType::Flat:
            case OVE::AccidentalType::Flat_Caution: {
                  alter = -1;
                  break;
                  }
            case OVE::AccidentalType::DoubleSharp:
            case OVE::AccidentalType::DoubleSharp_Caution: {
                  alter = 2;
                  break;
                  }
            case OVE::AccidentalType::DoubleFlat:
            case OVE::AccidentalType::DoubleFlat_Caution: {
                  alter = -2;
                  break;
                  }
            default:
                  break;
            }

      return alter;
}

void getMiddleToneOctave(OVE::ClefType clef, OVE::ToneType& tone, int& octave) {
      tone = OVE::ToneType::B;
      octave = 4;

      switch ( clef ) {
            case OVE::ClefType::Treble: {
                  tone = OVE::ToneType::B;
                  octave = 4;
                  break;
                  }
            case OVE::ClefType::Treble8va: {
                  tone = OVE::ToneType::B;
                  octave = 5;
                  break;
                  }
            case OVE::ClefType::Treble8vb: {
                  tone = OVE::ToneType::B;
                  octave = 3;
                  break;
                  }
            case OVE::ClefType::Bass: {
                  tone = OVE::ToneType::D;
                  octave = 3;
                  break;
                  }
            case OVE::ClefType::Bass8va: {
                  tone = OVE::ToneType::D;
                  octave = 4;
                  break;
                  }
            case OVE::ClefType::Bass8vb: {
                  tone = OVE::ToneType::D;
                  octave = 2;
                  break;
                  }
            case OVE::ClefType::Alto: {
                  tone = OVE::ToneType::C;
                  octave = 4;
                  break;
                  }
            case OVE::ClefType::UpAlto: {
                  tone = OVE::ToneType::A;
                  octave = 3;
                  break;
                  }
            case OVE::ClefType::DownDownAlto: {
                  tone = OVE::ToneType::G;
                  octave = 4;
                  break;
                  }
            case OVE::ClefType::DownAlto: {
                  tone = OVE::ToneType::E;
                  octave = 4;
                  break;
                  }
            case OVE::ClefType::UpUpAlto: {
                  tone = OVE::ToneType::F;
                  octave = 3;
                  break;
                  }
            case OVE::ClefType::Percussion1: {
                  tone = OVE::ToneType::A;
                  octave = 3;
                  break;
                  }
            case OVE::ClefType::Percussion2: {
                  tone = OVE::ToneType::A;
                  octave = 3;
                  break;
                  }
            case OVE::ClefType::TAB: {
                  break;
                  }
            default:
                  break;
            }
      }

OVE::ClefType getClefType(OVE::MeasureData* measure, int tick) {
      OVE::ClefType type = measure->getClef()->getClefType();
      QList<OVE::MusicData*> clefs = measure->getMusicDatas(OVE::MusicDataType::Clef);

      for(int i=0; i<clefs.size(); ++i){
            if(tick < clefs[i]->getTick()){
                  break;
                  }
            if(tick >= clefs[i]->getTick()){
                  OVE::Clef* clef = static_cast<OVE::Clef*>(clefs[i]);
                  type = clef->getClefType();
                  }
            }

      return type;
}

// beam in grace
int getGraceLevel(const QList<OVE::NoteContainer*>& containers, int tick, int unit){
      int graceCount = 0;
      int level = 0; // normal chord rest

      for(int i=0; i<containers.size(); ++i){
            OVE::NoteContainer* container = containers[i];
            if(container->getTick() > tick)
                  break;

            if(container->getIsGrace() && container->getTick() == tick){
                  ++graceCount;

                  if(unit <= container->start()->getOffset()){
                        ++level;
                        }
                  }
            }

      return level;
      }

bool isRestDefaultLine(OVE::Note* rest, OVE::NoteType noteType) {
      bool isDefault = true;
      switch (noteType) {
            case OVE::NoteType::Note_DoubleWhole:
            case OVE::NoteType::Note_Whole:
            case OVE::NoteType::Note_Half:
            case OVE::NoteType::Note_Quarter: {
                  if(rest->getLine() != 0)
                        isDefault = false;
                  break;
                  }
            case OVE::NoteType::Note_Eight: {
                  if(rest->getLine() != 1)
                        isDefault = false;
                  break;
                  }
            case OVE::NoteType::Note_Sixteen:
            case OVE::NoteType::Note_32: {
                  if(rest->getLine() != -1)
                        isDefault = false;
                  break;
                  }
            case OVE::NoteType::Note_64: {
                  if(rest->getLine() != -3)
                        isDefault = false;
                  break;
                  }
            case OVE::NoteType::Note_128: {
                  if(rest->getLine() != -4)
                        isDefault = false;
                  break;
                  }
            default: {
                  break;
                  }
            }

      return isDefault;
}

QString OveDynamics_To_Dynamics(OVE::DynamicsType type){
      QString dynamic = "other-dynamics";

      switch( type ){
            case OVE::DynamicsType::PPPP:
                  dynamic = "pppp";
                  break;
            case OVE::DynamicsType::PPP:
                  dynamic = "ppp";
                  break;
            case OVE::DynamicsType::PP:
                  dynamic = "pp";
                  break;
            case OVE::DynamicsType::P:
                  dynamic = "p";
                  break;
            case OVE::DynamicsType::MP:
                  dynamic = "mp";
                  break;
            case OVE::DynamicsType::MF:
                  dynamic = "mf";
                  break;
            case OVE::DynamicsType::F:
                  dynamic = "f";
                  break;
            case OVE::DynamicsType::FF:
                  dynamic = "ff";
                  break;
            case OVE::DynamicsType::FFF:
                  dynamic = "fff";
                  break;
            case OVE::DynamicsType::FFFF:
                  dynamic = "ffff";
                  break;
            case OVE::DynamicsType::SF:
                  dynamic = "sf";
                  break;
            case OVE::DynamicsType::FZ:
                  dynamic = "fz";
                  break;
            case OVE::DynamicsType::SFZ:
                  dynamic = "sfz";
                  break;
            case OVE::DynamicsType::SFFZ:
                  dynamic = "sffz";
                  break;
            case OVE::DynamicsType::FP:
                  dynamic = "fp";
                  break;
            case OVE::DynamicsType::SFP:
                  dynamic = "sfp";
                  break;
            default:
                  break;
            }

      return dynamic;
}


