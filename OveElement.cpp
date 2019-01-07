#include "OveElement.h"

namespace OVE {

/*template <class T>
inline void deleteVector(QList<T*>& vec) {
   for (int i=0; i<vec.size(); ++i)
      delete vec[i];
   }
   //vec.clear();
}*/

///////////////////////////////////////////////////////////////////////////////
TickElement::TickElement() {
      tick_ = 0;
      }

void TickElement::setTick(int tick) {
      tick_ = tick;
      }

int TickElement::getTick(void) const {
      return tick_;
      }

///////////////////////////////////////////////////////////////////////////////
MeasurePos::MeasurePos() {
      measure_ = 0;
      offset_ = 0;
      }

void MeasurePos::setMeasure(int measure) {
      measure_ = measure;
      }

int MeasurePos::getMeasure() const {
      return measure_;
      }

void MeasurePos::setOffset(int offset) {
      offset_ = offset;
      }

int MeasurePos::getOffset() const {
      return offset_;
      }

MeasurePos MeasurePos::shiftMeasure(int measure) const {
      MeasurePos mp;
      mp.setMeasure(getMeasure() + measure);
      mp.setOffset(getOffset());

      return mp;
      }

MeasurePos MeasurePos::shiftOffset(int offset) const {
      MeasurePos mp;
      mp.setMeasure(getMeasure());
      mp.setOffset(getOffset() + offset);

      return mp;
      }

bool MeasurePos::operator ==(const MeasurePos& mp) const {
      return getMeasure() == mp.getMeasure() && getOffset() == mp.getOffset();
      }

bool MeasurePos::operator !=(const MeasurePos& mp) const {
      return !(*this == mp);
      }

bool MeasurePos::operator <(const MeasurePos& mp) const {
      if (getMeasure() != mp.getMeasure()) {
            return getMeasure() < mp.getMeasure();
            }

      return getOffset() < mp.getOffset();
      }

bool MeasurePos::operator <=(const MeasurePos& mp) const {
      if (getMeasure() != mp.getMeasure()) {
            return getMeasure() <= mp.getMeasure();
            }

      return getOffset() <= mp.getOffset();
      }

bool MeasurePos::operator >(const MeasurePos& mp) const {
      return !(*this <= mp);
      }

bool MeasurePos::operator >=(const MeasurePos& mp) const {
      return !(*this < mp);
      }

///////////////////////////////////////////////////////////////////////////////
PairElement::PairElement() {
      start_ = new MeasurePos();
      stop_ = new MeasurePos();
      }

PairElement::~PairElement(){
      delete start_;
      delete stop_;
      }

MeasurePos* PairElement::start() const {
      return start_;
      }

MeasurePos* PairElement::stop() const {
      return stop_;
      }

///////////////////////////////////////////////////////////////////////////////
PairEnds::PairEnds() {
      leftLine_ = new LineElement();
      rightLine_ = new LineElement();
      leftShoulder_ = new OffsetElement();
      rightShoulder_ = new OffsetElement();
      }

PairEnds::~PairEnds(){
      delete leftLine_;
      delete rightLine_;
      delete leftShoulder_;
      delete rightShoulder_;
      }

LineElement* PairEnds::getLeftLine() const {
      return leftLine_;
      }

LineElement* PairEnds::getRightLine() const {
      return rightLine_;
      }

OffsetElement* PairEnds::getLeftShoulder() const {
      return leftShoulder_;
      }

OffsetElement* PairEnds::getRightShoulder() const {
      return rightShoulder_;
      }

///////////////////////////////////////////////////////////////////////////////
LineElement::LineElement() {
      line_ = 0;
      }

void LineElement::setLine(int line) {
      line_ = line;
      }

int LineElement::getLine(void) const {
      return line_;
      }

///////////////////////////////////////////////////////////////////////////////
OffsetElement::OffsetElement() {
      xOffset_ = 0;
      yOffset_ = 0;
      }

void OffsetElement::setXOffset(int offset) {
      xOffset_ = offset;
      }

int OffsetElement::getXOffset() const {
      return xOffset_;
      }

void OffsetElement::setYOffset(int offset) {
      yOffset_ = offset;
      }

int OffsetElement::getYOffset() const {
      return yOffset_;
      }

///////////////////////////////////////////////////////////////////////////////
LengthElement::LengthElement() {
      length_ = 0;
      }

void LengthElement::setLength(int length) {
      length_ = length;
      }

int LengthElement::getLength() const {
      return length_;
      }

///////////////////////////////////////////////////////////////////////////////
MusicData::MusicData() {
      musicDataType_ = MusicDataType::None;
      show_ = true;
      color_ = 0;
      voice_ = 0;
      }

MusicDataType MusicData::getMusicDataType() const {
      return musicDataType_;
      }

XmlDataType MusicData::getXmlDataType(MusicDataType type) {
      XmlDataType xmlType = XmlDataType::None;

      switch (type) {
            case MusicDataType::Measure_Repeat: {
                  xmlType = XmlDataType::Attributes;
                  break;
                  }
            case MusicDataType::Beam: {
                  xmlType = XmlDataType::NoteBeam;
                  break;
                  }
            case MusicDataType::Slur:
            case MusicDataType::Glissando:
            case MusicDataType::Tuplet:
            case MusicDataType::Tie: {
                  xmlType = XmlDataType::Notations;
                  break;
                  }
            case MusicDataType::Text:
            case MusicDataType::Repeat:
            case MusicDataType::Wedge:
            case MusicDataType::Dynamics:
            case MusicDataType::Pedal:
            case MusicDataType::OctaveShift_EndPoint: {
                  xmlType = XmlDataType::Direction;
                  break;
                  }
            default: {
                  xmlType = XmlDataType::None;
                  break;
                  }
            }

      return xmlType;
      }

/*bool MusicData::get_is_pair_element(MusicDataType type)
 {
 bool pair = false;

 switch ( type )
 {
 case MusicDataType::Numeric_Ending :
 case MusicDataType::Measure_Repeat :
 case MusicDataType::Wedge :
 case MusicDataType::OctaveShift :
 //case MusicDataType::OctaveShift_EndPoint :
 case MusicDataType::Pedal :
 case MusicDataType::Beam :
 case MusicDataType::Glissando :
 case MusicDataType::Slur :
 case MusicDataType::Tie :
 case MusicDataType::Tuplet :
 {
 pair = true;
 break;
 }
 default:
 break;
 }

 return pair;
 }*/

void MusicData::setShow(bool show) {
      show_ = show;
      }

bool MusicData::getShow() const {
      return show_;
      }

void MusicData::setColor(unsigned int color) {
      color_ = color;
      }

unsigned int MusicData::getColor() const {
      return color_;
      }

void MusicData::setVoice(unsigned int voice) {
      voice_ = voice;
      }

unsigned int MusicData::getVoice() const {
      return voice_;
      }

void MusicData::copyCommonBlock(const MusicData& source) {
      setTick(source.getTick());
      start()->setOffset(source.start()->getOffset());
      setColor(source.getColor());
      }

///////////////////////////////////////////////////////////////////////////////
MidiData::MidiData() {
      midiType_ = MidiType::None;
      }

MidiType MidiData::getMidiType() const {
      return midiType_;
      }


}