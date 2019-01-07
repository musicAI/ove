#ifndef _OVE_ELEMENT_H
#define _OVE_ELEMENT_H

#include "OveEnum.h"

namespace OVE{
      
class MusicData;
class OffsetElement;
class LineElement;
class PairEnds;

/////////////////////////////////////////////////////////////////////////////
// basic element
class TickElement {
public:
      TickElement();
      virtual ~TickElement() {}

public:
      void setTick(int tick);
      int getTick(void) const;

private:
      int tick_;
      };

class MeasurePos {
public:
      MeasurePos();
      virtual ~MeasurePos() {}

public:
      void setMeasure(int measure);
      int getMeasure() const;

      void setOffset(int offset);
      int getOffset() const;

      MeasurePos shiftMeasure(int measure) const;
      MeasurePos shiftOffset(int offset) const; // ignore cross measure

      bool operator ==(const MeasurePos& mp) const;
      bool operator !=(const MeasurePos& mp) const;
      bool operator <(const MeasurePos& mp) const;
      bool operator <=(const MeasurePos& mp) const;
      bool operator >(const MeasurePos& mp) const;
      bool operator >=(const MeasurePos& mp) const;

private:
      int measure_;
      int offset_;
      };

class PairElement {
public:
      PairElement();
      virtual ~PairElement();

public:
      MeasurePos* start() const;
      MeasurePos* stop() const;

private:
      MeasurePos* start_;
      MeasurePos* stop_;
      };

class PairEnds {
public:
      PairEnds();
      virtual ~PairEnds();

public:
      LineElement* getLeftLine() const;
      LineElement* getRightLine() const;

      OffsetElement* getLeftShoulder() const;
      OffsetElement* getRightShoulder() const;

private:
      LineElement* leftLine_;
      LineElement* rightLine_;
      OffsetElement* leftShoulder_;
      OffsetElement* rightShoulder_;
      };

class LineElement {
public:
      LineElement();
      virtual ~LineElement() {}

public:
      virtual void setLine(int line); // middle line (3rd line of each clef) is set 0
      virtual int getLine(void) const;

private:
      int line_;
      };

class OffsetElement {
public:
      OffsetElement();
      virtual ~OffsetElement() {}

public:
      virtual void setXOffset(int offset);
      virtual int getXOffset() const;

      virtual void setYOffset(int offset);
      virtual int getYOffset() const;

private:
      int xOffset_;
      int yOffset_;
      };

class LengthElement {
public:
      LengthElement();
      virtual ~LengthElement() {}

public:
      void setLength(int length);
      int getLength() const;

private:
      int length_; // tick
      };

// base class of many ove music element
class MusicData: public TickElement, public PairElement, public OffsetElement {
public:
      MusicData();
      virtual ~MusicData() {}

public:
      MusicDataType getMusicDataType() const;

      
      static XmlDataType getXmlDataType(MusicDataType type);
      //	static bool get_is_pair_element(MusicDataType type) ;

      // show / hide
      void setShow(bool show);
      bool getShow() const;

      // color
      void setColor(unsigned int color); // not exists in ove 3
      unsigned int getColor() const;

      void setVoice(unsigned int voice);
      unsigned int getVoice() const;

      void copyCommonBlock(const MusicData& source);

protected:
      MusicDataType musicDataType_;

private:
      bool show_;
      unsigned int color_;
      unsigned int voice_;
      };

class MidiData: public TickElement {
public:
      MidiData();
      virtual ~MidiData() {}

public:
      MidiType getMidiType() const;

protected:
      MidiType midiType_;
      };

}

#endif
      