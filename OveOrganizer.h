#ifndef _OVE_ORGANIZER_H
#define _OVE_ORGANIZER_H

#include "OveScore.h"

// OveOrganizer.h

namespace OVE{

int NoteTypeToTick(NoteType type, int quarter);

class OveOrganizer {
public:
      OveOrganizer(OveSong* ove) ;
      virtual ~OveOrganizer(){}

public:
      void organize() ;

private:
      void organizeAttributes() ;
      void organizeTracks() ;
      void organizeMeasures() ;
      void organizeMeasure(int part, int track, Measure* measure, MeasureData* measureData) ;

      void organizeContainers(int part, int track, Measure* measure, MeasureData* measureData) ;
      void organizeMusicDatas(int part, int track, Measure* measure, MeasureData* measureData) ;
      void organizeCrossMeasureElements(int part, int track, Measure* measure, MeasureData* measureData) ;

      void organizePairElement(MusicData* data, int part, int track, Measure* measure, MeasureData* measureData) ;
      void organizeOctaveShift(OctaveShift* octave, Measure* measure, MeasureData* measureData) ;
      void organizeWedge(Wedge* wedge, int part, int track, Measure* measure, MeasureData* measureData) ;

private:
      OveSong* ove_ ;
      };

}

#endif // _OVE_ORGANIZER_H