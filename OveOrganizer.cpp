#include "OveOrganizer.h"

namespace OVE {


int NoteTypeToTick(NoteType type, int quarter) {
      int c = 0x1 << int(type); // int(pow(2.0, int(type))) ; #include <math.h>
      return quarter * 4 * 2 / c ;
}      
	
// OveOrganize.cpp
OveOrganizer::OveOrganizer(OveSong* ove) {
      ove_ = ove;
      }

void OveOrganizer::organize() {
      if(ove_ == NULL) {
            return;
            }

      organizeTracks();
      organizeAttributes();
      organizeMeasures();
      }

void OveOrganizer::organizeAttributes() {
      int i;
      int j;
      int k;

      // key
      if(ove_->getLineCount() > 0) {
            Line* line = ove_->getLine(0);
            int partBarCount = ove_->getPartBarCount();
            int lastKey = 0;

            if(line != 0){
                  for(i=0; i<line->getStaffCount(); ++i) {
                        QPair<int, int> partStaff = ove_->trackToPartStaff(i);
                        Staff* staff = line->getStaff(i);
                        lastKey = staff->getKeyType();

                        for(j=0; j<partBarCount; ++j) {
                              MeasureData* measureData = ove_->getMeasureData(partStaff.first, partStaff.second, j);

                              if(measureData != 0) {
                                    Key* key = measureData->getKey();

                                    if( j==0 ) {
                                          key->setKey(lastKey);
                                          key->setPreviousKey(lastKey);
                                          }

                                    if( !key->getSetKey() ) {
                                          key->setKey(lastKey);
                                          key->setPreviousKey(lastKey);
                                          }
                                    else {
                                          if( key->getKey() != lastKey ) {
                                                lastKey = key->getKey();
                                                }
                                          }
                                    }
                              }
                        }
                  }
            }

      // clef
      if( ove_->getLineCount() > 0 ) {
            Line* line = ove_->getLine(0);
            int partBarCount = ove_->getPartBarCount();
            ClefType lastClefType = ClefType::Treble;

            if(line != 0){
                  for( i=0; i<line->getStaffCount(); ++i ) {
                        QPair<int, int> partStaff = ove_->trackToPartStaff(i);
                        Staff* staff = line->getStaff(i);
                        lastClefType = staff->getClefType();

                        for( j=0; j<partBarCount; ++j ) {
                              MeasureData* measureData = ove_->getMeasureData(partStaff.first, partStaff.second, j);

                              if(measureData != 0) {
                                    Clef* clefPtr = measureData->getClef();
                                    clefPtr->setClefType((int)lastClefType);

                                    const QList<MusicData*>& clefs = measureData->getMusicDatas(MusicDataType::Clef);

                                    for( k=0; k<clefs.size(); ++k ) {
                                          Clef* clef = static_cast<Clef*>(clefs[k]);
                                          lastClefType = clef->getClefType();
                                          }
                                    }
                              }
                        }
                  }
            }
      }

Staff* getStaff(OveSong* ove, int track) {
      if (ove->getLineCount() > 0) {
            Line* line = ove->getLine(0);
            if(line != 0 && line->getStaffCount() > 0) {
                  Staff* staff = line->getStaff(track);
                  return staff;
                  }
            }

      return 0;
      }

void OveOrganizer::organizeTracks() {
      int i;
      //QList<QPair<ClefType, int> > trackChannels;
      QList<Track*> tracks = ove_->getTracks();
      QList<bool> comboStaveStarts;

      for( i=0; i<tracks.size(); ++i ) {
            comboStaveStarts.push_back(false);
            }

      for( i=0; i<tracks.size(); ++i ) {
            Staff* staff = getStaff(ove_, i);
            if(staff != 0) {
                  if(staff->getGroupType() == GroupType::Brace && staff->getGroupStaffCount() == 1 ) {
                        comboStaveStarts[i] = true;
                        }
                  }

            /*if( i < tracks.size() - 1 ) {
         if( tracks[i]->getStartClef() == ClefType::Treble &&
            tracks[i+1]->getStartClef() == ClefType::Bass &&
            tracks[i]->getChannel() == tracks[i+1]->get_channel() ) {
         }
      }*/
            }

      int trackId = 0;
      QList<int> partStaffCounts;

      while( trackId < (int)tracks.size() ) {
            int partTrackCount = 1;

            if( comboStaveStarts[trackId] ) {
                  partTrackCount = 2;
                  }

            partStaffCounts.push_back(partTrackCount);
            trackId += partTrackCount;
            }

      ove_->setPartStaffCounts(partStaffCounts);
      }

void OveOrganizer::organizeMeasures() {
      int trackBarCount = ove_->getTrackBarCount();

      for( int i=0; i<ove_->getPartCount(); ++i ) {
            int partStaffCount = ove_->getStaffCount(i);

            for( int j=0; j<partStaffCount; ++j ) {
                  for( int k=0; k<trackBarCount; ++k ) {
                        Measure* measure = ove_->getMeasure(k);
                        MeasureData* measureData = ove_->getMeasureData(i, j, k);

                        organizeMeasure(i, j, measure, measureData);
                        }
                  }
            }
      }

void OveOrganizer::organizeMeasure(int part, int track, Measure* measure, MeasureData* measureData) {
      // note containers
      organizeContainers(part, track, measure, measureData);

      // single end data
      organizeMusicDatas(part, track, measure, measureData);

      // cross measure elements
      organizeCrossMeasureElements(part, track, measure, measureData);
      }

void addToList(QList<int>& list, int number) {
      for(int i=0; i<list.size(); ++i){
            if(list[i] == number){
                  return;
                  }
            }

      list.push_back(number);
      }

void OveOrganizer::organizeContainers(int /*part*/, int /*track*/,
                                      Measure* measure, MeasureData* measureData) {
      int i;
      QList<NoteContainer*> containers = measureData->getNoteContainers();
      int barUnits = measure->getTime()->getUnits();
      QList<int> voices;

      for(i=0; i<containers.size(); ++i){
            int endUnit = barUnits;
            if( i < containers.size() - 1 ) {
                  endUnit = containers[i+1]->start()->getOffset();
                  }

            containers[i]->stop()->setOffset(endUnit);
            addToList(voices, containers[i]->getVoice());
            }

      // shift voices
      qSort(voices.begin(), voices.end());

      for (i = 0; i < voices.size(); ++i) {
            int voice = voices[i];
            // voice -> i
            for(int j=0; j<(int)containers.size(); ++j) {
                  int avoice = containers[j]->getVoice();
                  if ( avoice == voice && avoice != i ) {
                        containers[j]->setVoice(i);
                        }
                  }
            }
      }

void OveOrganizer::organizeMusicDatas(int /*part*/, int /*track*/, Measure* measure, MeasureData* measureData) {
      int i;
      int barIndex = measure->getBarNumber()->getIndex();
      QList<MusicData*> datas = measureData->getMusicDatas(MusicDataType::None);

      for(i=0; i<datas.size(); ++i) {
            datas[i]->start()->setMeasure(barIndex);
            }
      }

void OveOrganizer::organizeCrossMeasureElements(int part, int track, Measure* measure, MeasureData* measureData) {
      int i;
      QList<MusicData*> pairs = measureData->getCrossMeasureElements(MusicDataType::None, MeasureData::PairType::Start);

      for(i=0; i<pairs.size(); ++i) {
            MusicData* pair = pairs[i];

            switch ( pair->getMusicDataType() ) {
                  case MusicDataType::Beam :
                  case MusicDataType::Glissando :
                  case MusicDataType::Slur :
                  case MusicDataType::Tie :
                  case MusicDataType::Tuplet :
                  case MusicDataType::Pedal :
                  case MusicDataType::Numeric_Ending :
                        //case MusicDataType::OctaveShift_EndPoint :
                  case MusicDataType::Measure_Repeat : {
                        organizePairElement(pair, part, track, measure, measureData);
                        break;
                        }
                  case MusicDataType::OctaveShift : {
                        OctaveShift* octave = static_cast<OctaveShift*>(pair);
                        organizeOctaveShift(octave, measure, measureData);
                        break;
                        }
                  case MusicDataType::Wedge : {
                        Wedge* wedge = static_cast<Wedge*>(pair);
                        organizeWedge(wedge, part, track, measure, measureData);
                        break;
                        }
                  default:
                        break;
                  }
            }
      }

void OveOrganizer::organizePairElement(
            MusicData* data,
            int part,
            int track,
            Measure* measure,
            MeasureData* measureData) {
      int bar1Index = measure->getBarNumber()->getIndex();
      int bar2Index = bar1Index + data->stop()->getMeasure();
      MeasureData* measureData2 = ove_->getMeasureData(part, track, bar2Index);

      data->start()->setMeasure(bar1Index);

      if(measureData2 != 0 && measureData != measureData2) {
            measureData2->addCrossMeasureElement(data, false);
            }

      if( data->getMusicDataType() == MusicDataType::Tuplet ){
            Tuplet* tuplet = static_cast<Tuplet*>(data);
            const QList<NoteContainer*> containers = measureData->getNoteContainers();

            for(int i=0; i<containers.size(); ++i){
                  if(containers[i]->getTick() > tuplet->getTick()){
                        break;
                        }

                  if(containers[i]->getTick() == tuplet->getTick()){
                        tuplet->setNoteType(containers[i]->getNoteType());
                        }
                  }

            int tupletTick = NoteTypeToTick(tuplet->getNoteType(), ove_->getQuarter())*tuplet->getSpace();
            if( tuplet->getTick() % tupletTick != 0 ) {
                  int newStartTick = (tuplet->getTick() / tupletTick) * tupletTick;

                  for(int i=0; i<containers.size(); ++i){
                        if( containers[i]->getTick() == newStartTick &&
                            containers[i]->getTuplet() == tuplet->getTuplet()) {
                              tuplet->setTick(containers[i]->getTick());
                              tuplet->start()->setOffset(containers[i]->start()->getOffset());
                              }
                        }
                  }
            }
      }

void OveOrganizer::organizeOctaveShift(
            OctaveShift* octave,
            Measure* measure,
            MeasureData* measureData) {
      // octave shift
      int i;
      const QList<NoteContainer*> containers = measureData->getNoteContainers();
      int barIndex = measure->getBarNumber()->getIndex();

      octave->start()->setMeasure(barIndex);

      for(i=0; i<containers.size(); ++i) {
            int noteShift = octave->getNoteShift();
            int containerTick = containers[i]->getTick();

            if( octave->getTick() <= containerTick && octave->getEndTick() > containerTick ) {
                  containers[i]->setNoteShift(noteShift);
                  }
            }
      }

bool getMiddleUnit(
            OveSong* ove, int /*part*/, int /*track*/,
            Measure* measure1, Measure* measure2, int unit1, int /*unit2*/,
            Measure* middleMeasure, int& middleUnit) {
      Q_UNUSED(middleMeasure); // avoid (bogus?) warning to show up
      QList<int> barUnits;
      int i;
      int bar1Index = measure1->getBarNumber()->getIndex();
      int bar2Index = measure2->getBarNumber()->getIndex();
      int sumUnit = 0;

      for( int j=bar1Index; j<=bar2Index; ++j ) {
            Measure* measure = ove->getMeasure(j);
            barUnits.push_back(measure->getTime()->getUnits());
            sumUnit += measure->getTime()->getUnits();
            }

      int currentSumUnit = 0;
      for( i=0; i<barUnits.size(); ++i ) {
            int barUnit = barUnits[i];

            if( i==0 ) {
                  barUnit = barUnits[i] - unit1;
                  }

            if( currentSumUnit + barUnit < sumUnit/2 ) {
                  currentSumUnit += barUnit;
                  }
            else {
                  break;
                  }
            }

      if( i < barUnits.size() ) {
            int barMiddleIndex = bar1Index + i;
            middleMeasure = ove->getMeasure(barMiddleIndex);
            middleUnit = sumUnit/2 - currentSumUnit;

            return true;
            }

      return false;
      }

void OveOrganizer::organizeWedge(Wedge* wedge, int part, int track, Measure* measure, MeasureData* measureData) {
      int bar1Index = measure->getBarNumber()->getIndex();
      int bar2Index = bar1Index + wedge->stop()->getMeasure();
      MeasureData* measureData2 = ove_->getMeasureData(part, track, bar2Index);
      WedgeType wedgeType = wedge->getWedgeType();

      if( wedge->getWedgeType() == WedgeType::Double_Line ) {
            wedgeType = WedgeType::Cres_Line;
            }

      wedge->start()->setMeasure(bar1Index);

      WedgeEndPoint* startPoint = new WedgeEndPoint();
      measureData->addMusicData(startPoint);

      startPoint->setTick(wedge->getTick());
      startPoint->start()->setOffset(wedge->start()->getOffset());
      startPoint->setWedgeStart(true);
      startPoint->setWedgeType(wedgeType);
      startPoint->setHeight(wedge->getHeight());

      WedgeEndPoint* stopPoint = new WedgeEndPoint();

      stopPoint->setTick(wedge->getTick());
      stopPoint->start()->setOffset(wedge->stop()->getOffset());
      stopPoint->setWedgeStart(false);
      stopPoint->setWedgeType(wedgeType);
      stopPoint->setHeight(wedge->getHeight());

      if(measureData2 != 0) {
            measureData2->addMusicData(stopPoint);
            }

      if( wedge->getWedgeType() == WedgeType::Double_Line ) {
            Measure* middleMeasure = NULL;
            int middleUnit = 0;

            getMiddleUnit(
                              ove_, part, track,
                              measure, ove_->getMeasure(bar2Index),
                              wedge->start()->getOffset(), wedge->stop()->getOffset(),
                              middleMeasure, middleUnit);

            if( middleUnit != 0 ) {
                  WedgeEndPoint* midStopPoint = new WedgeEndPoint();
                  measureData->addMusicData(midStopPoint);

                  midStopPoint->setTick(wedge->getTick());
                  midStopPoint->start()->setOffset(middleUnit);
                  midStopPoint->setWedgeStart(false);
                  midStopPoint->setWedgeType(WedgeType::Cres_Line);
                  midStopPoint->setHeight(wedge->getHeight());

                  WedgeEndPoint* midStartPoint = new WedgeEndPoint();
                  measureData->addMusicData(midStartPoint);

                  midStartPoint->setTick(wedge->getTick());
                  midStartPoint->start()->setOffset(middleUnit);
                  midStartPoint->setWedgeStart(true);
                  midStartPoint->setWedgeType(WedgeType::Decresc_Line);
                  midStartPoint->setHeight(wedge->getHeight());
                  }
            }
      }



}