#include "OveScore.h"

namespace OVE {

///////////////////////////////////////////////////////////////////////////////
OveSong::OveSong() :
      codec_(0) {
      clear();
      }

OveSong::~OveSong() {
      clear();
      }

void OveSong::setIsVersion4(bool version4){
      version4_ = version4;
      }

bool OveSong::getIsVersion4() const {
      return version4_;
      }

void OveSong::setQuarter(int tick) {
      quarter_ = tick;
      }

int OveSong::getQuarter(void) const {
      return quarter_;
      }

void OveSong::setShowPageMargin(bool show){
      showPageMargin_ = show;
      }

bool OveSong::getShowPageMargin() const {
      return showPageMargin_;
      }

void OveSong::setShowTransposeTrack(bool show) {
      showTransposeTrack = show;
      }

bool OveSong::getShowTransposeTrack() const {
      return showTransposeTrack;
      }

void OveSong::setShowLineBreak(bool show) {
      showLineBreak_ = show;
      }

bool OveSong::getShowLineBreak() const {
      return showLineBreak_;
      }

void OveSong::setShowRuler(bool show) {
      showRuler_ = show;
      }

bool OveSong::getShowRuler() const {
      return showRuler_;
      }

void OveSong::setShowColor(bool show) {
      showColor_ = show;
      }

bool OveSong::getShowColor() const {
      return showColor_;
      }

void OveSong::setPlayRepeat(bool play) {
      playRepeat_ = play;
      }

bool OveSong::getPlayRepeat() const {
      return playRepeat_;
      }

void OveSong::setPlayStyle(PlayStyle style) {
      playStyle_ = style;
      }

OveSong::PlayStyle OveSong::getPlayStyle() const {
      return playStyle_;
      }

void OveSong::addTitle(const QString& str) {
      titles_.push_back(str);
      }

QList<QString> OveSong::getTitles(void) const {
      return titles_;
      }

void OveSong::addAnnotate(const QString& str) {
      annotates_.push_back(str);
      }

QList<QString> OveSong::getAnnotates(void) const {
      return annotates_;
      }

void OveSong::addWriter(const QString& str) {
      writers_.push_back(str);
      }

QList<QString> OveSong::getWriters(void) const {
      return writers_;
      }

void OveSong::addCopyright(const QString& str) {
      copyrights_.push_back(str);
      }

QList<QString> OveSong::getCopyrights(void) const {
      return copyrights_;
      }

void OveSong::addHeader(const QString& str) {
      headers_.push_back(str);
      }

QList<QString> OveSong::getHeaders(void) const {
      return headers_;
      }

void OveSong::addFooter(const QString& str) {
      footers_.push_back(str);
      }

QList<QString> OveSong::getFooters(void) const {
      return footers_;
      }

void OveSong::addTrack(Track* ptr) {
      tracks_.push_back(ptr);
      }

int OveSong::getTrackCount(void) const {
      return tracks_.size();
      }

QList<Track*> OveSong::getTracks() const {
      return tracks_;
      }

void OveSong::setTrackBarCount(int count) {
      trackBarCount_ = count;
      }

int OveSong::getTrackBarCount() const {
      return trackBarCount_;
      }

Track* OveSong::getTrack(int part, int staff) const {
      int trackId = partStaffToTrack(part, staff);

      if( trackId >=0 && trackId < (int)tracks_.size() ) {
            return tracks_[trackId];
            }

      return 0;
      }

bool OveSong::addPage(Page* page) {
      pages_.push_back(page);
      return true;
      }

int OveSong::getPageCount() const {
      return pages_.size();
      }

Page* OveSong::getPage(int idx) {
      if( idx>=0 && idx<(int)pages_.size() ) {
            return pages_[idx];
            }

      return 0;
      }

void OveSong::addLine(Line* ptr) {
      lines_.push_back(ptr);
      }

int OveSong::getLineCount() const {
      return lines_.size();
      }

Line* OveSong::getLine(int idx) const {
      if( idx >=0 && idx<(int)lines_.size() ) {
            return lines_[idx];
            }

      return 0;
      }

void OveSong::addMeasure(Measure* ptr) {
      measures_.push_back(ptr);
      }

int OveSong::getMeasureCount(void) const {
      return measures_.size();
      }

Measure* OveSong::getMeasure(int bar) const {
      if( bar >= 0 && bar < (int)measures_.size() ) {
            return measures_[bar];
            }

      return 0;
      }

void OveSong::addMeasureData(MeasureData* ptr) {
      measureDatas_.push_back(ptr);
      }

int OveSong::getMeasureDataCount(void) const {
      return measureDatas_.size();
      }

MeasureData* OveSong::getMeasureData(int part, int staff/*=0*/, int bar) const {
      int trackId = partStaffToTrack(part, staff);
      int trackBarCount = getTrackBarCount();

      if( bar >= 0 && bar < trackBarCount ) {
            int measureId = trackBarCount * trackId + bar;

            if( measureId >=0 && measureId < (int)measureDatas_.size() ) {
                  return measureDatas_[measureId];
                  }
            }

      return 0;
      }

MeasureData* OveSong::getMeasureData(int track, int bar) const {
      int id = trackBarCount_*track + bar;

      if( id >=0 && id < (int)measureDatas_.size() ) {
            return measureDatas_[id];
            }

      return 0;
      }

void OveSong::setPartStaffCounts(const QList<int>& partStaffCounts) {
      //partStaffCounts_.assign(partStaffCounts.begin(), partStaffCounts.end());
      for(int i=0; i<partStaffCounts.size(); ++i) {
            partStaffCounts_.push_back(partStaffCounts[i]);
            }
      }

int OveSong::getPartCount() const {
      return partStaffCounts_.size();
      }

int OveSong::getStaffCount(int part) const {
      if( part>=0 && part<(int)partStaffCounts_.size() ) {
            return partStaffCounts_[part];
            }

      return 0;
      }

int OveSong::getPartBarCount() const {
      return measureDatas_.size() / tracks_.size();
      }

QPair<int, int> OveSong::trackToPartStaff(int track) const {
      QPair<int, int> partStaff;
      int i;
      int staffCount = 0;

      for( i=0; i<partStaffCounts_.size(); ++i ) {
            if( staffCount + partStaffCounts_[i] > track ) {
                  return qMakePair((int)i, track-staffCount);
                  }

            staffCount += partStaffCounts_[i];
            }

      return qMakePair((int)partStaffCounts_.size(), 0);
      }

int OveSong::partStaffToTrack(int part, int staff) const {
      int i;
      unsigned int staffCount = 0;

      for( i=0; i<partStaffCounts_.size(); ++i ) {
            if( part == (int)i && staff>=0 && staff<(int)partStaffCounts_[i] ) {
                  int trackId = staffCount + staff;

                  if( trackId >=0 && trackId < (int)tracks_.size() ) {
                        return trackId;
                        }
                  }

            staffCount += partStaffCounts_[i];
            }

      return tracks_.size();
      }

void OveSong::setTextCodecName(const QString& codecName) {
      codec_ = QTextCodec::codecForName(codecName.toLatin1());
      }

QString OveSong::getCodecString(const QByteArray& text) {
      QString s;
      if (codec_ == NULL)
            s = QString(text);
      else
            s = codec_->toUnicode(text);

      s = s.trimmed();
      return s;
      }

void OveSong::clear(void)
      {
      version4_ = true;
      quarter_ = 480;
      showPageMargin_ = false;
      showTransposeTrack = false;
      showLineBreak_ = false;
      showRuler_ = false;
      showColor_ = true;
      playRepeat_ = true;
      playStyle_ = PlayStyle::Record;

      annotates_.clear();
      copyrights_.clear();
      footers_.clear();
      headers_.clear();
      titles_.clear();
      writers_.clear();

      // deleteVector(tracks_);
      for(int i=0; i<tracks_.size(); ++i){
            delete tracks_[i];
            }
      for(int i=0; i<pages_.size(); ++i){
            delete pages_[i];
            }
      for(int i=0; i<lines_.size(); ++i){
            delete lines_[i];
            }
      for(int i=0; i<measures_.size(); ++i){
            delete measures_[i];
            }
      for(int i=0; i<measureDatas_.size(); ++i){
            delete measureDatas_[i];
            }
      tracks_.clear();
      pages_.clear();
      lines_.clear();
      measures_.clear();
      measureDatas_.clear();
      trackBarCount_ = 0;
      partStaffCounts_.clear();
      }

///////////////////////////////////////////////////////////////////////////////
Voice::Voice() {
      channel_ = 0;
      volume_ = -1;
      pitchShift_ = 0;
      pan_ = 0;
      patch_ = 0;
      stemType_ = 0;
      }

void Voice::setChannel(int channel) {
      channel_ = channel;
      }

int Voice::getChannel() const {
      return channel_;
      }

void Voice::setVolume(int volume) {
      volume_ = volume;
      }

int Voice::getVolume() const {
      return volume_;
      }

void Voice::setPitchShift(int pitchShift) {
      pitchShift_ = pitchShift;
      }

int Voice::getPitchShift() const {
      return pitchShift_;
      }

void Voice::setPan(int pan) {
      pan_ = pan;
      }

int Voice::getPan() const {
      return pan_;
      }

void Voice::setPatch(int patch) {
      patch_ = patch;
      }

int Voice::getPatch() const {
      return patch_;
      }

void Voice::setStemType(int stemType) {
      stemType_ = stemType;
      }

int Voice::getStemType() const {
      return stemType_;
      }

int Voice::getDefaultPatch() {
      return -1;
      }

int Voice::getDefaultVolume() {
      return -1;
      }

///////////////////////////////////////////////////////////////////////////////
Track::Track() {
      clear();
      }

Track::~Track() {
      clear();
      }

void Track::setName(const QString& str) {
      name_ = str;
      }

QString Track::getName(void) const {
      return name_;
      }

void Track::setBriefName(const QString& str) {
      briefName_ = str;
      }

QString Track::getBriefName(void) const {
      return briefName_;
      }

void Track::setPatch(unsigned int patch) {
      patch_ = patch;
      }

unsigned int Track::getPatch() const {
      return patch_;
      }

void Track::setChannel(int channel) {
      channel_ = channel;
      }

int Track::getChannel() const {
      return channel_;
      }

void Track::setShowName(bool show) {
      showName_ = show;
      }

bool Track::getShowName() const {
      return showName_;
      }

void Track::setShowBriefName(bool show) {
      showBriefName_ = show;
      }

bool Track::getShowBriefName() const {
      return showBriefName_;
      }

void Track::setMute(bool mute) {
      mute_ = mute;
      }

bool Track::getMute() const {
      return mute_;
      }

void Track::setSolo(bool solo) {
      solo_ = solo;
      }

bool Track::getSolo() const {
      return solo_;
      }

void Track::setShowKeyEachLine(bool show) {
      showKeyEachLine_ = show;
      }

bool Track::getShowKeyEachLine() const {
      return showKeyEachLine_;
      }

void Track::setVoiceCount(int voices) {
      voiceCount_ = voices;
      }

int Track::getVoiceCount() const {
      return voiceCount_;
      }

void Track::addVoice(Voice* voice) {
      voices_.push_back(voice);
      }

QList<Voice*> Track::getVoices() const {
      return voices_;
      }

void Track::setShowTranspose(bool show) {
      showTranspose_ = show;
      }

bool Track::getShowTranspose() const {
      return showTranspose_;
      }

void Track::setTranspose(int transpose) {
      transpose_ = transpose;
      }

int Track::getTranspose() const {
      return transpose_;
      }

void Track::setNoteShift(int shift) {
      noteShift_ = shift;
      }

int Track::getNoteShift() const {
      return noteShift_;
      }

void Track::setStartClef(int clef/*in Clef*/) {
      startClef_ = ClefType(clef);
      }

ClefType Track::getStartClef() const {
      return startClef_;
      }

void Track::setTransposeClef(int clef) {
      transposeClef_ = ClefType(clef);
      }

ClefType Track::getTansposeClef() const {
      return transposeClef_;
      }

void Track::setStartKey(int key) {
      startKey_ = key;
      }

int Track::getStartKey() const {
      return startKey_;
      }

void Track::setDisplayPercent(unsigned int percent/*25~100?*/) {
      displayPercent_ = percent;
      }

unsigned int Track::getDisplayPercent() const {
      return displayPercent_;
      }

void Track::setShowLegerLine(bool show) {
      showLegerLine_ = show;
      }

bool Track::getShowLegerLine() const {
      return showLegerLine_;
      }

void Track::setShowClef(bool show) {
      showClef_ = show;
      }

bool Track::getShowClef() const {
      return showClef_;
      }

void Track::setShowTimeSignature(bool show) {
      showTimeSignature_ = show;
      }

bool Track::getShowTimeSignature() const {
      return showTimeSignature_;
      }

void Track::setShowKeySignature(bool show) {
      showKeySignature_ = show;
      }

bool Track::getShowKeySignature() const {
      return showKeySignature_;
      }

void Track::setShowBarline(bool show) {
      showBarline_ = show;
      }

bool Track::getShowBarline() const {
      return showBarline_;
      }

void Track::setFillWithRest(bool fill) {
      fillWithRest_ = fill;
      }

bool Track::getFillWithRest() const {
      return fillWithRest_;
      }

void Track::setFlatTail(bool flat) {
      flatTail_ = flat;
      }

bool Track::getFlatTail() const {
      return flatTail_;
      }

void Track::setShowClefEachLine(bool show) {
      showClefEachLine_ = show;
      }

bool Track::getShowClefEachLine() const {
      return showClefEachLine_;
      }

void Track::addDrum(const DrumNode& node) {
      /*DrumNode node;
   node.line_ = line;
   node.headType_ = headType;
   node.pitch_ = pitch;
   node.voice_ = voice;*/
      drumKit_.push_back(node);
      }

QList<Track::DrumNode> Track::getDrumKit() const {
      return drumKit_;
      }

void Track::setPart(int part) {
      part_ = part;
      }

int Track::getPart() const {
      return part_;
      }

void Track::clear(void) {
      number_ = 0;

      name_ = QString();

      patch_ = 0;
      channel_ = 0;
      transpose_ = 0;
      showTranspose_ = false;
      noteShift_ = 0;
      startClef_ = ClefType::Treble;
      transposeClef_ = ClefType::Treble;
      displayPercent_ = 100;
      startKey_ = 0;
      voiceCount_ = 8;

      showName_ = true;
      showBriefName_ = false;
      showKeyEachLine_ = false;
      showLegerLine_ = true;
      showClef_ = true;
      showTimeSignature_ = true;
      showKeySignature_ = true;
      showBarline_ = true;
      showClefEachLine_ = false;

      fillWithRest_ = true;
      flatTail_ = false;

      mute_ = false;
      solo_ = false;

      drumKit_.clear();

      part_ = 0;

      for(int i=0; i<voices_.size(); ++i){
            delete voices_[i];
            }
      voices_.clear();
      }

///////////////////////////////////////////////////////////////////////////////
Page::Page() {
      beginLine_ = 0;
      lineCount_ = 0;

      lineInterval_ = 9;
      staffInterval_ = 7;
      staffInlineInterval_ = 6;

      lineBarCount_ = 4;
      pageLineCount_ = 5;

      leftMargin_ = 0xA8;
      topMargin_ = 0xA8;
      rightMargin_ = 0xA8;
      bottomMargin_ = 0xA8;

      pageWidth_ = 0x0B40;
      pageHeight_ = 0x0E90;
      }

void Page::setBeginLine(int line) {
      beginLine_ = line;
      }

int Page::getBeginLine() const {
      return beginLine_;
      }

void Page::setLineCount(int count) {
      lineCount_ = count;
      }

int Page::getLineCount() const {
      return lineCount_;
      }

void Page::setLineInterval(int interval) {
      lineInterval_ = interval;
      }

int Page::getLineInterval() const {
      return lineInterval_;
      }

void Page::setStaffInterval(int interval) {
      staffInterval_ = interval;
      }

int Page::getStaffInterval() const {
      return staffInterval_;
      }

void Page::setStaffInlineInterval(int interval) {
      staffInlineInterval_ = interval;
      }

int Page::getStaffInlineInterval() const {
      return staffInlineInterval_;
      }

void Page::setLineBarCount(int count) {
      lineBarCount_ = count;
      }

int Page::getLineBarCount() const {
      return lineBarCount_;
      }

void Page::setPageLineCount(int count) {
      pageLineCount_ = count;
      }

int Page::getPageLineCount() const {
      return pageLineCount_;
      }

void Page::setLeftMargin(int margin) {
      leftMargin_ = margin;
      }

int Page::getLeftMargin() const {
      return leftMargin_;
      }

void Page::setTopMargin(int margin) {
      topMargin_ = margin;
      }

int Page::getTopMargin() const {
      return topMargin_;
      }

void Page::setRightMargin(int margin) {
      rightMargin_ = margin;
      }

int Page::getRightMargin() const {
      return rightMargin_;
      }

void Page::setBottomMargin(int margin) {
      bottomMargin_ = margin;
      }

int Page::getBottomMargin() const {
      return bottomMargin_;
      }

void Page::setPageWidth(int width) {
      pageWidth_ = width;
      }

int Page::getPageWidth() const {
      return pageWidth_;
      }

void Page::setPageHeight(int height) {
      pageHeight_ = height;
      }

int Page::getPageHeight() const {
      return pageHeight_;
      }

///////////////////////////////////////////////////////////////////////////////
Line::Line() {
      beginBar_ = 0;
      barCount_ = 0;
      yOffset_ = 0;
      leftXOffset_ = 0;
      rightXOffset_ = 0;
      }

Line::~Line() {
      for(int i=0; i<staffs_.size(); ++i){
            delete staffs_[i];
            }
      staffs_.clear();
      }

void Line::addStaff(Staff* staff) {
      staffs_.push_back(staff);
      }

int Line::getStaffCount() const {
      return staffs_.size();
      }

Staff* Line::getStaff(int idx) const {
      if (idx >= 0 && idx < (int) staffs_.size()) {
            return staffs_[idx];
            }

      return 0;
      }

void Line::setBeginBar(unsigned int bar) {
      beginBar_ = bar;
      }

unsigned int Line::getBeginBar() const {
      return beginBar_;
      }

void Line::setBarCount(unsigned int count) {
      barCount_ = count;
      }

unsigned int Line::getBarCount() const {
      return barCount_;
      }

void Line::setYOffset(int offset) {
      yOffset_ = offset;
      }

int Line::getYOffset() const {
      return yOffset_;
      }

void Line::setLeftXOffset(int offset) {
      leftXOffset_ = offset;
      }

int Line::getLeftXOffset() const {
      return leftXOffset_;
      }

void Line::setRightXOffset(int offset) {
      rightXOffset_ = offset;
      }

int Line::getRightXOffset() const {
      return rightXOffset_;
      }

///////////////////////////////////////////////////////////////////////////////
Staff::Staff() {
      clef_ = ClefType::Treble;
      key_ = 0;
      visible_ = true;
      groupType_ = GroupType::None;
      groupStaffCount_ = 0;
      }

void Staff::setClefType(int clef) {
      clef_ = (ClefType) clef;
      }

ClefType Staff::getClefType() const {
      return clef_;
      }

void Staff::setKeyType(int key) {
      key_ = key;
      }

int Staff::getKeyType() const {
      return key_;
      }

void Staff::setVisible(bool visible) {
      visible_ = visible;
      }

bool Staff::setVisible() const {
      return visible_;
      }

void Staff::setGroupType(GroupType type){
      groupType_ = type;
      }

GroupType Staff::getGroupType() const {
      return groupType_;
      }

void Staff::setGroupStaffCount(int count) {
      groupStaffCount_ = count;
      }

int Staff::getGroupStaffCount() const {
      return groupStaffCount_;
      }

///////////////////////////////////////////////////////////////////////////////
Note::Note() {
      rest_ = false;
      note_ = 60;
      accidental_ = AccidentalType::Normal;
      showAccidental_ = false;
      offVelocity_ = 0x40;
      onVelocity_ = 0x50;
      headType_ = NoteHeadType::Standard;
      tiePos_ = TiePos::None;
      offsetStaff_ = 0;
      show_ = true;
      offsetTick_ = 0;
      }

void Note::setIsRest(bool rest) {
      rest_ = rest;
      }

bool Note::getIsRest() const {
      return rest_;
      }

void Note::setNote(unsigned int note) {
      note_ = note;
      }

unsigned int Note::getNote() const {
      return note_;
      }

void Note::setAccidental(int type) {
      accidental_ = (AccidentalType) type;
      }

AccidentalType Note::getAccidental() const {
      return accidental_;
      }

void Note::setShowAccidental(bool show) {
      showAccidental_ = show;
      }

bool Note::getShowAccidental() const {
      return showAccidental_;
      }

void Note::setOnVelocity(unsigned int velocity) {
      onVelocity_ = velocity;
      }

unsigned int Note::getOnVelocity() const {
      return onVelocity_;
      }

void Note::setOffVelocity(unsigned int velocity) {
      offVelocity_ = velocity;
      }

unsigned int Note::getOffVelocity() const {
      return offVelocity_;
      }

void Note::setHeadType(int type) {
      headType_ = (NoteHeadType) type;
      }

NoteHeadType Note::getHeadType() const {
      return headType_;
      }

void Note::setTiePos(int tiePos) {
      tiePos_ = (TiePos) tiePos;
      }

TiePos Note::getTiePos() const {
      return tiePos_;
      }

void Note::setOffsetStaff(int offset) {
      offsetStaff_ = offset;
      }

int Note::getOffsetStaff() const {
      return offsetStaff_;
      }

void Note::setShow(bool show) {
      show_ = show;
      }

bool Note::getShow() const {
      return show_;
      }

void Note::setOffsetTick(int offset) {
      offsetTick_ = offset;
      }

int Note::getOffsetTick() const {
      return offsetTick_;
      }

///////////////////////////////////////////////////////////////////////////////
Articulation::Articulation() {
      type_ = ArticulationType::Marcato;
      above_ = true;

      changeSoundEffect_ = false;
      changeLength_ = false;
      changeVelocity_ = false;
      changeExtraLength_ = false;

      soundEffect_ = qMakePair(0, 0);
      lengthPercentage_ = 100;
      velocityType_ = VelocityType::Offset;
      velocityValue_ = 0;
      extraLength_ = 0;

      trillNoteLength_ = 60;
      trillRate_ = NoteType::Note_Sixteen;
      accelerateType_ = AccelerateType::None;
      auxiliaryFirst_ = false;
      trillInterval_ = TrillInterval::Chromatic;
      }

void Articulation::setArtType(int type) {
      type_ = (ArticulationType) type;
      }

ArticulationType Articulation::getArtType() const {
      return type_;
      }

void Articulation::setPlacementAbove(bool above) {
      above_ = above;
      }

bool Articulation::getPlacementAbove() const {
      return above_;
      }

bool Articulation::getChangeSoundEffect() const {
      return changeSoundEffect_;
      }

void Articulation::setSoundEffect(int soundFrom, int soundTo) {
      soundEffect_ = qMakePair(soundFrom, soundTo);
      changeSoundEffect_ = true;
      }

QPair<int, int> Articulation::getSoundEffect() const {
      return soundEffect_;
      }

bool Articulation::getChangeLength() const {
      return changeLength_;
      }

void Articulation::setLengthPercentage(int percentage) {
      lengthPercentage_ = percentage;
      changeLength_ = true;
      }

int Articulation::getLengthPercentage() const {
      return lengthPercentage_;
      }

bool Articulation::getChangeVelocity() const {
      return changeVelocity_;
      }

void Articulation::setVelocityType(VelocityType type) {
      velocityType_ = type;
      changeVelocity_ = true;
      }

Articulation::VelocityType Articulation::getVelocityType() const {
      return velocityType_;
      }

void Articulation::setVelocityValue(int value) {
      velocityValue_ = value;
      }

int Articulation::getVelocityValue() const {
      return velocityValue_;
      }

bool Articulation::getChangeExtraLength() const {
      return changeExtraLength_;
      }

void Articulation::setExtraLength(int length) {
      extraLength_ = length;
      changeExtraLength_ = true;
      }

int Articulation::getExtraLength() const {
      return extraLength_;
      }

void Articulation::setTrillNoteLength(int length) {
      trillNoteLength_ = length;
      }

int Articulation::getTrillNoteLength() const {
      return trillNoteLength_;
      }

void Articulation::setTrillRate(NoteType rate) {
      trillRate_ = rate;
      }

NoteType Articulation::getTrillRate() const {
      return trillRate_;
      }

void Articulation::setAccelerateType(int type) {
      accelerateType_ = (AccelerateType) type;
      }

Articulation::AccelerateType Articulation::getAccelerateType() const {
      return accelerateType_;
      }

void Articulation::setAuxiliaryFirst(bool first) {
      auxiliaryFirst_ = first;
      }

bool Articulation::getAuxiliaryFirst() const {
      return auxiliaryFirst_;
      }

void Articulation::setTrillInterval(int interval) {
      trillInterval_ = (TrillInterval) interval;
      }

Articulation::TrillInterval Articulation::getTrillInterval() const {
      return trillInterval_;
      }

bool Articulation::willAffectNotes() const {
      bool affect = false;

      switch (getArtType()) {
            case ArticulationType::Major_Trill:
            case ArticulationType::Minor_Trill:
            case ArticulationType::Trill_Section:
            case ArticulationType::Inverted_Short_Mordent:
            case ArticulationType::Inverted_Long_Mordent:
            case ArticulationType::Short_Mordent:
            case ArticulationType::Turn:

            case ArticulationType::Arpeggio:
            case ArticulationType::Tremolo_Eighth:
            case ArticulationType::Tremolo_Sixteenth:
            case ArticulationType::Tremolo_Thirty_Second:
            case ArticulationType::Tremolo_Sixty_Fourth: {
                  affect = true;
                  break;
                  }
            case ArticulationType::Finger_1:
            case ArticulationType::Finger_2:
            case ArticulationType::Finger_3:
            case ArticulationType::Finger_4:
            case ArticulationType::Finger_5:
            case ArticulationType::Flat_Accidental_For_Trill:
            case ArticulationType::Sharp_Accidental_For_Trill:
            case ArticulationType::Natural_Accidental_For_Trill:
            case ArticulationType::Marcato:
            case ArticulationType::Marcato_Dot:
            case ArticulationType::Heavy_Attack:
            case ArticulationType::SForzando:
            case ArticulationType::SForzando_Dot:
            case ArticulationType::Heavier_Attack:
            case ArticulationType::SForzando_Inverted:
            case ArticulationType::SForzando_Dot_Inverted:
            case ArticulationType::Staccatissimo:
            case ArticulationType::Staccato:
            case ArticulationType::Tenuto:
            case ArticulationType::Up_Bow:
            case ArticulationType::Down_Bow:
            case ArticulationType::Up_Bow_Inverted:
            case ArticulationType::Down_Bow_Inverted:
            case ArticulationType::Natural_Harmonic:
            case ArticulationType::Artificial_Harmonic:
            case ArticulationType::Plus_Sign:
            case ArticulationType::Fermata:
            case ArticulationType::Fermata_Inverted:
            case ArticulationType::Pedal_Down:
            case ArticulationType::Pedal_Up:
            case ArticulationType::Pause:
            case ArticulationType::Grand_Pause:
            case ArticulationType::Toe_Pedal:
            case ArticulationType::Heel_Pedal:
            case ArticulationType::Toe_To_Heel_Pedal:
            case ArticulationType::Heel_To_Toe_Pedal:
            case ArticulationType::Open_String:
            case ArticulationType::Guitar_Lift:
            case ArticulationType::Guitar_Slide_Up:
            case ArticulationType::Guitar_Rip:
            case ArticulationType::Guitar_Fall_Off:
            case ArticulationType::Guitar_Slide_Down:
            case ArticulationType::Guitar_Spill:
            case ArticulationType::Guitar_Flip:
            case ArticulationType::Guitar_Smear:
            case ArticulationType::Guitar_Bend:
            case ArticulationType::Guitar_Doit:
            case ArticulationType::Guitar_Plop:
            case ArticulationType::Guitar_Wow_Wow:
            case ArticulationType::Guitar_Thumb:
            case ArticulationType::Guitar_Index_Finger:
            case ArticulationType::Guitar_Middle_Finger:
            case ArticulationType::Guitar_Ring_Finger:
            case ArticulationType::Guitar_Pinky_Finger:
            case ArticulationType::Guitar_Tap:
            case ArticulationType::Guitar_Hammer:
            case ArticulationType::Guitar_Pluck: {
                  break;
                  }
            default:
                  break;
            }

      return affect;
      }

bool Articulation::isTrill(ArticulationType type) {
      bool isTrill = false;

      switch (type) {
            case ArticulationType::Major_Trill:
            case ArticulationType::Minor_Trill:
            case ArticulationType::Trill_Section: {
                  isTrill = true;
                  break;
                  }
            default:
                  break;
            }

      return isTrill;
      }

Articulation::XmlType Articulation::getXmlType() const {
      XmlType xmlType = XmlType::Unknown;

      switch (type_) {
            case ArticulationType::Major_Trill:
            case ArticulationType::Minor_Trill:
            case ArticulationType::Trill_Section:
            case ArticulationType::Inverted_Short_Mordent:
            case ArticulationType::Inverted_Long_Mordent:
            case ArticulationType::Short_Mordent:
            case ArticulationType::Turn:
                  // case ArticulationType::Flat_Accidental_For_Trill :
                  // case ArticulationType::Sharp_Accidental_For_Trill :
                  // case ArticulationType::Natural_Accidental_For_Trill :
            case ArticulationType::Tremolo_Eighth:
            case ArticulationType::Tremolo_Sixteenth:
            case ArticulationType::Tremolo_Thirty_Second:
            case ArticulationType::Tremolo_Sixty_Fourth: {
                  xmlType = XmlType::Ornament;
                  break;
                  }
            case ArticulationType::Marcato:
            case ArticulationType::Marcato_Dot:
            case ArticulationType::Heavy_Attack:
            case ArticulationType::SForzando:
            case ArticulationType::SForzando_Inverted:
            case ArticulationType::SForzando_Dot:
            case ArticulationType::SForzando_Dot_Inverted:
            case ArticulationType::Heavier_Attack:
            case ArticulationType::Staccatissimo:
            case ArticulationType::Staccato:
            case ArticulationType::Tenuto:
            case ArticulationType::Pause:
            case ArticulationType::Grand_Pause: {
                  xmlType = XmlType::Articulation;
                  break;
                  }
            case ArticulationType::Up_Bow:
            case ArticulationType::Down_Bow:
            case ArticulationType::Up_Bow_Inverted:
            case ArticulationType::Down_Bow_Inverted:
            case ArticulationType::Natural_Harmonic:
            case ArticulationType::Artificial_Harmonic:
            case ArticulationType::Finger_1:
            case ArticulationType::Finger_2:
            case ArticulationType::Finger_3:
            case ArticulationType::Finger_4:
            case ArticulationType::Finger_5:
            case ArticulationType::Plus_Sign: {
                  xmlType = XmlType::Technical;
                  break;
                  }
            case ArticulationType::Arpeggio: {
                  xmlType = XmlType::Arpeggiate;
                  break;
                  }
            case ArticulationType::Fermata:
            case ArticulationType::Fermata_Inverted: {
                  xmlType = XmlType::Fermata;
                  break;
                  }
            case ArticulationType::Pedal_Down:
            case ArticulationType::Pedal_Up: {
                  xmlType = XmlType::Direction;
                  break;
                  }
                  // case ArticulationType::Toe_Pedal :
                  // case ArticulationType::Heel_Pedal :
                  // case ArticulationType::Toe_To_Heel_Pedal :
                  // case ArticulationType::Heel_To_Toe_Pedal :
                  // case ArticulationType::Open_String :
            default:
                  break;
            }

      return xmlType;
      }

///////////////////////////////////////////////////////////////////////////////
NoteContainer::NoteContainer() {
      musicDataType_ = MusicDataType::Note_Container;

      grace_ = false;
      cue_ = false;
      rest_ = false;
      raw_ = false;
      noteType_ = NoteType::Note_Quarter;
      dot_ = 0;
      graceNoteType_ = NoteType::Note_Eight;
      stemUp_ = true;
      showStem_ = true;
      stemLength_ = 7;
      inBeam_ = false;
      tuplet_ = 0;
      space_ = 2;//div by 0
      noteShift_ = 0;
      }

NoteContainer::~NoteContainer(){
      for(int i=0; i<notes_.size(); ++i){
            delete notes_[i];
            }
      for(int i=0; i<articulations_.size(); ++i){
            delete articulations_[i];
            }
      notes_.clear();
      articulations_.clear();
      }

void NoteContainer::setIsGrace(bool grace) {
      grace_ = grace;
      }

bool NoteContainer::getIsGrace() const {
      return grace_;
      }

void NoteContainer::setIsCue(bool cue) {
      cue_ = cue;
      }

bool NoteContainer::getIsCue() const {
      return cue_;
      }

void NoteContainer::setIsRest(bool rest) {
      rest_ = rest;
      }

bool NoteContainer::getIsRest() const {
      return rest_;
      }

void NoteContainer::setIsRaw(bool raw) {
      raw_ = raw;
      }

bool NoteContainer::getIsRaw() const {
      return raw_;
      }

void NoteContainer::setNoteType(NoteType type) {
      noteType_ = NoteType::Note_Quarter;

      switch (type) {
            case NoteType::Note_DoubleWhole:
            case NoteType::Note_Whole:
            case NoteType::Note_Half:
            case NoteType::Note_Quarter:
            case NoteType::Note_Eight:
            case NoteType::Note_Sixteen:
            case NoteType::Note_32:
            case NoteType::Note_64:
            case NoteType::Note_128:
            case NoteType::Note_256: {
                  noteType_ = type;
                  break;
                  }
            default: {
                  break;
                  }
            }
      }

NoteType NoteContainer::getNoteType() const {
      return noteType_;
      }

void NoteContainer::setDot(int dot) {
      dot_ = dot;
      }

int NoteContainer::getDot() const {
      return dot_;
      }

void NoteContainer::setGraceNoteType(NoteType type) {
      graceNoteType_ = type;
      }

NoteType NoteContainer::getGraceNoteType() const {
      return graceNoteType_;
      }

void NoteContainer::setInBeam(bool in) {
      inBeam_ = in;
      }

bool NoteContainer::getInBeam() const {
      return inBeam_;
      }

void NoteContainer::setStemUp(bool up) {
      stemUp_ = up;
      }

bool NoteContainer::getStemUp(void) const {
      return stemUp_;
      }

void NoteContainer::setShowStem(bool show) {
      showStem_ = show;
      }

bool NoteContainer::getShowStem() const {
      return showStem_;
      }

void NoteContainer::setStemLength(int line) {
      stemLength_ = line;
      }

int NoteContainer::getStemLength() const {
      return stemLength_;
      }

void NoteContainer::setTuplet(int tuplet) {
      tuplet_ = tuplet;
      }

int NoteContainer::getTuplet() const {
      return tuplet_;
      }

void NoteContainer::setSpace(int space) {
      space_ = space;
      }

int NoteContainer::getSpace() const {
      return space_;
      }

void NoteContainer::addNoteRest(Note* note) {
      notes_.push_back(note);
      }

QList<Note*> NoteContainer::getNotesRests() const {
      return notes_;
      }

void NoteContainer::addArticulation(Articulation* art) {
      articulations_.push_back(art);
      }

QList<Articulation*> NoteContainer::getArticulations() const {
      return articulations_;
      }

void NoteContainer::setNoteShift(int octave) {
      noteShift_ = octave;
      }

int NoteContainer::getNoteShift() const {
      return noteShift_;
      }

int NoteContainer::getOffsetStaff() const {
      if(getIsRest())
            return 0;

      int staffMove = 0;
      QList<OVE::Note*> notes = getNotesRests();
      for (int i = 0; i < notes.size(); ++i) {
            OVE::Note* notePtr = notes[i];
            staffMove = notePtr->getOffsetStaff();
            }

      return staffMove;
      }

int NoteContainer::getDuration() const {
      int duration = (int) NoteDuration::D_4;

      switch (noteType_) {
            case NoteType::Note_DoubleWhole: {
                  duration = (int) NoteDuration::D_Double_Whole;
                  break;
                  }
            case NoteType::Note_Whole: {
                  duration = (int) NoteDuration::D_Whole;
                  break;
                  }
            case NoteType::Note_Half: {
                  duration = (int) NoteDuration::D_2;
                  break;
                  }
            case NoteType::Note_Quarter: {
                  duration = (int) NoteDuration::D_4;
                  break;
                  }
            case NoteType::Note_Eight: {
                  duration = (int) NoteDuration::D_8;
                  break;
                  }
            case NoteType::Note_Sixteen: {
                  duration = (int) NoteDuration::D_16;
                  break;
                  }
            case NoteType::Note_32: {
                  duration = (int) NoteDuration::D_32;
                  break;
                  }
            case NoteType::Note_64: {
                  duration = (int) NoteDuration::D_64;
                  break;
                  }
            case NoteType::Note_128: {
                  duration = (int) NoteDuration::D_128;
                  break;
                  }
            case NoteType::Note_256: {
                  duration = (int) NoteDuration::D_256;
                  break;
                  }
            default:
                  break;
            }

      int dotLength = duration;

      for (int i = 0; i < dot_; ++i) {
            dotLength /= 2;
            }

      dotLength = duration - dotLength;

      duration += dotLength;

      return duration;
      }

///////////////////////////////////////////////////////////////////////////////
Beam::Beam() {
      musicDataType_ = MusicDataType::Beam;
      grace_ = false;
      }

void Beam::setIsGrace(bool grace) {
      grace_ = grace;
      }

bool Beam::getIsGrace() const {
      return grace_;
      }

void Beam::addLine(const MeasurePos& startMp, const MeasurePos& endMp) {
      lines_.push_back(qMakePair(startMp, endMp));
      }

const QList<QPair<MeasurePos, MeasurePos> > Beam::getLines() const {
      return lines_;
      }

///////////////////////////////////////////////////////////////////////////////
Tie::Tie() {
      musicDataType_ = MusicDataType::Tie;

      showOnTop_ = true;
      note_ = 72;
      height_ = 24;
      }

void Tie::setShowOnTop(bool top) {
      showOnTop_ = top;
      }

bool Tie::getShowOnTop() const {
      return showOnTop_;
      }

void Tie::setNote(int note) {
      note_ = note;
      }

int Tie::getNote() const {
      return note_;
      }

void Tie::setHeight(int height) {
      height_ = height;
      }

int Tie::getHeight() const {
      return height_;
      }

///////////////////////////////////////////////////////////////////////////////
Glissando::Glissando() {
      musicDataType_ = MusicDataType::Glissando;

      straight_ = true;
      text_ = "gliss.";
      lineThick_ = 8;
      }

void Glissando::setStraightWavy(bool straight) {
      straight_ = straight;
      }

bool Glissando::getStraightWavy() const {
      return straight_;
      }

void Glissando::setText(const QString& text) {
      text_ = text;
      }

QString Glissando::getText() const {
      return text_;
      }

void Glissando::setLineThick(int thick) {
      lineThick_ = thick;
      }

int Glissando::getLineThick() const {
      return lineThick_;
      }

///////////////////////////////////////////////////////////////////////////////
Decorator::Decorator() :
      decoratorType_(Type::Articulation),
      artType_(ArticulationType::Marcato) {
      musicDataType_ = MusicDataType::Decorator;
      }

void Decorator::setDecoratorType(Type type) {
      decoratorType_ = type;
      }

Decorator::Type Decorator::getDecoratorType() const {
      return decoratorType_;
      }

void Decorator::setArticulationType(ArticulationType type) {
      artType_ = type;
      }

ArticulationType Decorator::getArticulationType() const {
      return artType_;
      }

///////////////////////////////////////////////////////////////////////////////
MeasureRepeat::MeasureRepeat() {
      musicDataType_ = MusicDataType::Measure_Repeat;
      singleRepeat_ = true;
      }

void MeasureRepeat::setSingleRepeat(bool single) {
      singleRepeat_ = single;

      start()->setMeasure(0);
      start()->setOffset(0);
      stop()->setMeasure(single ? 1 : 2);
      stop()->setOffset(0);
      }

bool MeasureRepeat::getSingleRepeat() const {
      return singleRepeat_;
      }

///////////////////////////////////////////////////////////////////////////////
Tuplet::Tuplet() :
      tuplet_(3), space_(2), height_(0), noteType_(NoteType::Note_Quarter){
      musicDataType_ = MusicDataType::Tuplet;
      mark_ = new OffsetElement();
      }

Tuplet::~Tuplet(){
      delete mark_;
      }

void Tuplet::setTuplet(int tuplet) {
      tuplet_ = tuplet;
      }

int Tuplet::getTuplet() const {
      return tuplet_;
      }

void Tuplet::setSpace(int space) {
      space_ = space;
      }

int Tuplet::getSpace() const {
      return space_;
      }

OffsetElement* Tuplet::getMarkHandle() const {
      return mark_;
      }

void Tuplet::setHeight(int height) {
      height_ = height;
      }

int Tuplet::getHeight() const {
      return height_;
      }

void Tuplet::setNoteType(NoteType type) {
      noteType_ = type;
      }

NoteType Tuplet::getNoteType() const {
      return noteType_;
      }

///////////////////////////////////////////////////////////////////////////////
Harmony::Harmony() {
      musicDataType_ = MusicDataType::Harmony;

      harmonyType_ = "";
      root_ = 0;
      bass_ = -1; //0xff
      alterRoot_ = 0;
      alterBass_ = 0;
      bassOnBottom_ = false;
      angle_ = 0;
      }

void Harmony::setHarmonyType(QString type) {
      harmonyType_ = type;
      }

QString Harmony::getHarmonyType() const {
      return harmonyType_;
      }

void Harmony::setRoot(int root) {
      root_ = root;
      }

int Harmony::getRoot() const {
      return root_;
      }

void Harmony::setAlterRoot(int val) {
      alterRoot_ = val;
      }

int Harmony::getAlterRoot() const {
      return alterRoot_;
      }

void Harmony::setBass(int bass) {
      bass_ = bass;
      }

int Harmony::getBass() const {
      return bass_;
      }

void Harmony::setAlterBass(int val) {
      alterBass_ = val;
      }

int Harmony::getAlterBass() const {
      return alterBass_;
      }

void Harmony::setBassOnBottom(bool on) {
      bassOnBottom_ = on;
      }

bool Harmony::getBassOnBottom() const {
      return bassOnBottom_;
      }

void Harmony::setAngle(int angle) {
      angle_ = angle;
      }

int Harmony::getAngle() const {
      return angle_;
      }

///////////////////////////////////////////////////////////////////////////////
Clef::Clef() {
      musicDataType_ = MusicDataType::Clef;

      clefType_ = ClefType::Treble;
      }

void Clef::setClefType(int type) {
      clefType_ = (ClefType) type;
      }

ClefType Clef::getClefType() const {
      return clefType_;
      }

///////////////////////////////////////////////////////////////////////////////
Lyric::Lyric() {
      musicDataType_ = MusicDataType::Lyric;

      lyric_ = QString();
      verse_ = 0;
      }

void Lyric::setLyric(const QString& lyricText) {
      lyric_ = lyricText;
      }

QString Lyric::getLyric() const {
      return lyric_;
      }

void Lyric::setVerse(int verse) {
      verse_ = verse;
      }

int Lyric::getVerse() const {
      return verse_;
      }

///////////////////////////////////////////////////////////////////////////////
Slur::Slur() {
      musicDataType_ = MusicDataType::Slur;

      containerCount_ = 1;
      showOnTop_ = true;
      noteTimePercent_ = 100;

      handle_2_ = new OffsetElement();
      handle_3_ = new OffsetElement();
      }

Slur::~Slur() {
      delete handle_2_;
      delete handle_3_;
      }

void Slur::setContainerCount(int count) {
      containerCount_ = count;
      }

int Slur::getContainerCount() const {
      return containerCount_;
      }

void Slur::setShowOnTop(bool top) {
      showOnTop_ = top;
      }

bool Slur::getShowOnTop() const {
      return showOnTop_;
      }

OffsetElement* Slur::getHandle2() const {
      return handle_2_;
      }

OffsetElement* Slur::getHandle3() const {
      return handle_3_;
      }

void Slur::setNoteTimePercent(int percent) {
      noteTimePercent_ = percent;
      }

int Slur::getNoteTimePercent() const {
      return noteTimePercent_;
      }

///////////////////////////////////////////////////////////////////////////////
Dynamics::Dynamics() {
      musicDataType_ = MusicDataType::Dynamics;

      dynamicsType_ = DynamicsType::PPPP;
      playback_ = true;
      velocity_ = 30;
      }

void Dynamics::setDynamicsType(int type) {
      dynamicsType_ = DynamicsType(type);
      }

DynamicsType Dynamics::getDynamicsType() const {
      return dynamicsType_;
      }

void Dynamics::setIsPlayback(bool play) {
      playback_ = play;
      }

bool Dynamics::getIsPlayback() const {
      return playback_;
      }

void Dynamics::setVelocity(int vel) {
      velocity_ = vel;
      }

int Dynamics::getVelocity() const {
      return velocity_;
      }

///////////////////////////////////////////////////////////////////////////////
WedgeEndPoint::WedgeEndPoint() {
      musicDataType_ = MusicDataType::Wedge_EndPoint;

      wedgeType_ = WedgeType::Cres;
      height_ = 24;
      wedgeStart_ = true;
      }

void WedgeEndPoint::setWedgeType(WedgeType type) {
      wedgeType_ = type;
      }

WedgeType WedgeEndPoint::getWedgeType() const {
      return wedgeType_;
      }

void WedgeEndPoint::setHeight(int height) {
      height_ = height;
      }

int WedgeEndPoint::getHeight() const {
      return height_;
      }

void WedgeEndPoint::setWedgeStart(bool wedgeStart) {
      wedgeStart_ = wedgeStart;
      }

bool WedgeEndPoint::getWedgeStart() const {
      return wedgeStart_;
      }

///////////////////////////////////////////////////////////////////////////////
Wedge::Wedge() {
      musicDataType_ = MusicDataType::Wedge;

      wedgeType_ = WedgeType::Cres;
      height_ = 24;
      }

void Wedge::setWedgeType(WedgeType type) {
      wedgeType_ = type;
      }

WedgeType Wedge::getWedgeType() const {
      return wedgeType_;
      }

void Wedge::setHeight(int height) {
      height_ = height;
      }

int Wedge::getHeight() const {
      return height_;
      }

///////////////////////////////////////////////////////////////////////////////
Pedal::Pedal() {
      musicDataType_ = MusicDataType::Pedal;

      half_ = false;
      playback_ = false;
      playOffset_ = 0;

      pedalHandle_ = new OffsetElement();
      }

Pedal::~Pedal() {
      delete pedalHandle_;
      }

void Pedal::setHalf(bool half) {
      half_ = half;
      }

bool Pedal::getHalf() const {
      return half_;
      }

OffsetElement* Pedal::getPedalHandle() const {
      return pedalHandle_;
      }

void Pedal::setIsPlayback(bool playback) {
      playback_ = playback;
      }

bool Pedal::getIsPlayback() const {
      return playback_;
      }

void Pedal::setPlayOffset(int offset) {
      playOffset_ = offset;
      }

int Pedal::getPlayOffset() const {
      return playOffset_;
      }

///////////////////////////////////////////////////////////////////////////////
KuoHao::KuoHao() {
      musicDataType_ = MusicDataType::KuoHao;

      kuohaoType_ = KuoHaoType::Parentheses;
      height_ = 0;
      }

void KuoHao::setHeight(int height) {
      height_ = height;
      }

int KuoHao::getHeight() const {
      return height_;
      }

void KuoHao::setKuohaoType(int type) {
      kuohaoType_ = (KuoHaoType) type;
      }

KuoHaoType KuoHao::getKuohaoType() const {
      return kuohaoType_;
      }

///////////////////////////////////////////////////////////////////////////////
Expressions::Expressions() {
      musicDataType_ = MusicDataType::Expressions;

      text_ = QString();
      }

void Expressions::setText(const QString& str) {
      text_ = str;
      }

QString Expressions::getText() const {
      return text_;
      }

///////////////////////////////////////////////////////////////////////////////
HarpPedal::HarpPedal() :
      showType_(0),
      showCharFlag_(0) {
      musicDataType_ = MusicDataType::Harp_Pedal;
      }

void HarpPedal::setShowType(int type) {
      showType_ = type;
      }

int HarpPedal::getShowType() const {
      return showType_;
      }

void HarpPedal::setShowCharFlag(int flag) {
      showCharFlag_ = flag;
      }

int HarpPedal::getShowCharFlag() const {
      return showCharFlag_;
      }

///////////////////////////////////////////////////////////////////////////////
OctaveShift::OctaveShift() :
      octaveShiftType_(OctaveShiftType::OS_8),
      octaveShiftPosition_(OctaveShiftPosition::Start),
      endTick_(0) {
      musicDataType_ = MusicDataType::OctaveShift;
      }

void OctaveShift::setOctaveShiftType(OctaveShiftType type) {
      octaveShiftType_ = type;
      }

OctaveShiftType OctaveShift::getOctaveShiftType() const {
      return octaveShiftType_;
      }

int OctaveShift::getNoteShift() const {
      int shift = 12;

      switch (getOctaveShiftType()) {
            case OctaveShiftType::OS_8: {
                  shift = 12;
                  break;
                  }
            case OctaveShiftType::OS_Minus_8: {
                  shift = -12;
                  break;
                  }
            case OctaveShiftType::OS_15: {
                  shift = 24;
                  break;
                  }
            case OctaveShiftType::OS_Minus_15: {
                  shift = -24;
                  break;
                  }
            default:
                  break;
            }

      return shift;
      }

void OctaveShift::setEndTick(int tick) {
      endTick_ = tick;
      }

int OctaveShift::getEndTick() const {
      return endTick_;
      }

void OctaveShift::setOctaveShiftPosition(OctaveShiftPosition position) {
      octaveShiftPosition_ = position;
      }

OctaveShiftPosition OctaveShift::getOctaveShiftPosition() const {
      return octaveShiftPosition_;
      }

///////////////////////////////////////////////////////////////////////////////
OctaveShiftEndPoint::OctaveShiftEndPoint() {
      musicDataType_ = MusicDataType::OctaveShift_EndPoint;

      octaveShiftType_ = OctaveShiftType::OS_8;
      octaveShiftPosition_ = OctaveShiftPosition::Start;
      endTick_ = 0;
      }

void OctaveShiftEndPoint::setOctaveShiftType(OctaveShiftType type) {
      octaveShiftType_ = type;
      }

OctaveShiftType OctaveShiftEndPoint::getOctaveShiftType() const {
      return octaveShiftType_;
      }

void OctaveShiftEndPoint::setOctaveShiftPosition(OctaveShiftPosition position) {
      octaveShiftPosition_ = position;
      }

OctaveShiftPosition OctaveShiftEndPoint::getOctaveShiftPosition() const {
      return octaveShiftPosition_;
      }

void OctaveShiftEndPoint::setEndTick(int tick) {
      endTick_ = tick;
      }

int OctaveShiftEndPoint::getEndTick() const {
      return endTick_;
      }

///////////////////////////////////////////////////////////////////////////////
MultiMeasureRest::MultiMeasureRest() {
      musicDataType_ = MusicDataType::Multi_Measure_Rest;
      measureCount_ = 0;
      }

void MultiMeasureRest::setMeasureCount(int count) {
      measureCount_ = count;
      }

int MultiMeasureRest::getMeasureCount() const {
      return measureCount_;
      }

///////////////////////////////////////////////////////////////////////////////
Tempo::Tempo() {
      musicDataType_ = MusicDataType::Tempo;

      leftNoteType_ = 3;
      showMark_ = false;
      showText_ = false;
      showParenthesis_ = false;
      typeTempo_ = 96;
      leftText_ = QString();
      rightText_ = QString();
      swingEighth_ = false;
      rightNoteType_ = 3;
      leftNoteDot_ = false;
      rightNoteDot_ = false;
      rightSideType_ = 0;
      }

void Tempo::setLeftNoteType(int type) {
      leftNoteType_ = type;
      }

NoteType Tempo::getLeftNoteType() const {
      return (NoteType) leftNoteType_;
      }

void Tempo::setShowMark(bool show) {
      showMark_ = show;
      }

bool Tempo::getShowMark() const {
      return showMark_;
      }

void Tempo::setShowBeforeText(bool show) {
      showText_ = show;
      }

bool Tempo::getShowBeforeText() const {
      return showText_;
      }

void Tempo::setShowParenthesis(bool show) {
      showParenthesis_ = show;
      }

bool Tempo::getShowParenthesis() const {
      return showParenthesis_;
      }

void Tempo::setTypeTempo(double tempo) {
      typeTempo_ = tempo;
      }

double Tempo::getTypeTempo() const {
      return typeTempo_;
      }

double Tempo::getQuarterTempo() const {
      double factor = pow(2.0, int(NoteType::Note_Quarter) - int(getLeftNoteType()));
      if (getLeftNoteDot())
            factor *= 3.0/2.0;
      double tempo = getTypeTempo() * factor;

      return tempo;
      }

void Tempo::setLeftText(const QString& str) {
      leftText_ = str;
      }

QString Tempo::getLeftText() const {
      return leftText_;
      }

void Tempo::setRightText(const QString& str) {
      rightText_ = str;
      }

QString Tempo::getRightText() const {
      return rightText_;
      }

void Tempo::setSwingEighth(bool swing) {
      swingEighth_ = swing;
      }

bool Tempo::getSwingEighth() const {
      return swingEighth_;
      }

void Tempo::setRightNoteType(int type) {
      rightNoteType_ = type;
      }

NoteType Tempo::getRightNoteType() const {
      return (NoteType) rightNoteType_;
      }

void Tempo::setLeftNoteDot(bool showDot) {
      leftNoteDot_ = showDot;
      }

bool Tempo::getLeftNoteDot() const {
      return leftNoteDot_;
      }

void Tempo::setRightNoteDot(bool showDot) {
      rightNoteDot_ = showDot;
      }

bool Tempo::getRightNoteDot() const {
      return rightNoteDot_;
      }

void Tempo::setRightSideType(int type) {
      rightSideType_ = type;
      }

int Tempo::getRightSideType() const {
      return rightSideType_;
      }

///////////////////////////////////////////////////////////////////////////////
Text::Text() {
      musicDataType_ = MusicDataType::Text;

      textType_ = Type::Rehearsal;
      horiMargin_ = 8;
      vertMargin_ = 8;
      lineThick_ = 4;
      text_ = QString();
      width_ = 0;
      height_ = 0;
      }

void Text::setTextType(Type type) {
      textType_ = type;
      }

Text::Type Text::getTextType() const {
      return textType_;
      }

void Text::setHorizontalMargin(int margin) {
      horiMargin_ = margin;
      }

int Text::getHorizontalMargin() const {
      return horiMargin_;
      }

void Text::setVerticalMargin(int margin) {
      vertMargin_ = margin;
      }

int Text::getVerticalMargin() const {
      return vertMargin_;
      }

void Text::setLineThick(int thick) {
      lineThick_ = thick;
      }

int Text::getLineThick() const {
      return lineThick_;
      }

void Text::setText(const QString& text) {
      text_ = text;
      }

QString Text::getText() const {
      return text_;
      }

void Text::setWidth(int width) {
      width_ = width;
      }

int Text::getWidth() const {
      return width_;
      }

void Text::setHeight(int height) {
      height_ = height;
      }

int Text::getHeight() const {
      return height_;
      }

///////////////////////////////////////////////////////////////////////////////
TimeSignature::TimeSignature() {
      numerator_ = 4;
      denominator_ = 4;
      isSymbol_ = false;
      beatLength_ = 480;
      barLength_ = 1920;
      barLengthUnits_ = 0x400;
      replaceFont_ = false;
      showBeatGroup_ = false;

      groupNumerator1_ = 0;
      groupNumerator2_ = 0;
      groupNumerator3_ = 0;
      groupDenominator1_ = 4;
      groupDenominator2_ = 4;
      groupDenominator3_ = 4;

      beamGroup1_ = 4;
      beamGroup2_ = 0;
      beamGroup3_ = 0;
      beamGroup4_ = 0;

      beamCount16th_ = 4;
      beamCount32th_ = 1;
      }

void TimeSignature::setNumerator(int numerator) {
      numerator_ = numerator;
      }

int TimeSignature::getNumerator() const {
      return numerator_;
      }

void TimeSignature::setDenominator(int denominator) {
      denominator_ = denominator;
      }

int TimeSignature::getDenominator() const {
      return denominator_;
      }

void TimeSignature::setIsSymbol(bool symbol) {
      isSymbol_ = symbol;
      }

bool TimeSignature::getIsSymbol() const {
      if (numerator_ == 2 && denominator_ == 2) {
            return true;
            }

      return isSymbol_;
      }

void TimeSignature::setBeatLength(int length) {
      beatLength_ = length;
      }

int TimeSignature::getBeatLength() const {
      return beatLength_;
      }

void TimeSignature::setBarLength(int length) {
      barLength_ = length;
      }

int TimeSignature::getBarLength() const {
      return barLength_;
      }

void TimeSignature::addBeat(int startUnit, int lengthUnit, int startTick) {
      BeatNode node;
      node.startUnit_ = startUnit;
      node.lengthUnit_ = lengthUnit;
      node.startTick_ = startTick;
      beats_.push_back(node);
      }

void TimeSignature::endAddBeat() {
      int i;
      barLengthUnits_ = 0;

      for (i = 0; i < beats_.size(); ++i) {
            barLengthUnits_ += beats_[i].lengthUnit_;
            }
      }

int TimeSignature::getUnits() const {
      return barLengthUnits_;
      }

void TimeSignature::setReplaceFont(bool replace) {
      replaceFont_ = replace;
      }

bool TimeSignature::getReplaceFont() const {
      return replaceFont_;
      }

void TimeSignature::setShowBeatGroup(bool show) {
      showBeatGroup_ = show;
      }

bool TimeSignature::getShowBeatGroup() const {
      return showBeatGroup_;
      }

void TimeSignature::setGroupNumerator1(int numerator) {
      groupNumerator1_ = numerator;
      }

void TimeSignature::setGroupNumerator2(int numerator) {
      groupNumerator2_ = numerator;
      }

void TimeSignature::setGroupNumerator3(int numerator) {
      groupNumerator3_ = numerator;
      }

void TimeSignature::setGroupDenominator1(int denominator) {
      groupDenominator1_ = denominator;
      }

void TimeSignature::setGroupDenominator2(int denominator) {
      groupDenominator2_ = denominator;
      }

void TimeSignature::setGroupDenominator3(int denominator) {
      groupDenominator3_ = denominator;
      }

void TimeSignature::setBeamGroup1(int count) {
      beamGroup1_ = count;
      }

void TimeSignature::setBeamGroup2(int count) {
      beamGroup2_ = count;
      }

void TimeSignature::setBeamGroup3(int count) {
      beamGroup3_ = count;
      }

void TimeSignature::setBeamGroup4(int count) {
      beamGroup4_ = count;
      }

void TimeSignature::set16thBeamCount(int count) {
      beamCount16th_ = count;
      }

void TimeSignature::set32thBeamCount(int count) {
      beamCount32th_ = count;
      }

///////////////////////////////////////////////////////////////////////////////
Key::Key() {
      key_ = 0;
      set_ = false;
      previousKey_ = 0;
      symbolCount_ = 0;
      }

void Key::setKey(int key) {
      key_ = key;
      set_ = true;
      }

int Key::getKey() const {
      return key_;
      }

bool Key::getSetKey() const {
      return set_;
      }

void Key::setPreviousKey(int key) {
      previousKey_ = key;
      }

int Key::getPreviousKey() const {
      return previousKey_;
      }

void Key::setSymbolCount(int count) {
      symbolCount_ = count;
      }

int Key::getSymbolCount() const {
      return symbolCount_;
      }

///////////////////////////////////////////////////////////////////////////////
RepeatSymbol::RepeatSymbol() :
      text_("#1"), repeatType_(RepeatType::Segno) {
      musicDataType_ = MusicDataType::Repeat;
      }

void RepeatSymbol::setText(const QString& text) {
      text_ = text;
      }

QString RepeatSymbol::getText() const {
      return text_;
      }

void RepeatSymbol::setRepeatType(int repeatType) {
      repeatType_ = (RepeatType) repeatType;
      }

RepeatType RepeatSymbol::getRepeatType() const {
      return repeatType_;
      }

///////////////////////////////////////////////////////////////////////////////
NumericEnding::NumericEnding() {
      musicDataType_ = MusicDataType::Numeric_Ending;

      height_ = 0;
      text_ = QString();
      numericHandle_ = new OffsetElement();
      }

NumericEnding::~NumericEnding() {
      delete numericHandle_;
      }

OffsetElement* NumericEnding::getNumericHandle() const {
      return numericHandle_;
      }

void NumericEnding::setHeight(int height) {
      height_ = height;
      }

int NumericEnding::getHeight() const {
      return height_;
      }

void NumericEnding::setText(const QString& text) {
      text_ = text;
      }

QString NumericEnding::getText() const {
      return text_;
      }

QList<int> NumericEnding::getNumbers() const {
      int i;
      QStringList strs = text_.split(",", QString::SkipEmptyParts);
      QList<int> endings;

      for (i = 0; i < strs.size(); ++i) {
            bool ok;
            int num = strs[i].toInt(&ok);
            endings.push_back(num);
            }

      return endings;
      }

int NumericEnding::getJumpCount() const {
      QList<int> numbers = getNumbers();
      int count = 0;

      for (int i = 0; i < numbers.size(); ++i) {
            if ((int)i + 1 != numbers[i]) {
                  break;
                  }

            count = i + 1;
            }

      return count;
      }

///////////////////////////////////////////////////////////////////////////////
BarNumber::BarNumber() {
      index_ = 0;
      showOnParagraphStart_ = false;
      align_ = 0;
      showFlag_ = 1; // staff
      barRange_ = 1; // can't be 0
      prefix_ = QString();
      }

void BarNumber::setIndex(int index) {
      index_ = index;
      }

int BarNumber::getIndex() const {
      return index_;
      }

void BarNumber::setShowOnParagraphStart(bool show) {
      showOnParagraphStart_ = show;
      }

bool BarNumber::getShowOnParagraphStart() const {
      return showOnParagraphStart_;
      }

void BarNumber::setAlign(int align)// 0:left, 1:center, 2:right
      {
      align_ = align;
      }

int BarNumber::getAlign() const {
      return align_;
      }

void BarNumber::setShowFlag(int flag) {
      showFlag_ = flag;
      }

int BarNumber::getShowFlag() const {
      return showFlag_;
      }

void BarNumber::setShowEveryBarCount(int count) {
      barRange_ = count;
      }

int BarNumber::getShowEveryBarCount() const {
      return barRange_;
      }

void BarNumber::setPrefix(const QString& str) {
      prefix_ = str;
      }

QString BarNumber::getPrefix() const {
      return prefix_;
      }

///////////////////////////////////////////////////////////////////////////////
MidiController::MidiController() {
      midiType_ = MidiType::Controller;
      controller_ = 64; // pedal
      value_ = 0;
      }

void MidiController::setController(int number) {
      controller_ = number;
      }

int MidiController::getController() const {
      return controller_;
      }

void MidiController::setValue(int value) {
      value_ = value;
      }

int MidiController::getValue() const {
      return value_;
      }

///////////////////////////////////////////////////////////////////////////////
MidiProgramChange::MidiProgramChange() {
      midiType_ = MidiType::Program_Change;
      patch_ = 0; // grand piano
      }

void MidiProgramChange::setPatch(int patch) {
      patch_ = patch;
      }

int MidiProgramChange::getPatch() const {
      return patch_;
      }

///////////////////////////////////////////////////////////////////////////////
MidiChannelPressure::MidiChannelPressure() :
      pressure_(0) {
      midiType_ = MidiType::Channel_Pressure;
      }

void MidiChannelPressure::setPressure(int pressure) {
      pressure_ = pressure;
      }

int MidiChannelPressure::getPressure() const {
      return pressure_;
      }

///////////////////////////////////////////////////////////////////////////////
MidiPitchWheel::MidiPitchWheel() {
      midiType_ = MidiType::Pitch_Wheel;
      value_ = 0;
      }

void MidiPitchWheel::setValue(int value) {
      value_ = value;
      }

int MidiPitchWheel::getValue() const {
      return value_;
      }

///////////////////////////////////////////////////////////////////////////////
Measure::Measure(int index) {
      barNumber_ = new BarNumber();
      barNumber_->setIndex(index);
      time_ = new TimeSignature();

      clear();
      }

Measure::~Measure(){
      clear();

      delete barNumber_;
      delete time_;
      }

BarNumber* Measure::getBarNumber() const {
      return barNumber_;
      }

TimeSignature* Measure::getTime() const {
      return time_;
      }

void Measure::setLeftBarline(int barline) {
      leftBarline_ = (BarLineType) barline;
      }

BarLineType Measure::getLeftBarline() const {
      return leftBarline_;
      }

void Measure::setRightBarline(int barline) {
      rightBarline_ = (BarLineType) barline;
      }

BarLineType Measure::getRightBarline() const {
      return rightBarline_;
      }

void Measure::setBackwardRepeatCount(int repeatCount) {
      repeatCount_ = repeatCount;
      }

int Measure::getBackwardRepeatCount() const {
      return repeatCount_;
      }

void Measure::setTypeTempo(double tempo) {
      typeTempo_ = tempo;
      }

double Measure::getTypeTempo() const {
      return typeTempo_;
      }

void Measure::setIsPickup(bool pickup) {
      pickup_ = pickup;
      }

bool Measure::getIsPickup() const {
      return pickup_;
      }

void Measure::setIsMultiMeasureRest(bool rest) {
      multiMeasureRest_ = rest;
      }

bool Measure::getIsMultiMeasureRest() const {
      return multiMeasureRest_;
      }

void Measure::setMultiMeasureRestCount(int count) {
      multiMeasureRestCount_ = count;
      }

int Measure::getMultiMeasureRestCount() const {
      return multiMeasureRestCount_;
      }

void Measure::clear() {
      leftBarline_ = BarLineType::Default;
      rightBarline_ = BarLineType::Default;
      repeatCount_ = 1;
      typeTempo_ = 96.00;
      setLength(0x780); //time = 4/4
      pickup_ = false;
      multiMeasureRest_ = false;
      multiMeasureRestCount_ = 0;
      }

///////////////////////////////////////////////////////////////////////////////
MeasureData::MeasureData() {
      key_ = new Key();
      clef_ = new Clef();
      }

MeasureData::~MeasureData(){
      int i;
      for(i=0; i<musicDatas_.size(); ++i){
            delete musicDatas_[i];
            }
      musicDatas_.clear();

      // noteContainers_ also in musicDatas_, no need to destroy
      noteContainers_.clear();

      // only delete at element start
      for(i=0; i<crossMeasureElements_.size(); ++i){
            if(crossMeasureElements_[i].second){
                  delete crossMeasureElements_[i].first;
                  }
            }
      crossMeasureElements_.clear();

      for(i=0; i<midiDatas_.size(); ++i){
            delete midiDatas_[i];
            }
      midiDatas_.clear();

      delete key_;
      delete clef_;
      }

Key* MeasureData::getKey() const {
      return key_;
      }

Clef* MeasureData::getClef() const {
      return clef_;
      }

void MeasureData::addNoteContainer(NoteContainer* ptr) {
      noteContainers_.push_back(ptr);
      }

QList<NoteContainer*> MeasureData::getNoteContainers() const {
      return noteContainers_;
      }

void MeasureData::addMusicData(MusicData* ptr) {
      musicDatas_.push_back(ptr);
      }

QList<MusicData*> MeasureData::getMusicDatas(MusicDataType type) {
      int i;
      QList<MusicData*> notations;

      for (i = 0; i < musicDatas_.size(); ++i) {
            if (type == MusicDataType::None || musicDatas_[i]->getMusicDataType() == type) {
                  notations.push_back(musicDatas_[i]);
                  }
            }

      return notations;
      }

void MeasureData::addCrossMeasureElement(MusicData* ptr, bool start) {
      crossMeasureElements_.push_back(qMakePair(ptr, start));
      }

QList<MusicData*> MeasureData::getCrossMeasureElements(
            MusicDataType type, PairType pairType)
      {
      int i;
      QList<MusicData*> pairs;

      for (i = 0; i < crossMeasureElements_.size(); ++i) {
            if ((type == MusicDataType::None || crossMeasureElements_[i].first->getMusicDataType() == type)
                && (pairType == PairType::All || ((crossMeasureElements_[i].second && pairType == PairType::Start)
                                                 || (!crossMeasureElements_[i].second && pairType == PairType::Stop)))) {
                  pairs.push_back(crossMeasureElements_[i].first);
                  }
            }

      return pairs;
      }

void MeasureData::addMidiData(MidiData* ptr) {
      midiDatas_.push_back(ptr);
      }

QList<MidiData*> MeasureData::getMidiDatas(MidiType type) {
      int i;
      QList<MidiData*> datas;

      for (i = 0; i < midiDatas_.size(); ++i) {
            if (type == MidiType::None || midiDatas_[i]->getMidiType() == type) {
                  datas.push_back(midiDatas_[i]);
                  }
            }

      return datas;
      }
      


}


