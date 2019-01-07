#include "ChunkParse.h"

namespace OVE {

StreamHandle::StreamHandle() :
      size_(0), curPos_(0), point_(NULL) {
      }

StreamHandle::StreamHandle(unsigned char* p, int size) :
      size_(size), curPos_(0), point_(p) {
      }

StreamHandle::~StreamHandle() {
      point_ = NULL;
      }

bool StreamHandle::read(char* buff, int size) {
      if (point_ != NULL && curPos_ + size <= size_) {
            memcpy(buff, point_ + curPos_, size);
            curPos_ += size;

            return true;
            }

      return false;
      }

bool StreamHandle::write(char* /*buff*/, int /*size*/) {
      return true;
      }


// ChunkParse.cpp
unsigned int getHighNibble(unsigned int byte) {
      return byte / 16;
      }

unsigned int getLowNibble(unsigned int byte) {
      return byte % 16;
      }

int oveKeyToKey(int oveKey) {
      int key = 0;

      if( oveKey == 0 ) {
            key = 0;
            }
      else if( oveKey > 7 ) {
            key = oveKey - 7;
            }
      else if( oveKey <= 7 ) {
            key = oveKey * (-1);
            }

      return key;
      }


///////////////////////////////////////////////////////////////////////////////
BasicParse::BasicParse(OveSong* ove) :
      ove_(ove), handle_(NULL), notify_(NULL) {
      }

BasicParse::BasicParse() :
      ove_(NULL), handle_(NULL), notify_(NULL) {
      }

BasicParse::~BasicParse() {
      ove_ = NULL;
      handle_ = NULL;
      notify_ = NULL;
      }

void BasicParse::setNotify(IOveNotify* notify) {
      notify_ = notify;
      }

bool BasicParse::parse() {
      return false;
      }

bool BasicParse::readBuffer(Block& placeHolder, int size) {
      if (handle_ == NULL) {
            return false;
            }
      if (placeHolder.size() != size) {
            placeHolder.resize(size);
            }

      if (size > 0) {
            return handle_->read((char*) placeHolder.data(), placeHolder.size());
            }

      return true;
      }

bool BasicParse::jump(int offset) {
      if (handle_ == NULL || offset < 0) {
            return false;
            }

      if (offset > 0) {
            Block placeHolder(offset);
            return handle_->read((char*) placeHolder.data(), placeHolder.size());
            }

      return true;
      }

void BasicParse::messageOut(const QString& str) {
      if (notify_ != NULL) {
            notify_->loadInfo(str);
            }
      }

///////////////////////////////////////////////////////////////////////////////
OvscParse::OvscParse(OveSong* ove) :
      BasicParse(ove), chunk_(NULL) {
      }

OvscParse::~OvscParse() {
      chunk_ = NULL;
      }

void OvscParse::setOvsc(SizeChunk* chunk) {
      chunk_ = chunk;
      }

bool OvscParse::parse() {
      Block* dataBlock = chunk_->getDataBlock();
      unsigned int blockSize = chunk_->getSizeBlock()->toSize();
      StreamHandle handle(dataBlock->data(), blockSize);
      Block placeHolder;

      handle_ = &handle;

      // version
      if (!readBuffer(placeHolder, 1)) { return false; }
      bool version4 = placeHolder.toUnsignedInt() == 4;
      ove_->setIsVersion4(version4);

      QString str = QString("This file is created by Overture ") + (version4 ? "4" : "3") + "\n";
      messageOut(str);

      if( !jump(6) ) { return false; }

      // show page margin
      if (!readBuffer(placeHolder, 1)) { return false; }
      ove_->setShowPageMargin(placeHolder.toBoolean());

      if( !jump(1) ) { return false; }

      // transpose track
      if (!readBuffer(placeHolder, 1)) { return false; }
      ove_->setShowTransposeTrack(placeHolder.toBoolean());

      // play repeat
      if (!readBuffer(placeHolder, 1)) { return false; }
      ove_->setPlayRepeat(placeHolder.toBoolean());

      // play style
      if (!readBuffer(placeHolder, 1)) { return false; }
      OveSong::PlayStyle style = OveSong::PlayStyle::Record;
      if(placeHolder.toUnsignedInt() == 1){
            style = OveSong::PlayStyle::Swing;
            }
      else if(placeHolder.toUnsignedInt() == 2){
            style = OveSong::PlayStyle::Notation;
            }
      ove_->setPlayStyle(style);

      // show line break
      if (!readBuffer(placeHolder, 1)) { return false; }
      ove_->setShowLineBreak(placeHolder.toBoolean());

      // show ruler
      if (!readBuffer(placeHolder, 1)) { return false; }
      ove_->setShowRuler(placeHolder.toBoolean());

      // show color
      if (!readBuffer(placeHolder, 1)) { return false; }
      ove_->setShowColor(placeHolder.toBoolean());

      return true;
      }

///////////////////////////////////////////////////////////////////////////////
TrackParse::TrackParse(OveSong* ove)
      :BasicParse(ove) {
      }

TrackParse::~TrackParse() {
      }

void TrackParse::setTrack(SizeChunk* chunk) {
      chunk_ = chunk;
      }

bool TrackParse::parse() {
      Block* dataBlock = chunk_->getDataBlock();
      unsigned int blockSize = ove_->getIsVersion4() ? chunk_->getSizeBlock()->toSize() : SizeChunk::version3TrackSize;
      StreamHandle handle(dataBlock->data(), blockSize);
      Block placeHolder;

      handle_ = &handle;

      Track* oveTrack = new Track();
      ove_->addTrack(oveTrack);

      // 2 32bytes long track name buffer
      if( !readBuffer(placeHolder, 32) ) { return false; }
      oveTrack->setName(ove_->getCodecString(placeHolder.fixedSizeBufferToStrByteArray()));

      if( !readBuffer(placeHolder, 32) ) { return false; }
      oveTrack->setBriefName(ove_->getCodecString(placeHolder.fixedSizeBufferToStrByteArray()));

      if( !jump(8) ) { return false; } //0x fffa0012 fffa0012
      if( !jump(1) ) { return false; }

      // patch
      if( !readBuffer(placeHolder, 1) ) { return false; }
      unsigned int thisByte = placeHolder.toInt();
      oveTrack->setPatch(thisByte&0x7f);

      // show name
      if( !readBuffer(placeHolder, 1) ) { return false; }
      oveTrack->setShowName(placeHolder.toBoolean());

      // show brief name
      if( !readBuffer(placeHolder, 1) ) { return false; }
      oveTrack->setShowBriefName(placeHolder.toBoolean());

      if( !jump(1) ) { return false; }

      // show transpose
      if( !readBuffer(placeHolder, 1) ) { return false; }
      oveTrack->setShowTranspose(placeHolder.toBoolean());

      if( !jump(1) ) { return false; }

      // mute
      if( !readBuffer(placeHolder, 1) ) { return false; }
      oveTrack->setMute(placeHolder.toBoolean());

      // solo
      if( !readBuffer(placeHolder, 1) ) { return false; }
      oveTrack->setSolo(placeHolder.toBoolean());

      if( !jump(1) ) { return false; }

      // show key each line
      if( !readBuffer(placeHolder, 1) ) { return false; }
      oveTrack->setShowKeyEachLine(placeHolder.toBoolean());

      // voice count
      if( !readBuffer(placeHolder, 1) ) { return false; }
      oveTrack->setVoiceCount(placeHolder.toUnsignedInt());

      if( !jump(3) ) { return false; }

      // transpose value [-127, 127]
      if( !readBuffer(placeHolder, 1) ) { return false; }
      oveTrack->setTranspose(placeHolder.toInt());

      if( !jump(2) ) { return false; }

      // start clef
      if( !readBuffer(placeHolder, 1) ) { return false; }
      oveTrack->setStartClef(placeHolder.toUnsignedInt());

      // transpose celf
      if( !readBuffer(placeHolder, 1) ) { return false; }
      oveTrack->setTransposeClef(placeHolder.toUnsignedInt());

      // start key
      if( !readBuffer(placeHolder, 1) ) { return false; }
      oveTrack->setStartKey(placeHolder.toUnsignedInt());

      // display percent
      if( !readBuffer(placeHolder, 1) ) { return false; }
      oveTrack->setDisplayPercent(placeHolder.toUnsignedInt());

      // show leger line
      if( !readBuffer(placeHolder, 1) ) { return false; }
      oveTrack->setShowLegerLine(placeHolder.toBoolean());

      // show clef
      if( !readBuffer(placeHolder, 1) ) { return false; }
      oveTrack->setShowClef(placeHolder.toBoolean());

      // show time signature
      if( !readBuffer(placeHolder, 1) ) { return false; }
      oveTrack->setShowTimeSignature(placeHolder.toBoolean());

      // show key signature
      if( !readBuffer(placeHolder, 1) ) { return false; }
      oveTrack->setShowKeySignature(placeHolder.toBoolean());

      // show barline
      if( !readBuffer(placeHolder, 1) ) { return false; }
      oveTrack->setShowBarline(placeHolder.toBoolean());

      // fill with rest
      if( !readBuffer(placeHolder, 1) ) { return false; }
      oveTrack->setFillWithRest(placeHolder.toBoolean());

      // flat tail
      if( !readBuffer(placeHolder, 1) ) { return false; }
      oveTrack->setFlatTail(placeHolder.toBoolean());

      // show clef each line
      if( !readBuffer(placeHolder, 1) ) { return false; }
      oveTrack->setShowClefEachLine(placeHolder.toBoolean());

      if( !jump(12) ) { return false; }

      // 8 voices
      int i;
      QList<Voice*> voices;
      for( i=0; i<8; ++i ) {
            Voice* voicePtr = new Voice();

            if( !jump(5) ) { return false; }

            // channel
            if( !readBuffer(placeHolder, 1) ) { return false; }
            voicePtr->setChannel(placeHolder.toUnsignedInt());

            // volume
            if( !readBuffer(placeHolder, 1) ) { return false; }
            voicePtr->setVolume(placeHolder.toInt());

            // pitch shift
            if( !readBuffer(placeHolder, 1) ) { return false; }
            voicePtr->setPitchShift(placeHolder.toInt());

            // pan
            if( !readBuffer(placeHolder, 1) ) { return false; }
            voicePtr->setPan(placeHolder.toInt());

            if( !jump(6) ) { return false; }

            // patch
            if( !readBuffer(placeHolder, 1) ) { return false; }
            voicePtr->setPatch(placeHolder.toInt());

            voices.push_back(voicePtr);
            }

      // stem type
      for( i=0; i<8; ++i ) {
            if( !readBuffer(placeHolder, 1) ) { return false; }
            voices[i]->setStemType(placeHolder.toUnsignedInt());

            oveTrack->addVoice(voices[i]);
            }

      // percussion define
      QList<Track::DrumNode> nodes;
      for(i=0; i<16; ++i) {
            nodes.push_back(Track::DrumNode());
            }

      // line
      for( i=0; i<16; ++i ) {
            if( !readBuffer(placeHolder, 1) ) { return false; }
            nodes[i].line_ = placeHolder.toInt();
            }

      // head type
      for( i=0; i<16; ++i ) {
            if( !readBuffer(placeHolder, 1) ) { return false; }
            nodes[i].headType_ = placeHolder.toUnsignedInt();
            }

      // pitch
      for( i=0; i<16; ++i ) {
            if( !readBuffer(placeHolder, 1) ) { return false; }
            nodes[i].pitch_ = placeHolder.toUnsignedInt();
            }

      // voice
      for( i=0; i<16; ++i ) {
            if( !readBuffer(placeHolder, 1) ) { return false; }
            nodes[i].voice_ = placeHolder.toUnsignedInt();
            }

      for( i=0; i<nodes.size(); ++i ) {
            oveTrack->addDrum(nodes[i]);
            }

      /* if( !Jump(17) ) { return false; }

   // voice 0 channel
   if( !ReadBuffer(placeHolder, 1) ) { return false; }
   oveTrack->setChannel(placeHolder.toUnsignedInt());

   // to be continued. if anything important...*/

      return true;
      }

///////////////////////////////////////////////////////////////////////////////
GroupParse::GroupParse(OveSong* ove)
      :BasicParse(ove) {
      }

GroupParse::~GroupParse(){
      sizeChunks_.clear();
      }

void GroupParse::addSizeChunk(SizeChunk* sizeChunk) {
      sizeChunks_.push_back(sizeChunk);
      }

bool GroupParse::parse() {
      return false;
      }

///////////////////////////////////////////////////////////////////////////////
PageGroupParse::PageGroupParse(OveSong* ove)
      :BasicParse(ove) {
      }

PageGroupParse::~PageGroupParse(){
      pageChunks_.clear();
      }

void PageGroupParse::addPage(SizeChunk* chunk) {
      pageChunks_.push_back(chunk);
      }

bool PageGroupParse::parse() {
      if( pageChunks_.isEmpty() ) {
            return false;
            }

      int i;
      for( i=0; i<pageChunks_.size(); ++i ) {
            Page* page = new Page();
            ove_->addPage(page);

            if( !parsePage(pageChunks_[i], page) ) { return false; }
            }

      return true;
      }

bool PageGroupParse::parsePage(SizeChunk* chunk, Page* page) {
      Block placeHolder(2);
      StreamHandle handle(chunk->getDataBlock()->data(), chunk->getSizeBlock()->toSize());

      handle_ = &handle;

      // begin line
      if( !readBuffer(placeHolder, 2) ) { return false; }
      page->setBeginLine(placeHolder.toUnsignedInt());

      // line count
      if( !readBuffer(placeHolder, 2) ) { return false; }
      page->setLineCount(placeHolder.toUnsignedInt());

      if( !jump(4) ) { return false; }

      // staff interval
      if( !readBuffer(placeHolder, 2) ) { return false; }
      page->setStaffInterval(placeHolder.toUnsignedInt());

      // line interval
      if( !readBuffer(placeHolder, 2) ) { return false; }
      page->setLineInterval(placeHolder.toUnsignedInt());

      // staff inline interval
      if( !readBuffer(placeHolder, 2) ) { return false; }
      page->setStaffInlineInterval(placeHolder.toUnsignedInt());

      // line bar count
      if( !readBuffer(placeHolder, 2) ) { return false; }
      page->setLineBarCount(placeHolder.toUnsignedInt());

      // page line count
      if( !readBuffer(placeHolder, 2) ) { return false; }
      page->setPageLineCount(placeHolder.toUnsignedInt());

      // left margin
      if( !readBuffer(placeHolder, 4) ) { return false; }
      page->setLeftMargin(placeHolder.toUnsignedInt());

      // top margin
      if( !readBuffer(placeHolder, 4) ) { return false; }
      page->setTopMargin(placeHolder.toUnsignedInt());

      // right margin
      if( !readBuffer(placeHolder, 4) ) { return false; }
      page->setRightMargin(placeHolder.toUnsignedInt());

      // bottom margin
      if( !readBuffer(placeHolder, 4) ) { return false; }
      page->setBottomMargin(placeHolder.toUnsignedInt());

      // page width
      if( !readBuffer(placeHolder, 4) ) { return false; }
      page->setPageWidth(placeHolder.toUnsignedInt());

      // page height
      if( !readBuffer(placeHolder, 4) ) { return false; }
      page->setPageHeight(placeHolder.toUnsignedInt());

      handle_ = NULL;

      return true;
      }

///////////////////////////////////////////////////////////////////////////////
StaffCountGetter::StaffCountGetter(OveSong* ove)
      :BasicParse(ove) {
      }

unsigned int StaffCountGetter::getStaffCount(SizeChunk* chunk) {
      StreamHandle handle(chunk->getDataBlock()->data(), chunk->getSizeBlock()->toSize());
      Block placeHolder;

      handle_ = &handle;

      if( !jump(6) ) { return false; }

      // staff count
      if( !readBuffer(placeHolder, 2) ) { return false; }
      return placeHolder.toUnsignedInt();
      }

///////////////////////////////////////////////////////////////////////////////
LineGroupParse::LineGroupParse(OveSong* ove) :
      BasicParse(ove), chunk_(NULL) {
      }

LineGroupParse::~LineGroupParse(){
      chunk_ = NULL;
      lineChunks_.clear();
      staffChunks_.clear();
      }

void LineGroupParse::setLineGroup(GroupChunk* chunk) {
      chunk_ = chunk;
      }

void LineGroupParse::addLine(SizeChunk* chunk) {
      lineChunks_.push_back(chunk);
      }

void LineGroupParse::addStaff(SizeChunk* chunk) {
      staffChunks_.push_back(chunk);
      }

bool LineGroupParse::parse() {
      if( lineChunks_.isEmpty() || staffChunks_.size() % lineChunks_.size() != 0 ) { return false; }

      int i;
      unsigned int j;
      unsigned int lineStaffCount = staffChunks_.size() / lineChunks_.size();

      for( i=0; i<lineChunks_.size(); ++i ) {
            Line* linePtr = new Line();

            ove_->addLine(linePtr);

            if( !parseLine(lineChunks_[i], linePtr) ) { return false; }

            for( j=lineStaffCount*i; j<lineStaffCount*(i+1); ++j ) {
                  Staff* staffPtr = new Staff();

                  linePtr->addStaff(staffPtr);

                  if( !parseStaff(staffChunks_[j], staffPtr) ) { return false; }
                  }
            }

      return true;
      }

bool LineGroupParse::parseLine(SizeChunk* chunk, Line* line) {
      Block placeHolder;

      StreamHandle handle(chunk->getDataBlock()->data(), chunk->getSizeBlock()->toSize());

      handle_ = &handle;

      if( !jump(2) ) { return false; }

      // begin bar
      if( !readBuffer(placeHolder, 2) ) { return false; }
      line->setBeginBar(placeHolder.toUnsignedInt());

      // bar count
      if( !readBuffer(placeHolder, 2) ) { return false; }
      line->setBarCount(placeHolder.toUnsignedInt());

      if( !jump(6) ) { return false; }

      // y offset
      if( !readBuffer(placeHolder, 2) ) { return false; }
      line->setYOffset(placeHolder.toInt());

      // left x offset
      if( !readBuffer(placeHolder, 2) ) { return false; }
      line->setLeftXOffset(placeHolder.toInt());

      // right x offset
      if( !readBuffer(placeHolder, 2) ) { return false; }
      line->setRightXOffset(placeHolder.toInt());

      if( !jump(4) ) { return false; }

      handle_ = NULL;

      return true;
      }

bool LineGroupParse::parseStaff(SizeChunk* chunk, Staff* staff) {
      Block placeHolder;

      StreamHandle handle(chunk->getDataBlock()->data(), chunk->getSizeBlock()->toSize());

      handle_ = &handle;

      if( !jump(7) ) { return false; }

      // clef
      if( !readBuffer(placeHolder, 1) ) { return false; }
      staff->setClefType(placeHolder.toUnsignedInt());

      // key
      if( !readBuffer(placeHolder, 1) ) { return false; }
      staff->setKeyType(oveKeyToKey(placeHolder.toUnsignedInt()));

      if( !jump(2) ) { return false; }

      // visible
      if( !readBuffer(placeHolder, 1) ) { return false; }
      staff->setVisible(placeHolder.toBoolean());

      if( !jump(12) ) { return false; }

      // y offset
      if( !readBuffer(placeHolder, 2) ) { return false; }
      staff->setYOffset(placeHolder.toInt());

      int jumpAmount = ove_->getIsVersion4() ? 26 : 18;
      if( !jump(jumpAmount) ) { return false; }

      // group type
      if( !readBuffer(placeHolder, 1) ) { return false; }
      GroupType groupType = GroupType::None;
      if(placeHolder.toUnsignedInt() == 1) {
            groupType = GroupType::Brace;
            } else if(placeHolder.toUnsignedInt() == 2) {
            groupType = GroupType::Bracket;
            }
      staff->setGroupType(groupType);

      // group staff count
      if( !readBuffer(placeHolder, 1) ) { return false; }
      staff->setGroupStaffCount(placeHolder.toUnsignedInt());

      handle_ = NULL;

      return true;
      }

///////////////////////////////////////////////////////////////////////////////
BarsParse::BarsParse(OveSong* ove) :
      BasicParse(ove) {
      }

BarsParse::~BarsParse(){
      measureChunks_.clear();
      conductChunks_.clear();
      bdatChunks_.clear();
      }

void BarsParse::addMeasure(SizeChunk* chunk) {
      measureChunks_.push_back(chunk);
      }

void BarsParse::addConduct(SizeChunk* chunk) {
      conductChunks_.push_back(chunk);
      }

void BarsParse::addBdat(SizeChunk* chunk) {
      bdatChunks_.push_back(chunk);
      }

bool BarsParse::parse() {
      int i;
      int trackMeasureCount = ove_->getTrackBarCount();
      int trackCount = ove_->getTrackCount();
      int measureDataCount = trackCount * measureChunks_.size();
      QList<Measure*> measures;
      QList<MeasureData*> measureDatas;

      if( measureChunks_.isEmpty() ||
          measureChunks_.size() != conductChunks_.size() ||
          (int)bdatChunks_.size() != measureDataCount ) {
            return false;
            }

      // add to ove
      for ( i=0; i<(int)measureChunks_.size(); ++i ) {
            Measure* measure = new Measure(i);

            measures.push_back(measure);
            ove_->addMeasure(measure);
            }

      for ( i=0; i<measureDataCount; ++i ) {
            MeasureData* oveMeasureData = new MeasureData();

            measureDatas.push_back(oveMeasureData);
            ove_->addMeasureData(oveMeasureData);
            }

      for( i=0; i<(int)measureChunks_.size(); ++i ) {
            Measure* measure = measures[i];

            // MEAS
            if( !parseMeas(measure, measureChunks_[i]) ) {
                  QString ss = QString("failed in parse MEAS %1\n").arg(i);
                  messageOut(ss);

                  return false;
                  }
            }

      for( i=0; i<(int)conductChunks_.size(); ++i ) {
            // COND
            if( !parseCond(measures[i], measureDatas[i], conductChunks_[i]) ) {
                  QString ss = QString("failed in parse COND %1\n").arg(i);
                  messageOut(ss);

                  return false;
                  }
            }

      for( i=0; i<(int)bdatChunks_.size(); ++i ) {
            int measId = i % trackMeasureCount;

            // BDAT
            if( !parseBdat(measures[measId], measureDatas[i], bdatChunks_[i]) ) {
                  QString ss = QString("failed in parse BDAT %1\n").arg(i);
                  messageOut(ss);

                  return false;
                  }

            if( notify_ != NULL ) {
                  int measureID = i % trackMeasureCount;
                  int trackID = i / trackMeasureCount;

                  //msg.msg_ = OVE_IMPORT_POS;
                  //msg.param1_ = (measureID<<16) + trackMeasureCount;
                  //msg.param2_ = (trackID<<16) + trackCount;

                  notify_->loadPosition(measureID, trackMeasureCount, trackID, trackCount);
                  }
            }

      return true;
      }

bool BarsParse::parseMeas(Measure* measure, SizeChunk* chunk) {
      Block placeHolder;

      StreamHandle measureHandle(chunk->getDataBlock()->data(), chunk->getSizeBlock()->toSize());

      handle_ = &measureHandle;

      if( !jump(2) ) { return false; }

      // multi-measure rest
      if( !readBuffer(placeHolder, 1) ) { return false; }
      measure->setIsMultiMeasureRest(placeHolder.toBoolean());

      // pickup
      if( !readBuffer(placeHolder, 1) ) { return false; }
      measure->setIsPickup(placeHolder.toBoolean());

      if( !jump(4) ) { return false; }

      // left barline
      if( !readBuffer(placeHolder, 1) ) { return false; }
      measure->setLeftBarline(placeHolder.toUnsignedInt());

      // right barline
      if( !readBuffer(placeHolder, 1) ) { return false; }
      measure->setRightBarline(placeHolder.toUnsignedInt());

      // tempo
      if( !readBuffer(placeHolder, 2) ) { return false; }
      double tempo = ((double)placeHolder.toUnsignedInt());
      if( ove_->getIsVersion4() ) {
            tempo /= 100.0;
            }
      measure->setTypeTempo(tempo);

      // bar length(tick)
      if( !readBuffer(placeHolder, 2) ) { return false; }
      measure->setLength(placeHolder.toUnsignedInt());

      if( !jump(6) ) { return false; }

      // bar number offset
      if( !parseOffsetElement(measure->getBarNumber()) ) { return false; }

      if( !jump(2) ) { return false; }

      // multi-measure rest count
      if( !readBuffer(placeHolder, 2) ) { return false; }
      measure->setMultiMeasureRestCount(placeHolder.toUnsignedInt());

      handle_ = NULL;

      return true;
      }

bool BarsParse::parseCond(Measure* measure, MeasureData* measureData, SizeChunk* chunk) {
      Block placeHolder;

      StreamHandle handle(chunk->getDataBlock()->data(), chunk->getSizeBlock()->toSize());

      handle_ = &handle;

      // item count
      if( !readBuffer(placeHolder, 2) ) { return false; }
      unsigned int cnt = placeHolder.toUnsignedInt();

      if( !parseTimeSignature(measure, 36) ) { return false; }

      for( unsigned int i=0; i<cnt; ++i ) {
            if( !readBuffer(placeHolder, 2) ) { return false; }
            unsigned int twoByte = placeHolder.toUnsignedInt();
            unsigned int oldBlockSize = twoByte - 11;
            unsigned int newBlockSize = twoByte - 7;

            // type id
            if( !readBuffer(placeHolder, 1) ) { return false; }
            unsigned int thisByte = placeHolder.toUnsignedInt();
            CondType type;

            if( !getCondElementType(thisByte, type) ) { return false; }

            switch (type) {
                  case CondType::Bar_Number: {
                        if (!parseBarNumber(measure, twoByte - 1)) {
                              return false;
                              }
                        break;
                        }
                  case CondType::Repeat: {
                        if (!parseRepeatSymbol(measureData, oldBlockSize)) {
                              return false;
                              }
                        break;
                        }
                  case CondType::Numeric_Ending: {
                        if (!parseNumericEndings(measureData, oldBlockSize)) {
                              return false;
                              }
                        break;
                        }
                  case CondType::Decorator: {
                        if (!parseDecorators(measureData, newBlockSize)) {
                              return false;
                              }
                        break;
                        }
                  case CondType::Tempo: {
                        if (!parseTempo(measureData, newBlockSize)) {
                              return false;
                              }
                        break;
                        }
                  case CondType::Text: {
                        if (!parseText(measureData, newBlockSize)) {
                              return false;
                              }
                        break;
                        }
                  case CondType::Expression: {
                        if (!parseExpressions(measureData, newBlockSize)) {
                              return false;
                              }
                        break;
                        }
                  case CondType::Time_Parameters: {
                        if (!parseTimeSignatureParameters(measure, newBlockSize)) {
                              return false;
                              }
                        break;
                        }
                  case CondType::Barline_Parameters: {
                        if (!parseBarlineParameters(measure, newBlockSize)) {
                              return false;
                              }
                        break;
                        }
                  default: {
                        if (!jump(newBlockSize)) {
                              return false;
                              }
                        break;
                        }
                  }
            }

      handle_ = NULL;

      return true;
      }

bool BarsParse::parseTimeSignature(Measure* measure, int /*length*/) {
      Block placeHolder;

      TimeSignature* timeSignature = measure->getTime();

      // numerator
      if( !readBuffer(placeHolder, 1) ) { return false; }
      timeSignature->setNumerator(placeHolder.toUnsignedInt());

      // denominator
      if( !readBuffer(placeHolder, 1) ) { return false; }
      timeSignature->setDenominator(placeHolder.toUnsignedInt());

      if( !jump(2) ) { return false; }

      // beat length
      if( !readBuffer(placeHolder, 2) ) { return false; }
      timeSignature->setBeatLength(placeHolder.toUnsignedInt());

      // bar length
      if( !readBuffer(placeHolder, 2) ) { return false; }
      timeSignature->setBarLength(placeHolder.toUnsignedInt());

      if( !jump(4) ) { return false; }

      // is symbol
      if( !readBuffer(placeHolder, 1) ) { return false; }
      timeSignature->setIsSymbol(placeHolder.toBoolean());

      if( !jump(1) ) { return false; }

      // replace font
      if( !readBuffer(placeHolder, 1) ) { return false; }
      timeSignature->setReplaceFont(placeHolder.toBoolean());

      // color
      if( !readBuffer(placeHolder, 1) ) { return false; }
      timeSignature->setColor(placeHolder.toUnsignedInt());

      // show
      if( !readBuffer(placeHolder, 1) ) { return false; }
      timeSignature->setShow(placeHolder.toBoolean());

      // show beat group
      if( !readBuffer(placeHolder, 1) ) { return false; }
      timeSignature->setShowBeatGroup(placeHolder.toBoolean());

      if( !jump(6) ) { return false; }

      // numerator 1, 2, 3
      if( !readBuffer(placeHolder, 1) ) { return false; }
      timeSignature->setGroupNumerator1(placeHolder.toUnsignedInt());
      if( !readBuffer(placeHolder, 1) ) { return false; }
      timeSignature->setGroupNumerator2(placeHolder.toUnsignedInt());
      if( !readBuffer(placeHolder, 1) ) { return false; }
      timeSignature->setGroupNumerator3(placeHolder.toUnsignedInt());

      // denominator
      if( !readBuffer(placeHolder, 1) ) { return false; }
      timeSignature->setGroupDenominator1(placeHolder.toUnsignedInt());
      if( !readBuffer(placeHolder, 1) ) { return false; }
      timeSignature->setGroupDenominator2(placeHolder.toUnsignedInt());
      if( !readBuffer(placeHolder, 1) ) { return false; }
      timeSignature->setGroupDenominator3(placeHolder.toUnsignedInt());

      // beam group 1~4
      if( !readBuffer(placeHolder, 1) ) { return false; }
      timeSignature->setBeamGroup1(placeHolder.toUnsignedInt());
      if( !readBuffer(placeHolder, 1) ) { return false; }
      timeSignature->setBeamGroup2(placeHolder.toUnsignedInt());
      if( !readBuffer(placeHolder, 1) ) { return false; }
      timeSignature->setBeamGroup3(placeHolder.toUnsignedInt());
      if( !readBuffer(placeHolder, 1) ) { return false; }
      timeSignature->setBeamGroup4(placeHolder.toUnsignedInt());

      // beam 16th
      if( !readBuffer(placeHolder, 1) ) { return false; }
      timeSignature->set16thBeamCount(placeHolder.toUnsignedInt());

      // beam 32th
      if( !readBuffer(placeHolder, 1) ) { return false; }
      timeSignature->set32thBeamCount(placeHolder.toUnsignedInt());

      return true;
      }

bool BarsParse::parseTimeSignatureParameters(Measure* measure, int length) {
      Block placeHolder;
      TimeSignature* ts = measure->getTime();

      int cursor = ove_->getIsVersion4() ? 10 : 8;
      if( !jump(cursor) ) { return false; }

      // numerator
      if( !readBuffer(placeHolder, 1) ) { return false; }
      unsigned int numerator = placeHolder.toUnsignedInt();

      cursor = ove_->getIsVersion4() ? 11 : 9;
      if( ( length - cursor ) % 8 != 0 || (length - cursor) / 8 != (int)numerator ) {
            return false;
            }

      for( unsigned int i =0; i<numerator; ++i ) {
            // beat start unit
            if( !readBuffer(placeHolder, 2) ) { return false; }
            int beatStart = placeHolder.toUnsignedInt();

            // beat length unit
            if( !readBuffer(placeHolder, 2) ) { return false; }
            int beatLength = placeHolder.toUnsignedInt();

            if( !jump(2) ) { return false; }

            // beat start tick
            if( !readBuffer(placeHolder, 2) ) { return false; }
            int beatStartTick = placeHolder.toUnsignedInt();

            ts->addBeat(beatStart, beatLength, beatStartTick);
            }

      ts->endAddBeat();

      return true;
      }

bool BarsParse::parseBarlineParameters(Measure* measure, int /*length*/) {
      Block placeHolder;

      int cursor = ove_->getIsVersion4() ? 12 : 10;
      if( !jump(cursor) ) { return false; }

      // repeat count
      if( !readBuffer(placeHolder, 1) ) { return false; }
      int repeatCount = placeHolder.toUnsignedInt();

      measure->setBackwardRepeatCount(repeatCount);

      if( !jump(6) ) { return false; }

      return true;
      }

bool BarsParse::parseNumericEndings(MeasureData* measureData, int /*length*/) {
      Block placeHolder;

      NumericEnding* numeric = new NumericEnding();
      measureData->addCrossMeasureElement(numeric, true);

      if( !jump(3) ) { return false; }

      // common
      if( !parseCommonBlock(numeric) ) { return false; }

      if( !jump(6) ) { return false; }

      // measure count
      if( !readBuffer(placeHolder, 2) ) { return false; }
      //int offsetMeasure = placeHolder.toUnsignedInt() - 1;
      int offsetMeasure = placeHolder.toUnsignedInt();
      numeric->stop()->setMeasure(offsetMeasure);

      if( !jump(2) ) { return false; }

      // left x offset
      if( !readBuffer(placeHolder, 2) ) { return false; }
      numeric->getLeftShoulder()->setXOffset(placeHolder.toInt());

      // height
      if( !readBuffer(placeHolder, 2) ) { return false; }
      numeric->setHeight(placeHolder.toUnsignedInt());

      // left x offset
      if( !readBuffer(placeHolder, 2) ) { return false; }
      numeric->getRightShoulder()->setXOffset(placeHolder.toInt());

      if( !jump(2) ) { return false; }

      // y offset
      if( !readBuffer(placeHolder, 2) ) { return false; }
      numeric->getLeftShoulder()->setYOffset(placeHolder.toInt());
      numeric->getRightShoulder()->setYOffset(placeHolder.toInt());

      // number offset
      if( !readBuffer(placeHolder, 2) ) { return false; }
      numeric->getNumericHandle()->setXOffset(placeHolder.toInt());
      if( !readBuffer(placeHolder, 2) ) { return false; }
      numeric->getNumericHandle()->setYOffset(placeHolder.toInt());

      if( !jump(6) ) { return false; }

      // text size
      if( !readBuffer(placeHolder, 1) ) { return false; }
      unsigned int size = placeHolder.toUnsignedInt();

      // text : size maybe a huge value
      if( !readBuffer(placeHolder, size) ) { return false; }
      numeric->setText(ove_->getCodecString(placeHolder.fixedSizeBufferToStrByteArray()));

      // fix for wedding march.ove
      if( size % 2 == 0 ) {
            if( !jump(1) ) { return false; }
            }

      return true;
      }

bool BarsParse::parseTempo(MeasureData* measureData, int /*length*/) {
      Block placeHolder;
      unsigned int thisByte;

      Tempo* tempo = new Tempo();
      measureData->addMusicData(tempo);
      if( !jump(3) )
            return false;
      // common
      if( !parseCommonBlock(tempo) )
            return false;
      if( !readBuffer(placeHolder, 1) )
            return false;
      thisByte = placeHolder.toUnsignedInt();
      // show tempo
      tempo->setShowMark( (getHighNibble(thisByte) & 0x4) == 0x4 );
      // show before text
      tempo->setShowBeforeText( (getHighNibble(thisByte) & 0x8 ) == 0x8 ) ;
      // show parenthesis
      tempo->setShowParenthesis( (getHighNibble(thisByte) & 0x1 ) == 0x1 );
      // left note type
      tempo->setLeftNoteType( getLowNibble(thisByte) );
      // left note dot
      tempo->setLeftNoteDot((getHighNibble(thisByte) & 0x2 ) == 0x2 );
      if( !jump(1) )  // dimension of the note symbol
            return false;
      if( ove_->getIsVersion4() ) {
            if( !jump(2) )
                  return false;
            // tempo
            if( !readBuffer(placeHolder, 2) )
                  return false;
            tempo->setTypeTempo(((double)placeHolder.toUnsignedInt())/100.0);
            }
      else {
            // tempo
            if( !readBuffer(placeHolder, 2) )
                  return false;
            tempo->setTypeTempo((double)placeHolder.toUnsignedInt());
            if( !jump(2) )
                  return false;
            }
      // offset
      if( !parseOffsetElement(tempo) )
            return false;
      if( !jump(16) )
            return false;
      // 31 bytes left text
      if( !readBuffer(placeHolder, 31) )
            return false;
      tempo->setLeftText(ove_->getCodecString(placeHolder.fixedSizeBufferToStrByteArray()));

      if( !readBuffer(placeHolder, 1) )
            return false;
      thisByte = placeHolder.toUnsignedInt();
      // swing eighth
      tempo->setSwingEighth((getHighNibble(thisByte) & 0x4 ) == 0x4 );
      // right note dot
      tempo->setRightNoteDot((getHighNibble(thisByte) & 0x1 ) == 0x1 );
      // compatibility with v3 files ?
      tempo->setRightSideType((int)(getHighNibble(thisByte) & 0x2));
      // right note type
      tempo->setRightNoteType(getLowNibble(thisByte));
      // right text
      if( ove_->getIsVersion4() ) {
            if( !readBuffer(placeHolder, 31) )
                  return false;
            tempo->setRightText(ove_->getCodecString(placeHolder.fixedSizeBufferToStrByteArray()));
            if( !readBuffer(placeHolder, 1) )
                  return false;
            // 00 -> float      03 -> integer(floor)     01 -> notetype    02 -> text
            tempo->setRightSideType(placeHolder.toInt());
            }

      return true;
      }


bool BarsParse::parseBarNumber(Measure* measure, int /*length*/) {
      Block placeHolder;

      BarNumber* barNumber = measure->getBarNumber();

      if( !jump(2) ) { return false; }

      // show on paragraph start
      if( !readBuffer(placeHolder, 1) ) { return false; }
      barNumber->setShowOnParagraphStart(getLowNibble(placeHolder.toUnsignedInt())==8);

      unsigned int blankSize = ove_->getIsVersion4() ? 9 : 7;
      if( !jump(blankSize) ) { return false; }

      // text align
      if( !readBuffer(placeHolder, 1) ) { return false; }
      barNumber->setAlign(placeHolder.toUnsignedInt());

      if( !jump(4) ) { return false; }

      // show flag
      if( !readBuffer(placeHolder, 1) ) { return false; }
      barNumber->setShowFlag(placeHolder.toUnsignedInt());

      if( !jump(10) ) { return false; }

      // bar range
      if( !readBuffer(placeHolder, 1) ) { return false; }
      barNumber->setShowEveryBarCount(placeHolder.toUnsignedInt());

      // prefix
      if( !readBuffer(placeHolder, 2) ) { return false; }
      barNumber->setPrefix(ove_->getCodecString(placeHolder.fixedSizeBufferToStrByteArray()));

      if( !jump(18) ) { return false; }

      return true;
      }

bool BarsParse::parseText(MeasureData* measureData, int length) {
      Block placeHolder;

      Text* text = new Text();
      measureData->addMusicData(text);

      if( !jump(3) ) { return false; }

      // common
      if( !parseCommonBlock(text) ) { return false; }

      // type
      if( !readBuffer(placeHolder, 1) ) { return false; }
      unsigned int thisByte = placeHolder.toUnsignedInt();
      bool includeLineBreak = ( (getHighNibble(thisByte)&0x2) != 0x2 );
      unsigned int id = getLowNibble(thisByte);
      Text::Type textType = Text::Type::Rehearsal;

      if (id == 0) {
            textType = Text::Type::MeasureText;
            } else if (id == 1) {
            textType = Text::Type::SystemText;
            } else // id ==2
            {
            textType = Text::Type::Rehearsal;
            }

      text->setTextType(textType);

      if( !jump(1) ) { return false; }

      // x offset
      if( !readBuffer(placeHolder, 4) ) { return false; }
      text->setXOffset(placeHolder.toInt());

      // y offset
      if( !readBuffer(placeHolder, 4) ) { return false; }
      text->setYOffset(placeHolder.toInt());

      // width
      if( !readBuffer(placeHolder, 4) ) { return false; }
      text->setWidth(placeHolder.toUnsignedInt());

      // height
      if( !readBuffer(placeHolder, 4) ) { return false; }
      text->setHeight(placeHolder.toUnsignedInt());

      if( !jump(7) ) { return false; }

      // horizontal margin
      if( !readBuffer(placeHolder, 1) ) { return false; }
      text->setHorizontalMargin(placeHolder.toUnsignedInt());

      if( !jump(1) ) { return false; }

      // vertical margin
      if( !readBuffer(placeHolder, 1) ) { return false; }
      text->setVerticalMargin(placeHolder.toUnsignedInt());

      if( !jump(1) ) { return false; }

      // line thick
      if( !readBuffer(placeHolder, 1) ) { return false; }
      text->setLineThick(placeHolder.toUnsignedInt());

      if( !jump(2) ) { return false; }

      // text size
      if( !readBuffer(placeHolder, 2) ) { return false; }
      unsigned int size = placeHolder.toUnsignedInt();

      // text string, maybe huge
      if( !readBuffer(placeHolder, size) ) { return false; }
      text->setText(ove_->getCodecString(placeHolder.fixedSizeBufferToStrByteArray()));

      if( !includeLineBreak ) {
            if( !jump(6) ) { return false; }
            } else {
            unsigned int cursor = ove_->getIsVersion4() ? 43 : 41;
            cursor += size;

            // multi lines of text
            for( unsigned int i=0; i<2; ++i ) {
                  if( (int)cursor < length ) {
                        // line parameters count
                        if( !readBuffer(placeHolder, 2) ) { return false; }
                        unsigned int lineCount = placeHolder.toUnsignedInt();

                        if( i==0 && int(cursor + 2 + 8*lineCount) > length ) {
                              return false;
                              }

                        if( i==1 && int(cursor + 2 + 8*lineCount) != length ) {
                              return false;
                              }

                        if( !jump(8*lineCount) ) { return false; }

                        cursor += 2 + 8*lineCount;
                        }
                  }
            }

      return true;
      }

bool BarsParse::parseRepeatSymbol(MeasureData* measureData, int /*length*/) {
      Block placeHolder;

      RepeatSymbol* repeat = new RepeatSymbol();
      measureData->addMusicData(repeat);

      if( !jump(3) ) { return false; }

      // common
      if( !parseCommonBlock(repeat) ) { return false; }

      // RepeatType
      if( !readBuffer(placeHolder, 1) ) { return false; }
      repeat->setRepeatType(placeHolder.toUnsignedInt());

      if( !jump(13) ) { return false; }

      // offset
      if( !parseOffsetElement(repeat) ) { return false; }

      if( !jump(15) ) { return false; }

      // size
      if( !readBuffer(placeHolder, 2) ) { return false; }
      unsigned int size = placeHolder.toUnsignedInt();

      // text, maybe huge
      if( !readBuffer(placeHolder, size) ) { return false; }
      repeat->setText(ove_->getCodecString(placeHolder.fixedSizeBufferToStrByteArray()));

      // last 0
      if( size % 2 == 0 ) {
            if( !jump(1) ) { return false; }
            }

      return true;
      }

bool BarsParse::parseBdat(Measure* /*measure*/, MeasureData* measureData, SizeChunk* chunk) {
      Block placeHolder;
      StreamHandle handle(chunk->getDataBlock()->data(), chunk->getSizeBlock()->toSize());

      handle_ = &handle;

      // parse here
      if( !readBuffer(placeHolder, 2) ) { return false; }
      unsigned int cnt = placeHolder.toUnsignedInt();

      for( unsigned int i=0; i<cnt; ++i ) {
            // 0x0028 or 0x0016 or 0x002C
            if( !readBuffer(placeHolder, 2) ) { return false; }
            unsigned int count = placeHolder.toUnsignedInt() - 7;

            // type id
            if( !readBuffer(placeHolder, 1) ) { return false; }
            unsigned int thisByte = placeHolder.toUnsignedInt();
            BdatType type;

            if( !getBdatElementType(thisByte, type) ) { return false; }

            switch( type ) {
                  case BdatType::Raw_Note :
                  case BdatType::Rest :
                  case BdatType::Note : {
                        if( !parseNoteRest(measureData, count, type) ) { return false; }
                        break;
                        }
                  case BdatType::Beam : {
                        if( !parseBeam(measureData, count) ) { return false; }
                        break;
                        }
                  case BdatType::Harmony : {
                        if( !parseHarmony(measureData, count) ) { return false; }
                        break;
                        }
                  case BdatType::Clef : {
                        if( !parseClef(measureData, count) ) { return false; }
                        break;
                        }
                  case BdatType::Dynamics : {
                        if( !parseDynamics(measureData, count) ) { return false; }
                        break;
                        }
                  case BdatType::Wedge : {
                        if( !parseWedge(measureData, count) ) { return false; }
                        break;
                        }
                  case BdatType::Glissando : {
                        if( !parseGlissando(measureData, count) ) { return false; }
                        break;
                        }
                  case BdatType::Decorator : {
                        if( !parseDecorators(measureData, count) ) { return false; }
                        break;
                        }
                  case BdatType::Key : {
                        if( !parseKey(measureData, count) ) { return false; }
                        break;
                        }
                  case BdatType::Lyric : {
                        if( !parseLyric(measureData, count) ) { return false; }
                        break;
                        }
                  case BdatType::Octave_Shift: {
                        if( !parseOctaveShift(measureData, count) ) { return false; }
                        break;
                        }
                  case BdatType::Slur : {
                        if( !parseSlur(measureData, count) ) { return false; }
                        break;
                        }
                  case BdatType::Text : {
                        if( !parseText(measureData, count) ) { return false; }
                        break;
                        }
                  case BdatType::Tie : {
                        if( !parseTie(measureData, count) ) { return false; }
                        break;
                        }
                  case BdatType::Tuplet : {
                        if( !parseTuplet(measureData, count) ) { return false; }
                        break;
                        }
                  case BdatType::Guitar_Bend :
                  case BdatType::Guitar_Barre : {
                        if( !parseSizeBlock(count) ) { return false; }
                        break;
                        }
                  case BdatType::Pedal: {
                        if( !parsePedal(measureData, count) ) { return false; }
                        break;
                        }
                  case BdatType::KuoHao: {
                        if( !parseKuohao(measureData, count) ) { return false; }
                        break;
                        }
                  case BdatType::Expressions: {
                        if( !parseExpressions(measureData, count) ) { return false; }
                        break;
                        }
                  case BdatType::Harp_Pedal: {
                        if( !parseHarpPedal(measureData, count) ) { return false; }
                        break;
                        }
                  case BdatType::Multi_Measure_Rest: {
                        if( !parseMultiMeasureRest(measureData, count) ) { return false; }
                        break;
                        }
                  case BdatType::Harmony_GuitarFrame: {
                        if( !parseHarmonyGuitarFrame(measureData, count) ) { return false; }
                        break;
                        }
                  case BdatType::Graphics_40:
                  case BdatType::Graphics_RoundRect:
                  case BdatType::Graphics_Rect:
                  case BdatType::Graphics_Round:
                  case BdatType::Graphics_Line:
                  case BdatType::Graphics_Curve:
                  case BdatType::Graphics_WedgeSymbol: {
                        if( !parseSizeBlock(count) ) { return false; }
                        break;
                        }
                  case BdatType::Midi_Controller : {
                        if( !parseMidiController(measureData, count) ) { return false; }
                        break;
                        }
                  case BdatType::Midi_Program_Change : {
                        if( !parseMidiProgramChange(measureData, count) ) { return false; }
                        break;
                        }
                  case BdatType::Midi_Channel_Pressure : {
                        if( !parseMidiChannelPressure(measureData, count) ) { return false; }
                        break;
                        }
                  case BdatType::Midi_Pitch_Wheel : {
                        if( !parseMidiPitchWheel(measureData, count) ) { return false; }
                        break;
                        }
                  default: {
                        if( !jump(count) ) { return false; }
                        break;
                        }
                  }

            // if i==count-1 then is bar end place holder
            }

      handle_ = NULL;

      return true;
      }

int getInt(int byte, int bits) {
      int num = 0;

      if( bits > 0 ) {
            int factor = int(pow(2.0, bits-1));
            num = (byte % (factor*2));

            if ( (byte & factor) == factor ) {
                  num -= factor*2;
                  }
            }

      return num;
      }

bool BarsParse::parseNoteRest(MeasureData* measureData, int length, BdatType type) {
      NoteContainer* container = new NoteContainer();
      Block placeHolder;
      unsigned int thisByte;

      measureData->addNoteContainer(container);
      measureData->addMusicData(container);

      // note|rest & grace
      container->setIsRest(type==BdatType::Rest);
      container->setIsRaw(type==BdatType::Raw_Note);

      if( !readBuffer(placeHolder, 2) ) { return false; }
      thisByte = placeHolder.toUnsignedInt();
      container->setIsGrace( thisByte == 0x3C00 );
      container->setIsCue( thisByte == 0x4B40 || thisByte == 0x3240 );

      // show / hide
      if( !readBuffer(placeHolder, 1) ) { return false; }
      thisByte = placeHolder.toUnsignedInt();
      container->setShow(getLowNibble(thisByte)!=0x8);

      // voice
      container->setVoice(getLowNibble(thisByte)&0x7);

      // common
      if( !parseCommonBlock(container) ) { return false; }

      // tuplet
      if( !readBuffer(placeHolder, 1) ) { return false; }
      container->setTuplet(placeHolder.toUnsignedInt());

      // space
      if( !readBuffer(placeHolder, 1) ) { return false; }
      container->setSpace(placeHolder.toUnsignedInt());

      // in beam
      if( !readBuffer(placeHolder, 1) ) { return false; }
      thisByte = placeHolder.toUnsignedInt();
      bool inBeam = ( getHighNibble(thisByte) & 0x1 ) == 0x1;
      container->setInBeam(inBeam);

      // grace NoteType
      container->setGraceNoteType((NoteType)getHighNibble(thisByte));

      // dot
      container->setDot(getLowNibble(thisByte)&0x03);

      // NoteType
      if( !readBuffer(placeHolder, 1) ) { return false; }
      thisByte = placeHolder.toUnsignedInt();
      container->setNoteType((NoteType)getLowNibble(thisByte));

      int cursor = 0;

      if( type == BdatType::Rest ) {
            Note* restPtr = new Note();
            container->addNoteRest(restPtr);
            restPtr->setIsRest(true);

            // line
            if( !readBuffer(placeHolder, 1) ) { return false; }
            restPtr->setLine(placeHolder.toInt());

            if( !jump(1) ) { return false; }

            cursor = ove_->getIsVersion4() ? 16 : 14;
            } else // type == Bdat_Note || type == Bdat_Raw_Note
            {
            // stem up 0x80, stem down 0x00
            if( !readBuffer(placeHolder, 1) ) { return false; }
            thisByte = placeHolder.toUnsignedInt();
            container->setStemUp((getHighNibble(thisByte)&0x8)==0x8);

            // stem length
            int stemOffset = thisByte%0x80;
            container->setStemLength(getInt(stemOffset, 7)+7/*3.5 line span*/);

            // show stem 0x00, hide stem 0x40
            if( !readBuffer(placeHolder, 1) ) { return false; }
            bool hideStem = getHighNibble(thisByte)==0x4;
            container->setShowStem(!hideStem);

            if( !jump(1) ) { return false; }

            // note count
            if( !readBuffer(placeHolder, 1) ) { return false; }
            unsigned int noteCount = placeHolder.toUnsignedInt();
            unsigned int i;

            // each note 16 bytes
            for( i=0; i<noteCount; ++i ) {
                  Note* notePtr = new Note();
                  notePtr->setIsRest(false);

                  container->addNoteRest(notePtr);

                  // note show / hide
                  if( !readBuffer(placeHolder, 1) ) { return false; }
                  thisByte = placeHolder.toUnsignedInt();
                  notePtr->setShow((thisByte&0x80) != 0x80);

                  // notehead type
                  notePtr->setHeadType(thisByte&0x7f);

                  // tie pos
                  if( !readBuffer(placeHolder, 1) ) { return false; }
                  thisByte = placeHolder.toUnsignedInt();
                  notePtr->setTiePos(getHighNibble(thisByte));

                  // offset staff, in {-1, 0, 1}
                  if( !readBuffer(placeHolder, 1) ) { return false; }
                  thisByte = getLowNibble(placeHolder.toUnsignedInt());
                  int offsetStaff = 0;
                  if( thisByte == 1 ) { offsetStaff = 1; }
                  if( thisByte == 7 ) { offsetStaff = -1; }
                  notePtr->setOffsetStaff(offsetStaff);

                  // accidental
                  if( !readBuffer(placeHolder, 1) ) { return false; }
                  thisByte = placeHolder.toUnsignedInt();
                  notePtr->setAccidental(getLowNibble(thisByte));
                  // accidental 0: influenced by key, 4: influenced by previous accidental in measure
                  //bool notShow = ( getHighNibble(thisByte) == 0 ) || ( getHighNibble(thisByte) == 4 );
                  bool notShow = !(getHighNibble(thisByte)&0x1);
                  notePtr->setShowAccidental(!notShow);

                  if( !jump(1) ) { return false; }

                  // line
                  if( !readBuffer(placeHolder, 1) ) { return false; }
                  notePtr->setLine(placeHolder.toInt());

                  if( !jump(1) ) { return false; }

                  // note
                  if( !readBuffer(placeHolder, 1) ) { return false; }
                  unsigned int note = placeHolder.toUnsignedInt();
                  notePtr->setNote(note);

                  // note on velocity
                  if( !readBuffer(placeHolder, 1) ) { return false; }
                  unsigned int onVelocity = placeHolder.toUnsignedInt();
                  notePtr->setOnVelocity(onVelocity);

                  // note off velocity
                  if( !readBuffer(placeHolder, 1) ) { return false; }
                  unsigned int offVelocity = placeHolder.toUnsignedInt();
                  notePtr->setOffVelocity(offVelocity);

                  if( !jump(2) ) { return false; }

                  // length (tick)
                  if( !readBuffer(placeHolder, 2) ) { return false; }
                  container->setLength(placeHolder.toUnsignedInt());

                  // offset tick
                  if( !readBuffer(placeHolder, 2) ) { return false; }
                  notePtr->setOffsetTick(placeHolder.toInt());
                  }

            cursor = ove_->getIsVersion4() ? 18 : 16;
            cursor += noteCount * 16/*note size*/;
            }

      // articulation
      while ( cursor < length + 1/* 0x70 || 0x80 || 0x90 */ ) {
            Articulation* art = new Articulation();
            container->addArticulation(art);

            // block size
            if( !readBuffer(placeHolder, 2) ) { return false; }
            int blockSize = placeHolder.toUnsignedInt();

            // articulation type
            if( !readBuffer(placeHolder, 1) ) { return false; }
            art->setArtType(placeHolder.toUnsignedInt());

            // placement
            if( !readBuffer(placeHolder, 1) ) { return false; }
            art->setPlacementAbove(placeHolder.toUnsignedInt()!=0x00); //0x00:below, 0x30:above

            // offset
            if( !parseOffsetElement(art) ) { return false; }

            if( !ove_->getIsVersion4() ) {
                  if( blockSize - 8 > 0 ) {
                        if( !jump(blockSize-8) ) { return false; }
                        }
                  } else {
                  // setting
                  if( !readBuffer(placeHolder, 1) ) { return false; }
                  thisByte = placeHolder.toUnsignedInt();
                  const bool changeSoundEffect = ( ( thisByte & 0x1 ) == 0x1 );
                  const bool changeLength = ( ( thisByte & 0x2 ) == 0x2 );
                  const bool changeVelocity = ( ( thisByte & 0x4 ) == 0x4 );
                  //const bool changeExtraLength = ( ( thisByte & 0x20 ) == 0x20 );

                  if( !jump(8) ) { return false; }

                  // velocity type
                  if( !readBuffer(placeHolder, 1) ) { return false; }
                  thisByte = placeHolder.toUnsignedInt();
                  if( changeVelocity ) {
                        art->setVelocityType((Articulation::VelocityType)thisByte);
                        }

                  if( !jump(14) ) { return false; }

                  // sound effect
                  if( !readBuffer(placeHolder, 2) ) { return false; }
                  int from = placeHolder.toInt();
                  if( !readBuffer(placeHolder, 2) ) { return false; }
                  int to = placeHolder.toInt();
                  if( changeSoundEffect ) {
                        art->setSoundEffect(from, to);
                        }

                  if( !jump(1) ) { return false; }

                  // length percentage
                  if( !readBuffer(placeHolder, 1) ) { return false; }
                  if( changeLength ) {
                        art->setLengthPercentage(placeHolder.toUnsignedInt());
                        }

                  // velocity
                  if( !readBuffer(placeHolder, 2) ) { return false; }
                  if( changeVelocity ) {
                        art->setVelocityValue(placeHolder.toInt());
                        }

                  if( Articulation::isTrill(art->getArtType()) ) {
                        if( !jump(8) ) { return false; }

                        // trill note length
                        if( !readBuffer(placeHolder, 1) ) { return false; }
                        art->setTrillNoteLength(placeHolder.toUnsignedInt());

                        // trill rate
                        if( !readBuffer(placeHolder, 1) ) { return false; }
                        thisByte = placeHolder.toUnsignedInt();
                        NoteType trillNoteType = NoteType::Note_Sixteen;
                        switch ( getHighNibble(thisByte) ) {
                              case 0:
                                    trillNoteType = NoteType::Note_None;
                                    break;
                              case 1:
                                    trillNoteType = NoteType::Note_Sixteen;
                                    break;
                              case 2:
                                    trillNoteType = NoteType::Note_32;
                                    break;
                              case 3:
                                    trillNoteType = NoteType::Note_64;
                                    break;
                              case 4:
                                    trillNoteType = NoteType::Note_128;
                                    break;
                              default:
                                    break;
                              }
                        art->setTrillRate(trillNoteType);

                        // accelerate type
                        art->setAccelerateType(thisByte&0xf);

                        if( !jump(1) ) { return false; }

                        // auxiliary first
                        if( !readBuffer(placeHolder, 1) ) { return false; }
                        art->setAuxiliaryFirst(placeHolder.toBoolean());

                        if( !jump(1) ) { return false; }

                        // trill interval
                        if( !readBuffer(placeHolder, 1) ) { return false; }
                        art->setTrillInterval(placeHolder.toUnsignedInt());
                        } else {
                        if( blockSize > 40 ) {
                              if( !jump( blockSize - 40 ) ) { return false; }
                              }
                        }
                  }

            cursor += blockSize;
            }

      return true;
      }

int tupletToSpace(int tuplet) {
      int a(1);

      while( a*2 < tuplet ) {
            a *= 2;
            }

      return a;
      }

bool BarsParse::parseBeam(MeasureData* measureData, int length) {
      int i;
      Block placeHolder;

      Beam* beam = new Beam();
      measureData->addCrossMeasureElement(beam, true);

      // maybe create tuplet, for < quarter & tool 3(
      bool createTuplet = false;
      int maxEndUnit = 0;
      Tuplet* tuplet = new Tuplet();

      // is grace
      if( !readBuffer(placeHolder, 1) ) { return false; }
      beam->setIsGrace(placeHolder.toBoolean());

      if( !jump(1) ) { return false; }

      // voice
      if( !readBuffer(placeHolder, 1) ) { return false; }
      beam->setVoice(getLowNibble(placeHolder.toUnsignedInt())&0x7);

      // common
      if( !parseCommonBlock(beam) ) { return false; }

      if( !jump(2) ) { return false; }

      // beam count
      if( !readBuffer(placeHolder, 1) ) { return false; }
      int beamCount = placeHolder.toUnsignedInt();

      if( !jump(1) ) { return false; }

      // left line
      if( !readBuffer(placeHolder, 1) ) { return false; }
      beam->getLeftLine()->setLine(placeHolder.toInt());

      // right line
      if( !readBuffer(placeHolder, 1) ) { return false; }
      beam->getRightLine()->setLine(placeHolder.toInt());

      if( ove_->getIsVersion4() ) {
            if( !jump(8) ) { return false; }
            }

      int currentCursor = ove_->getIsVersion4() ? 23 : 13;
      int count = (length - currentCursor)/16;

      if( count != beamCount ) { return false; }

      for( i=0; i<count; ++i ) {
            if( !jump(1) ) { return false; }

            // tuplet
            if( !readBuffer(placeHolder, 1) ) { return false; }
            int tupletCount = placeHolder.toUnsignedInt();
            if( tupletCount > 0 ) {
                  createTuplet = true;
                  tuplet->setTuplet(tupletCount);
                  tuplet->setSpace(tupletToSpace(tupletCount));
                  }

            // start / stop measure
            // line i start end position
            MeasurePos startMp;
            MeasurePos stopMp;

            if( !readBuffer(placeHolder, 1) ) { return false; }
            startMp.setMeasure(placeHolder.toUnsignedInt());
            if( !readBuffer(placeHolder, 1) ) { return false; }
            stopMp.setMeasure(placeHolder.toUnsignedInt());

            if( !readBuffer(placeHolder, 2) ) { return false; }
            startMp.setOffset(placeHolder.toInt());
            if( !readBuffer(placeHolder, 2) ) { return false; }
            stopMp.setOffset(placeHolder.toInt());

            beam->addLine(startMp, stopMp);

            if( stopMp.getOffset() > maxEndUnit ) {
                  maxEndUnit = stopMp.getOffset();
                  }

            if( i == 0 ) {
                  if( !jump(4) ) { return false; }

                  // left offset up+4, down-4
                  if( !readBuffer(placeHolder, 2) ) { return false; }
                  beam->getLeftShoulder()->setYOffset(placeHolder.toInt());

                  // right offset up+4, down-4
                  if( !readBuffer(placeHolder, 2) ) { return false; }
                  beam->getRightShoulder()->setYOffset(placeHolder.toInt());
                  } else {
                  if( !jump(8) ) { return false; }
                  }
            }

      const QList<QPair<MeasurePos, MeasurePos> > lines = beam->getLines();
      MeasurePos offsetMp;

      for( i=0; i<lines.size(); ++i ) {
            if( lines[i].second > offsetMp ) {
                  offsetMp = lines[i].second;
                  }
            }

      beam->stop()->setMeasure(offsetMp.getMeasure());
      beam->stop()->setOffset(offsetMp.getOffset());

      // a case that Tuplet block don't exist, and hide inside beam
      if( createTuplet ) {
            tuplet->copyCommonBlock(*beam);
            tuplet->getLeftLine()->setLine(beam->getLeftLine()->getLine());
            tuplet->getRightLine()->setLine(beam->getRightLine()->getLine());
            tuplet->stop()->setMeasure(beam->stop()->getMeasure());
            tuplet->stop()->setOffset(maxEndUnit);

            measureData->addCrossMeasureElement(tuplet, true);
            } else {
            delete tuplet;
            }

      return true;
      }

bool BarsParse::parseTie(MeasureData* measureData, int /*length*/) {
      Block placeHolder;

      Tie* tie = new Tie();
      measureData->addCrossMeasureElement(tie, true);

      if( !jump(3) ) { return false; }

      // start common
      if( !parseCommonBlock(tie) ) { return false; }

      if( !jump(1) ) { return false; }

      // note
      if( !readBuffer(placeHolder, 1) ) { return false; }
      tie->setNote(placeHolder.toUnsignedInt());

      // pair lines
      if( !parsePairLinesBlock(tie) ) { return false; }

      // offset common
      if( !parseOffsetCommonBlock(tie) ) { return false; }

      // left shoulder offset
      if( !parseOffsetElement(tie->getLeftShoulder()) ) { return false; }

      // right shoulder offset
      if( !parseOffsetElement(tie->getRightShoulder()) ) { return false; }

      // height
      if( !readBuffer(placeHolder, 2) ) { return false; }
      tie->setHeight(placeHolder.toUnsignedInt());

      return true;
      }

bool BarsParse::parseTuplet(MeasureData* measureData, int /*length*/) {
      Block placeHolder;

      Tuplet* tuplet = new Tuplet();
      measureData->addCrossMeasureElement(tuplet, true);

      if( !jump(3) ) { return false; }

      // common
      if( !parseCommonBlock(tuplet) ) { return false; }

      if( !jump(2) ) { return false; }

      // pair lines
      if( !parsePairLinesBlock(tuplet) ) { return false; }

      // offset common
      if( !parseOffsetCommonBlock(tuplet) ) { return false; }

      // left shoulder offset
      if( !parseOffsetElement(tuplet->getLeftShoulder()) ) { return false; }

      // right shoulder offset
      if( !parseOffsetElement(tuplet->getRightShoulder()) ) { return false; }

      if( !jump(2) ) { return false; }

      // height
      if( !readBuffer(placeHolder, 2) ) { return false; }
      tuplet->setHeight(placeHolder.toUnsignedInt());

      // tuplet
      if( !readBuffer(placeHolder, 1) ) { return false; }
      tuplet->setTuplet(placeHolder.toUnsignedInt());

      // space
      if( !readBuffer(placeHolder, 1) ) { return false; }
      tuplet->setSpace(placeHolder.toUnsignedInt());

      // mark offset
      if( !parseOffsetElement(tuplet->getMarkHandle()) ) { return false; }

      return true;
      }

QString binaryToHarmonyType(int bin) {
      QString type = "";
      switch (bin) {
            case 0x0005: { type = "add9(no3)";        break; }
            case 0x0009: { type = "min(no5)";         break; }
            case 0x0011: { type = "(no5)";            break; }
            case 0x0021: { type = "sus(no5)";         break; }
            case 0x0025: { type = "24";               break; }
            case 0x0029: { type = "min4(no5)";        break; }
            case 0x0049: { type = "dim";              break; }
            case 0x0051: { type = "(b5)";             break; }
            case 0x0055: { type = "2#4(no5)";         break; }
            case 0x0081: { type = "(no3)";            break; }
            case 0x0085: { type = "2";                break; }
            case 0x0089: { type = "min";              break; }
            case 0x008D: { type = "min(add9)";        break; }
            case 0x0091: { type = "";                 break; }
            case 0x0093: { type = "addb9";            break; }
            case 0x0095: { type = "add9";             break; }
            case 0x00A1: { type = "sus4";             break; }
            case 0x00A5: { type = "sus(add9)";        break; }
            case 0x00A9: { type = "min4";             break; }
            case 0x00D5: { type = "2#4";              break; }
            case 0x0111: { type = "aug";              break; }
            case 0x0115: { type = "aug(add9)";        break; }
            case 0x0151: { type = "(b5b6)";           break; }
            case 0x0155: { type = "+add9#11";         break; }
            case 0x0189: { type = "minb6";            break; }
            case 0x018D: { type = "min2b6";           break; }
            case 0x0191: { type = "(b6)";             break; }
            case 0x0199: { type = "(add9)b6";         break; }
            case 0x0205: { type = "26";               break; }
            case 0x020D: { type = "min69";            break; }
            case 0x0211: { type = "6";                break; }
            case 0x0215: { type = "69";               break; }
            case 0x022D: { type = "min69 11";         break; }
            case 0x0249: { type = "dim7";             break; }
            case 0x0251: { type = "6#11";             break; }
            case 0x0255: { type = "13#11";            break; }
            case 0x0281: { type = "6(no3)";           break; }
            case 0x0285: { type = "69(no3)";          break; }
            case 0x0289: { type = "min6";             break; }
            case 0x028D: { type = "min69";            break; }
            case 0x0291: { type = "6";                break; }
            case 0x0293: { type = "6b9";              break; }
            case 0x0295: { type = "69";               break; }
            case 0x02AD: { type = "min69 11";         break; }
            case 0x02C5: { type = "69#11(no3)";       break; }
            case 0x02D5: { type = "69#11";            break; }
            case 0x040D: { type = "min9(no5)";        break; }
            case 0x0411: { type = "7(no5)";           break; }
            case 0x0413: { type = "7b9";              break; }
            case 0x0415: { type = "9";                break; }
            case 0x0419: { type = "7#9";              break; }
            case 0x041B: { type = "7b9#9";            break; }
            case 0x0421: { type = "sus7";             break; }
            case 0x0429: { type = "min11";            break; }
            case 0x042D: { type = "min11";            break; }
            case 0x0445: { type = "9b5(no3)";         break; }
            case 0x0449: { type = "min7b5";           break; }
            case 0x044D: { type = "min9b5";           break; }
            case 0x0451: { type = "7b5";              break; }
            case 0x0453: { type = "7b9b5";            break; }
            case 0x0455: { type = "9b5";              break; }
            case 0x045B: { type = "7b5b9#9";          break; }
            case 0x0461: { type = "sus7b5";           break; }
            case 0x0465: { type = "sus9b5";           break; }
            case 0x0469: { type = "min11b5";          break; }
            case 0x046D: { type = "min11b5";          break; }
            case 0x0481: { type = "7(no3)";           break; }
            case 0x0489: { type = "min7";             break; }
            case 0x048D: { type = "min9";             break; }
            case 0x0491: { type = "7";                break; }
            case 0x0493: { type = "7b9";              break; }
            case 0x0495: { type = "9";                break; }
            case 0x0499: { type = "7#9";              break; }
            case 0x049B: { type = "7b9#9";            break; }
            case 0x04A1: { type = "sus7";             break; }
            case 0x04A5: { type = "sus9";             break; }
            case 0x04A9: { type = "min11";            break; }
            case 0x04AD: { type = "min11";            break; }
            case 0x04B5: { type = "11";               break; }
            case 0x04D5: { type = "9#11";             break; }
            case 0x0509: { type = "min7#5";           break; }
            case 0x0511: { type = "aug7";             break; }
            case 0x0513: { type = "7#5b9";            break; }
            case 0x0515: { type = "aug9";             break; }
            case 0x0519: { type = "7#5#9";            break; }
            case 0x0529: { type = "min11b13";         break; }
            case 0x0533: { type = "11b9#5";           break; }
            case 0x0551: { type = "aug7#11";          break; }
            case 0x0553: { type = "7b5b9b13";         break; }
            case 0x0555: { type = "aug9#11";          break; }
            case 0x0559: { type = "aug7#9#11";        break; }
            case 0x0609: { type = "min13";            break; }
            case 0x0611: { type = "13";               break; }
            case 0x0613: { type = "13b9";             break; }
            case 0x0615: { type = "13";               break; }
            case 0x0619: { type = "13#9";             break; }
            case 0x061B: { type = "13b9#9";           break; }
            case 0x0621: { type = "sus13";            break; }
            case 0x062D: { type = "min13(11)";        break; }
            case 0x0633: { type = "13b9add4";         break; }
            case 0x0635: { type = "13";               break; }
            case 0x0645: { type = "13#11(no3)";       break; }
            case 0x0651: { type = "13b5";             break; }
            case 0x0653: { type = "13b9#11";          break; }
            case 0x0655: { type = "13#11";            break; }
            case 0x0659: { type = "13#9b5";           break; }
            case 0x065B: { type = "13b5b9#9";         break; }
            case 0x0685: { type = "13(no3)";          break; }
            case 0x068D: { type = "min13";            break; }
            case 0x0691: { type = "13";               break; }
            case 0x0693: { type = "13b9";             break; }
            case 0x0695: { type = "13";               break; }
            case 0x0699: { type = "13#9";             break; }
            case 0x06A5: { type = "sus13";            break; }
            case 0x06AD: { type = "min13(11)";        break; }
            case 0x06B5: { type = "13";               break; }
            case 0x06D5: { type = "13#11";            break; }
            case 0x0813: { type = "maj7b9";           break; }
            case 0x0851: { type = "maj7#11";          break; }
            case 0x0855: { type = "maj9#11";          break; }
            case 0x0881: { type = "maj7(no3)";        break; }
            case 0x0889: { type = "min(\u266e7)";     break; }   // "min(<sym>accidentalNatural</sym>7)"
            case 0x088D: { type = "min9(\u266e7)";    break; }   // "min9(<sym>accidentalNatural</sym>7)"
            case 0x0891: { type = "maj7";             break; }
            case 0x0895: { type = "maj9";             break; }
            case 0x08C9: { type = "dim7(add maj 7)";  break; }
            case 0x08D1: { type = "maj7#11";          break; }
            case 0x08D5: { type = "maj9#11";          break; }
            case 0x0911: { type = "maj7#5";           break; }
            case 0x0991: { type = "maj7#5";           break; }
            case 0x0995: { type = "maj9#5";            break; }
            case 0x0A0D: { type = "min69(\u266e7)";   break; }   // "min69(<sym>accidentalNatural</sym>7)"
            case 0x0A11: { type = "maj13";            break; }
            case 0x0A15: { type = "maj13";            break; }
            case 0x0A51: { type = "maj13#11";         break; }
            case 0x0A55: { type = "maj13#11";         break; }
            case 0x0A85: { type = "maj13(no3)";       break; }
            case 0x0A89: { type = "min13(\u266e7)";   break; }   // "min13(<sym>accidentalNatural</sym>7)"
            case 0x0A8D: { type = "min69(\u266e7)";   break; }   // "min69(<sym>accidentalNatural</sym>7)"
            case 0x0A91: { type = "maj13";            break; }
            case 0x0A95: { type = "maj13";            break; }
            case 0x0AAD: { type = "min13(\u266e7)";   break; }   // "min13(<sym>accidentalNatural</sym>7)"
            case 0x0AD5: { type = "maj13#11";         break; }
            case 0x0B45: { type = "maj13#5#11(no4)";  break; }
            default: {
                  qDebug("Unrecognized harmony type: %04X",bin);
                  type = "";
                  break;
                  }
            }
      return type;
      }

bool BarsParse::parseHarmony(MeasureData* measureData, int /*length*/) {
      Block placeHolder;

      Harmony* harmony = new Harmony();
      measureData->addMusicData(harmony);

      if( !jump(3) ) { return false; }

      // common
      if( !parseCommonBlock(harmony) ) { return false; }

      // bass on bottom
      if( !readBuffer(placeHolder, 1) ) { return false; }
      harmony->setBassOnBottom((getHighNibble(placeHolder.toUnsignedInt()) & 0x4));

      // root alteration
      switch (placeHolder.toUnsignedInt() & 0x18) {
            case 0: {
                  harmony->setAlterRoot(0); // natural
                  break;
                  }
            case 16: {
                  harmony->setAlterRoot(-1); // flat
                  break;
                  }
            case 8: {
                  harmony->setAlterRoot(1); // sharp
                  break;
                  }
            default: {
                  harmony->setAlterRoot(0);
                  break;
                  }
            }

      // bass alteration
      switch (placeHolder.toUnsignedInt() & 0x3) {
            case 0: {
                  harmony->setAlterBass(0); // natural
                  break;
                  }
            case 2: {
                  harmony->setAlterBass(-1); // flat
                  break;
                  }
            case 1: {
                  harmony->setAlterBass(1); // sharp
                  break;
                  }
            default: {
                  harmony->setAlterBass(0);
                  break;
                  }
            }

      // show bass
      bool useBass = placeHolder.toUnsignedInt() & 0x80;

      if( !jump(1) ) { return false; }

      // y offset
      if( !readBuffer(placeHolder, 2) ) { return false; }
      harmony->setYOffset(placeHolder.toInt());

      // harmony type
      if( !readBuffer(placeHolder, 2) ) { return false; }
      harmony->setHarmonyType(binaryToHarmonyType(placeHolder.toUnsignedInt()));

      // root
      if( !readBuffer(placeHolder, 1) ) { return false; }
      harmony->setRoot(placeHolder.toInt());

      // bass
      if( !readBuffer(placeHolder, 1) ) { return false; }
      if (useBass)
            harmony->setBass(placeHolder.toInt());

      // angle
      if( !readBuffer(placeHolder, 2) ) { return false; }
      harmony->setAngle(placeHolder.toInt());

      if( ove_->getIsVersion4() ) {
            // length (tick)
            if( !readBuffer(placeHolder, 2) ) { return false; }
            harmony->setLength(placeHolder.toUnsignedInt());

            if( !jump(4) ) { return false; }
            }

      return true;
      }

bool BarsParse::parseClef(MeasureData* measureData, int /*length*/) {
      Block placeHolder;

      Clef* clef = new Clef();
      measureData->addMusicData(clef);

      if( !jump(3) ) { return false; }

      // common
      if( !parseCommonBlock(clef) ) { return false; }

      // clef type
      if( !readBuffer(placeHolder, 1) ) { return false; }
      clef->setClefType(placeHolder.toUnsignedInt());

      // line
      if( !readBuffer(placeHolder, 1) ) { return false; }
      clef->setLine(placeHolder.toInt());

      if( !jump(2) ) { return false; }

      return true;
      }

bool BarsParse::parseLyric(MeasureData* measureData, int length) {
      Block placeHolder;

      Lyric* lyric = new Lyric();
      measureData->addMusicData(lyric);

      if( !jump(3) ) { return false; }

      // common
      if( !parseCommonBlock(lyric) ) { return false; }

      if( !jump(2) ) { return false; }

      // offset
      if( !parseOffsetElement(lyric) ) { return false; }

      if( !jump(7) ) { return false; }

      // verse
      if( !readBuffer(placeHolder, 1) ) { return false; }
      lyric->setVerse(placeHolder.toUnsignedInt());

      if( ove_->getIsVersion4() ) {
            if( !jump(6) ) { return false; }

            // lyric
            if( length > 29 ) {
                  if( !readBuffer(placeHolder, length-29) ) { return false; }
                  lyric->setLyric(ove_->getCodecString(placeHolder.fixedSizeBufferToStrByteArray()));
                  }
            }

      return true;
      }

bool BarsParse::parseSlur(MeasureData* measureData, int /*length*/) {
      Block placeHolder;

      Slur* slur = new Slur();
      measureData->addCrossMeasureElement(slur, true);

      if( !jump(2) ) { return false; }

      // voice
      if( !readBuffer(placeHolder, 1) ) { return false; }
      slur->setVoice(getLowNibble(placeHolder.toUnsignedInt())&0x7);

      // common
      if( !parseCommonBlock(slur) ) { return false; }

      // show on top
      if( !readBuffer(placeHolder, 1) ) { return false; }
      slur->setShowOnTop(getHighNibble(placeHolder.toUnsignedInt())==0x8);

      if( !jump(1) ) { return false; }

      // pair lines
      if( !parsePairLinesBlock(slur) ) { return false; }

      // offset common
      if( !parseOffsetCommonBlock(slur) ) { return false; }

      // handle 1
      if( !parseOffsetElement(slur->getLeftShoulder()) ) { return false; }

      // handle 4
      if( !parseOffsetElement(slur->getRightShoulder()) ) { return false; }

      // handle 2
      if( !parseOffsetElement(slur->getHandle2()) ) { return false; }

      // handle 3
      if( !parseOffsetElement(slur->getHandle3()) ) { return false; }

      if( ove_->getIsVersion4() ) {
            if( !jump(3) ) { return false; }

            // note time percent
            if( !readBuffer(placeHolder, 1) ) { return false; }
            slur->setNoteTimePercent(placeHolder.toUnsignedInt());

            if( !jump(36) ) { return false; }
            }

      return true;
      }

bool BarsParse::parseGlissando(MeasureData* measureData, int /*length*/) {
      Block placeHolder;

      Glissando* glissando = new Glissando();
      measureData->addCrossMeasureElement(glissando, true);

      if( !jump(3) ) { return false; }

      // common
      if( !parseCommonBlock(glissando) ) { return false; }

      // straight or wavy
      if( !readBuffer(placeHolder, 1) ) { return false; }
      unsigned int thisByte = placeHolder.toUnsignedInt();
      glissando->setStraightWavy(getHighNibble(thisByte)==4);

      if( !jump(1) ) { return false; }

      // pair lines
      if( !parsePairLinesBlock(glissando) ) { return false; }

      // offset common
      if( !parseOffsetCommonBlock(glissando) ) { return false; }

      // left shoulder
      if( !parseOffsetElement(glissando->getLeftShoulder()) ) { return false; }

      // right shoulder
      if( !parseOffsetElement(glissando->getRightShoulder()) ) { return false; }

      if( ove_->getIsVersion4() ) {
            if( !jump(1) ) { return false; }

            // line thick
            if( !readBuffer(placeHolder, 1) ) { return false; }
            glissando->setLineThick(placeHolder.toUnsignedInt());

            if( !jump(12) ) { return false; }

            // text 32 bytes
            if( !readBuffer(placeHolder, 32) ) { return false; }
            glissando->setText(ove_->getCodecString(placeHolder.fixedSizeBufferToStrByteArray()));

            if( !jump(6) ) { return false; }
            }

      return true;
      }

bool getDecoratorType(
            unsigned int thisByte,
            bool& measureRepeat,
            Decorator::Type& decoratorType,
            bool& singleRepeat,
            ArticulationType& artType) {
      measureRepeat = false;
      decoratorType = Decorator::Type::Articulation;
      singleRepeat = true;
      artType = ArticulationType::None;

      switch (thisByte) {
            case 0x00: {
                  decoratorType = Decorator::Type::Dotted_Barline;
                  break;
                  }
            case 0x30: {
                  artType = ArticulationType::Open_String;
                  break;
                  }
            case 0x31: {
                  artType = ArticulationType::Finger_1;
                  break;
                  }
            case 0x32: {
                  artType = ArticulationType::Finger_2;
                  break;
                  }
            case 0x33: {
                  artType = ArticulationType::Finger_3;
                  break;
                  }
            case 0x34: {
                  artType = ArticulationType::Finger_4;
                  break;
                  }
            case 0x35: {
                  artType = ArticulationType::Finger_5;
                  break;
                  }
            case 0x6B: {
                  artType = ArticulationType::Flat_Accidental_For_Trill;
                  break;
                  }
            case 0x6C: {
                  artType = ArticulationType::Sharp_Accidental_For_Trill;
                  break;
                  }
            case 0x6D: {
                  artType = ArticulationType::Natural_Accidental_For_Trill;
                  break;
                  }
            case 0x8d: {
                  measureRepeat = true;
                  singleRepeat = true;
                  break;
                  }
            case 0x8e: {
                  measureRepeat = true;
                  singleRepeat = false;
                  break;
                  }
            case 0xA0: {
                  artType = ArticulationType::Minor_Trill;
                  break;
                  }
            case 0xA1: {
                  artType = ArticulationType::Major_Trill;
                  break;
                  }
            case 0xA2: {
                  artType = ArticulationType::Trill_Section;
                  break;
                  }
            case 0xA6: {
                  artType = ArticulationType::Turn;
                  break;
                  }
            case 0xA8: {
                  artType = ArticulationType::Tremolo_Eighth;
                  break;
                  }
            case 0xA9: {
                  artType = ArticulationType::Tremolo_Sixteenth;
                  break;
                  }
            case 0xAA: {
                  artType = ArticulationType::Tremolo_Thirty_Second;
                  break;
                  }
            case 0xAB: {
                  artType = ArticulationType::Tremolo_Sixty_Fourth;
                  break;
                  }
            case 0xB2: {
                  artType = ArticulationType::Fermata;
                  break;
                  }
            case 0xB3: {
                  artType = ArticulationType::Fermata_Inverted;
                  break;
                  }
            case 0xB9: {
                  artType = ArticulationType::Pause;
                  break;
                  }
            case 0xBA: {
                  artType = ArticulationType::Grand_Pause;
                  break;
                  }
            case 0xC0: {
                  artType = ArticulationType::Marcato;
                  break;
                  }
            case 0xC1: {
                  artType = ArticulationType::Marcato_Dot;
                  break;
                  }
            case 0xC2: {
                  artType = ArticulationType::SForzando;
                  break;
                  }
            case 0xC3: {
                  artType = ArticulationType::SForzando_Dot;
                  break;
                  }
            case 0xC4: {
                  artType = ArticulationType::SForzando_Inverted;
                  break;
                  }
            case 0xC5: {
                  artType = ArticulationType::SForzando_Dot_Inverted;
                  break;
                  }
            case 0xC6: {
                  artType = ArticulationType::Staccatissimo;
                  break;
                  }
            case 0xC7: {
                  artType = ArticulationType::Staccato;
                  break;
                  }
            case 0xC8: {
                  artType = ArticulationType::Tenuto;
                  break;
                  }
            case 0xC9: {
                  artType = ArticulationType::Natural_Harmonic;
                  break;
                  }
            case 0xCA: {
                  artType = ArticulationType::Artificial_Harmonic;
                  break;
                  }
            case 0xCB: {
                  artType = ArticulationType::Plus_Sign;
                  break;
                  }
            case 0xCC: {
                  artType = ArticulationType::Up_Bow;
                  break;
                  }
            case 0xCD: {
                  artType = ArticulationType::Down_Bow;
                  break;
                  }
            case 0xCE: {
                  artType = ArticulationType::Up_Bow_Inverted;
                  break;
                  }
            case 0xCF: {
                  artType = ArticulationType::Down_Bow_Inverted;
                  break;
                  }
            case 0xD0: {
                  artType = ArticulationType::Pedal_Down;
                  break;
                  }
            case 0xD1: {
                  artType = ArticulationType::Pedal_Up;
                  break;
                  }
            case 0xD6: {
                  artType = ArticulationType::Heavy_Attack;
                  break;
                  }
            case 0xD7: {
                  artType = ArticulationType::Heavier_Attack;
                  break;
                  }
            default:
                  return false;
                  break;
            }

      return true;
      }

bool BarsParse::parseDecorators(MeasureData* measureData, int length) {
      Block placeHolder;
      MusicData* musicData = new MusicData();

      if( !jump(3) ) { return false; }

      // common
      if( !parseCommonBlock(musicData) ) { return false; }

      if( !jump(2) ) { return false; }

      // y offset
      if( !readBuffer(placeHolder, 2) ) { return false; }
      musicData->setYOffset(placeHolder.toInt());

      if( !jump(2) ) { return false; }

      // measure repeat | piano pedal | dotted barline | articulation
      if( !readBuffer(placeHolder, 1) ) { return false; }
      unsigned int thisByte = placeHolder.toUnsignedInt();

      Decorator::Type decoratorType;
      bool isMeasureRepeat;
      bool isSingleRepeat = true;
      ArticulationType artType = ArticulationType::None;

      getDecoratorType(thisByte, isMeasureRepeat, decoratorType, isSingleRepeat, artType);

      if( isMeasureRepeat ) {
            MeasureRepeat* measureRepeat = new MeasureRepeat();
            measureData->addCrossMeasureElement(measureRepeat, true);

            measureRepeat->copyCommonBlock(*musicData);
            measureRepeat->setYOffset(musicData->getYOffset());

            measureRepeat->setSingleRepeat(isSingleRepeat);
            } else {
            Decorator* decorator = new Decorator();
            measureData->addMusicData(decorator);

            decorator->copyCommonBlock(*musicData);
            decorator->setYOffset(musicData->getYOffset());

            decorator->setDecoratorType(decoratorType);
            decorator->setArticulationType(artType);
            }

      int cursor = ove_->getIsVersion4() ? 16 : 14;
      if( !jump(length-cursor) ) { return false; }

      return true;
      }

bool BarsParse::parseWedge(MeasureData* measureData, int length) {
      Block placeHolder;
      Wedge* wedge = new Wedge();

      if( !jump(3) ) { return false; }

      // common
      if( !parseCommonBlock(wedge) ) { return false; }

      // wedge type
      if( !readBuffer(placeHolder, 1) ) { return false; }
      WedgeType wedgeType = WedgeType::Cres_Line;
      bool wedgeOrExpression = true;
      unsigned int highHalfByte = getHighNibble(placeHolder.toUnsignedInt());
      unsigned int lowHalfByte = getLowNibble(placeHolder.toUnsignedInt());

      switch (highHalfByte) {
            case 0x0: {
                  wedgeType = WedgeType::Cres_Line;
                  wedgeOrExpression = true;
                  break;
                  }
            case 0x4: {
                  wedgeType = WedgeType::Decresc_Line;
                  wedgeOrExpression = true;
                  break;
                  }
            case 0x6: {
                  wedgeType = WedgeType::Decresc;
                  wedgeOrExpression = false;
                  break;
                  }
            case 0x2: {
                  wedgeType = WedgeType::Cres;
                  wedgeOrExpression = false;
                  break;
                  }
            default:
                  break;
            }

      // 0xb | 0x8(ove3) , else 3, 0(ove3)
      if( (lowHalfByte & 0x8) == 0x8 ) {
            wedgeType = WedgeType::Double_Line;
            wedgeOrExpression = true;
            }

      if( !jump(1) ) { return false; }

      // y offset
      if( !readBuffer(placeHolder, 2) ) { return false; }
      wedge->setYOffset(placeHolder.toInt());

      // wedge
      if( wedgeOrExpression ) {
            measureData->addCrossMeasureElement(wedge, true);
            wedge->setWedgeType(wedgeType);

            if( !jump(2) ) { return false; }

            // height
            if( !readBuffer(placeHolder, 2) ) { return false; }
            wedge->setHeight(placeHolder.toUnsignedInt());

            // offset common
            if( !parseOffsetCommonBlock(wedge) ) { return false; }

            int cursor = ove_->getIsVersion4() ? 21 : 19;
            if( !jump(length-cursor) ) { return false; }
            }
      // expression : cresc, decresc
      else {
            Expressions* express = new Expressions();
            measureData->addMusicData(express);

            express->copyCommonBlock(*wedge);
            express->setYOffset(wedge->getYOffset());

            if( !jump(4) ) { return false; }

            // offset common
            if( !parseOffsetCommonBlock(express) ) { return false; }

            if( ove_->getIsVersion4() ) {
                  if( !jump(18) ) { return false; }

                  // words
                  if( length > 39 ) {
                        if( !readBuffer(placeHolder, length-39) ) { return false; }
                        express->setText(ove_->getCodecString(placeHolder.fixedSizeBufferToStrByteArray()));
                        }
                  } else {
                  QString str = wedgeType==WedgeType::Cres ? "cresc" : "decresc";
                  express->setText(str);

                  if( !jump(8) ) { return false; }
                  }
            }

      return true;
      }

bool BarsParse::parseDynamics(MeasureData* measureData, int /*length*/) {
      Block placeHolder;

      Dynamics* dynamics = new Dynamics();
      measureData->addMusicData(dynamics);

      if( !jump(1) ) { return false; }

      // is playback
      if( !readBuffer(placeHolder, 1) ) { return false; }
      dynamics->setIsPlayback(getHighNibble(placeHolder.toUnsignedInt())!=0x4);

      if( !jump(1) ) { return false; }

      // common
      if( !parseCommonBlock(dynamics) ) { return false; }

      // y offset
      if( !readBuffer(placeHolder, 2) ) { return false; }
      dynamics->setYOffset(placeHolder.toInt());

      // dynamics type
      if( !readBuffer(placeHolder, 1) ) { return false; }
      dynamics->setDynamicsType(getLowNibble(placeHolder.toUnsignedInt()));

      // velocity
      if( !readBuffer(placeHolder, 1) ) { return false; }
      dynamics->setVelocity(placeHolder.toUnsignedInt());

      int cursor = ove_->getIsVersion4() ? 4 : 2;

      if( !jump(cursor) ) { return false; }

      return true;
      }

bool BarsParse::parseKey(MeasureData* measureData, int /*length*/) {
      Block placeHolder;
      Key* key = measureData->getKey();
      int cursor = ove_->getIsVersion4() ? 9 : 7;

      if( !jump(cursor) ) { return false; }

      // key
      if( !readBuffer(placeHolder, 1) ) { return false; }
      key->setKey(oveKeyToKey(placeHolder.toUnsignedInt()));

      // previous key
      if( !readBuffer(placeHolder, 1) ) { return false; }
      key->setPreviousKey(oveKeyToKey(placeHolder.toUnsignedInt()));

      if( !jump(3) ) { return false; }

      // symbol count
      if( !readBuffer(placeHolder, 1) ) { return false; }
      key->setSymbolCount(placeHolder.toUnsignedInt());

      if( !jump(4) ) { return false; }

      return true;
      }

bool BarsParse::parsePedal(MeasureData* measureData, int length) {
      Block placeHolder;

      Pedal* pedal = new Pedal();
      //measureData->addMusicData(pedal); //can't remember why
      measureData->addCrossMeasureElement(pedal, true);

      if( !jump(1) ) { return false; }

      // is playback
      if( !readBuffer(placeHolder, 1) ) { return false; }
      pedal->setIsPlayback(getHighNibble(placeHolder.toUnsignedInt())!=4);

      if( !jump(1) ) { return false; }

      // common
      if( !parseCommonBlock(pedal) ) { return false; }

      if( !jump(2) ) { return false; }

      // pair lines
      if( !parsePairLinesBlock(pedal) ) { return false; }

      // offset common
      if( !parseOffsetCommonBlock(pedal) ) { return false; }

      // left shoulder
      if( !parseOffsetElement(pedal->getLeftShoulder()) ) { return false; }

      // right shoulder
      if( !parseOffsetElement(pedal->getRightShoulder()) ) { return false; }

      int cursor = ove_->getIsVersion4() ? 0x45 : 0x23;
      int blankCount = ove_->getIsVersion4() ? 42 : 10;

      pedal->setHalf( length > cursor );

      if( !jump(blankCount) ) { return false; }

      if( length > cursor ) {
            if( !jump(2) ) { return false; }

            // handle x offset
            if( !readBuffer(placeHolder, 2) ) { return false; }
            pedal->getPedalHandle()->setXOffset(placeHolder.toInt());

            if( !jump(6) ) { return false; }
            }

      return true;
      }

bool BarsParse::parseKuohao(MeasureData* measureData, int /*length*/) {
      Block placeHolder;

      KuoHao* kuoHao = new KuoHao();
      measureData->addMusicData(kuoHao);

      if( !jump(3) ) { return false; }

      // common
      if( !parseCommonBlock(kuoHao) ) { return false; }

      if( !jump(2) ) { return false; }

      // pair lines
      if( !parsePairLinesBlock(kuoHao) ) { return false; }

      if( !jump(4) ) { return false; }

      // left shoulder
      if( !parseOffsetElement(kuoHao->getLeftShoulder()) ) { return false; }

      // right shoulder
      if( !parseOffsetElement(kuoHao->getRightShoulder()) ) { return false; }

      // kuohao type
      if( !readBuffer(placeHolder, 1) ) { return false; }
      kuoHao->setKuohaoType(placeHolder.toUnsignedInt());

      // height
      if( !readBuffer(placeHolder, 1) ) { return false; }
      kuoHao->setHeight(placeHolder.toUnsignedInt());

      int jumpAmount = ove_->getIsVersion4() ? 40 : 8;
      if( !jump(jumpAmount) ) { return false; }

      return true;
      }

bool BarsParse::parseExpressions(MeasureData* measureData, int length) {
      Block placeHolder;

      Expressions* expressions = new Expressions();
      measureData->addMusicData(expressions);

      if( !jump(3) ) { return false; }

      // common00
      if( !parseCommonBlock(expressions) ) { return false; }

      if( !jump(2) ) { return false; }

      // y offset
      if( !readBuffer(placeHolder, 2) ) { return false; }
      expressions->setYOffset(placeHolder.toInt());

      // range bar offset
      if( !readBuffer(placeHolder, 2) ) { return false; }
      //int barOffset = placeHolder.toUnsignedInt();

      if( !jump(10) ) { return false; }

      // tempo 1
      if( !readBuffer(placeHolder, 2) ) { return false; }
      //double tempo1 = ((double)placeHolder.toUnsignedInt()) / 100.0;

      // tempo 2
      if( !readBuffer(placeHolder, 2) ) { return false; }
      //double tempo2 = ((double)placeHolder.toUnsignedInt()) / 100.0;

      if( !jump(6) ) { return false; }

      // text
      int cursor = ove_->getIsVersion4() ? 35 : 33;
      if( length > cursor ) {
            if( !readBuffer(placeHolder, length-cursor) ) { return false; }
            expressions->setText(ove_->getCodecString(placeHolder.fixedSizeBufferToStrByteArray()));
            }

      return true;
      }

bool BarsParse::parseHarpPedal(MeasureData* measureData, int /*length*/) {
      Block placeHolder;

      HarpPedal* harpPedal = new HarpPedal();
      measureData->addMusicData(harpPedal);

      if( !jump(3) ) { return false; }

      // common
      if( !parseCommonBlock(harpPedal) ) { return false; }

      if( !jump(2) ) { return false; }

      // y offset
      if( !readBuffer(placeHolder, 2) ) { return false; }
      harpPedal->setYOffset(placeHolder.toInt());

      // show type
      if( !readBuffer(placeHolder, 1) ) { return false; }
      harpPedal->setShowType(placeHolder.toUnsignedInt());

      // show char flag
      if( !readBuffer(placeHolder, 1) ) { return false; }
      harpPedal->setShowCharFlag(placeHolder.toUnsignedInt());

      if( !jump(8) ) { return false; }

      return true;
      }

bool BarsParse::parseMultiMeasureRest(MeasureData* measureData, int /*length*/) {
      Block placeHolder(2);
      MultiMeasureRest* measureRest = new MultiMeasureRest();
      measureData->addMusicData(measureRest);

      if( !jump(3) ) { return false; }

      // common
      if( !parseCommonBlock(measureRest) ) { return false; }

      if( !jump(6) ) { return false; }

      return true;
      }

bool BarsParse::parseHarmonyGuitarFrame(MeasureData* measureData, int length) {
      Block placeHolder;

      Harmony* harmony = new Harmony();
      measureData->addMusicData(harmony);

      if( !jump(3) ) { return false; }

      // common
      if( !parseCommonBlock(harmony) ) { return false; }

      // root
      if( !readBuffer(placeHolder, 1) ) { return false; }
      harmony->setRoot(placeHolder.toUnsignedInt());

      // type
      if( !readBuffer(placeHolder, 1) ) { return false; }
      //harmony->setHarmonyType((HarmonyType)placeHolder.toUnsignedInt()); // TODO

      // bass
      if( !readBuffer(placeHolder, 1) ) { return false; }
      harmony->setBass(placeHolder.toUnsignedInt());

      int jumpAmount = ove_->getIsVersion4() ? length - 12 : length - 10;
      if( !jump(jumpAmount) ) { return false; }

      return true;
      }

void extractOctave(unsigned int Bits, OctaveShiftType& octaveShiftType, QList<OctaveShiftPosition>& positions) {
      octaveShiftType = OctaveShiftType::OS_8;
      positions.clear();

      switch (Bits) {
            case 0x0: {
                  octaveShiftType = OctaveShiftType::OS_8;
                  positions.push_back(OctaveShiftPosition::Continue);
                  break;
                  }
            case 0x1: {
                  octaveShiftType = OctaveShiftType::OS_Minus_8;
                  positions.push_back(OctaveShiftPosition::Continue);
                  break;
                  }
            case 0x2: {
                  octaveShiftType = OctaveShiftType::OS_15;
                  positions.push_back(OctaveShiftPosition::Continue);
                  break;
                  }
            case 0x3: {
                  octaveShiftType = OctaveShiftType::OS_Minus_15;
                  positions.push_back(OctaveShiftPosition::Continue);
                  break;
                  }
            case 0x4: {
                  octaveShiftType = OctaveShiftType::OS_8;
                  positions.push_back(OctaveShiftPosition::Stop);
                  break;
                  }
            case 0x5: {
                  octaveShiftType = OctaveShiftType::OS_Minus_8;
                  positions.push_back(OctaveShiftPosition::Stop);
                  break;
                  }
            case 0x6: {
                  octaveShiftType = OctaveShiftType::OS_15;
                  positions.push_back(OctaveShiftPosition::Stop);
                  break;
                  }
            case 0x7: {
                  octaveShiftType = OctaveShiftType::OS_Minus_15;
                  positions.push_back(OctaveShiftPosition::Stop);
                  break;
                  }
            case 0x8: {
                  octaveShiftType = OctaveShiftType::OS_8;
                  positions.push_back(OctaveShiftPosition::Start);
                  break;
                  }
            case 0x9: {
                  octaveShiftType = OctaveShiftType::OS_Minus_8;
                  positions.push_back(OctaveShiftPosition::Start);
                  break;
                  }
            case 0xA: {
                  octaveShiftType = OctaveShiftType::OS_15;
                  positions.push_back(OctaveShiftPosition::Start);
                  break;
                  }
            case 0xB: {
                  octaveShiftType = OctaveShiftType::OS_Minus_15;
                  positions.push_back(OctaveShiftPosition::Start);
                  ;
                  break;
                  }
            case 0xC: {
                  octaveShiftType = OctaveShiftType::OS_8;
                  positions.push_back(OctaveShiftPosition::Start);
                  positions.push_back(OctaveShiftPosition::Stop);
                  break;
                  }
            case 0xD: {
                  octaveShiftType = OctaveShiftType::OS_Minus_8;
                  positions.push_back(OctaveShiftPosition::Start);
                  positions.push_back(OctaveShiftPosition::Stop);
                  break;
                  }
            case 0xE: {
                  octaveShiftType = OctaveShiftType::OS_15;
                  positions.push_back(OctaveShiftPosition::Start);
                  positions.push_back(OctaveShiftPosition::Stop);
                  break;
                  }
            case 0xF: {
                  octaveShiftType = OctaveShiftType::OS_Minus_15;
                  positions.push_back(OctaveShiftPosition::Start);
                  positions.push_back(OctaveShiftPosition::Stop);
                  break;
                  }
            default:
                  break;
            }
      }

bool BarsParse::parseOctaveShift(MeasureData* measureData, int /*length*/) {
      Block placeHolder;

      OctaveShift* octave = new OctaveShift();
      measureData->addCrossMeasureElement(octave, true);

      if( !jump(3) ) { return false; }

      // common
      if( !parseCommonBlock(octave) ) { return false; }

      // octave
      if( !readBuffer(placeHolder, 1) ) { return false; }
      unsigned int type = getLowNibble(placeHolder.toUnsignedInt());
      OctaveShiftType octaveShiftType = OctaveShiftType::OS_8;
      QList<OctaveShiftPosition> positions;
      extractOctave(type, octaveShiftType, positions);

      octave->setOctaveShiftType(octaveShiftType);

      if( !jump(1) ) { return false; }

      // y offset
      if( !readBuffer(placeHolder, 2) ) { return false; }
      octave->setYOffset(placeHolder.toInt());

      if( !jump(4) ) { return false; }

      // length
      if( !readBuffer(placeHolder, 2) ) { return false; }
      octave->setLength(placeHolder.toUnsignedInt());

      // end tick
      if( !readBuffer(placeHolder, 2) ) { return false; }
      octave->setEndTick(placeHolder.toUnsignedInt());

      // start & stop maybe appear in same measure
      for (int i=0; i<positions.size(); ++i) {
            OctaveShiftPosition position = positions[i];
            OctaveShiftEndPoint* octavePoint = new OctaveShiftEndPoint();
            measureData->addMusicData(octavePoint);

            octavePoint->copyCommonBlock(*octave);
            octavePoint->setOctaveShiftType(octaveShiftType);
            octavePoint->setOctaveShiftPosition(position);
            octavePoint->setEndTick(octave->getEndTick());

            // stop
            if( i==0 && position == OctaveShiftPosition::Stop ) {
                  octavePoint->start()->setOffset(octave->start()->getOffset()+octave->getLength());
                  }

            // end point
            if( i>0 ) {
                  octavePoint->start()->setOffset(octave->start()->getOffset()+octave->getLength());
                  octavePoint->setTick(octave->getEndTick());
                  }
            }

      return true;
      }

bool BarsParse::parseMidiController(MeasureData* measureData, int /*length*/) {
      Block placeHolder;
      MidiController* controller = new MidiController();
      measureData->addMidiData(controller);

      parseMidiCommon(controller);

      // value [0, 128)
      if( !readBuffer(placeHolder, 1) ) { return false; }
      controller->setValue(placeHolder.toUnsignedInt());

      // controller number
      if( !readBuffer(placeHolder, 1) ) { return false; }
      controller->setController(placeHolder.toUnsignedInt());

      if( ove_->getIsVersion4() ) {
            if( !jump(2) ) { return false; }
            }

      return true;
      }

bool BarsParse::parseMidiProgramChange(MeasureData* measureData, int /*length*/) {
      Block placeHolder;
      MidiProgramChange* program = new MidiProgramChange();
      measureData->addMidiData(program);

      parseMidiCommon(program);

      if( !jump(1) ) { return false; }

      // patch
      if( !readBuffer(placeHolder, 1) ) { return false; }
      program->setPatch(placeHolder.toUnsignedInt());

      if( ove_->getIsVersion4() ) {
            if( !jump(2) ) { return false; }
            }

      return true;
      }

bool BarsParse::parseMidiChannelPressure(MeasureData* measureData, int /*length*/) {
      Block placeHolder;
      MidiChannelPressure* pressure = new MidiChannelPressure();
      measureData->addMidiData(pressure);

      parseMidiCommon(pressure);

      if( !jump(1) ) { return false; }

      // pressure
      if( !readBuffer(placeHolder, 1) ) { return false; }
      pressure->setPressure(placeHolder.toUnsignedInt());

      if( ove_->getIsVersion4() )
            {
            if( !jump(2) ) { return false; }
            }

      return true;
      }

bool BarsParse::parseMidiPitchWheel(MeasureData* measureData, int /*length*/) {
      Block placeHolder;
      MidiPitchWheel* wheel = new MidiPitchWheel();
      measureData->addMidiData(wheel);

      parseMidiCommon(wheel);

      // pitch wheel
      if( !readBuffer(placeHolder, 2) ) { return false; }
      int value = placeHolder.toUnsignedInt();
      wheel->setValue(value);

      if( ove_->getIsVersion4() ) {
            if( !jump(2) ) { return false; }
            }

      return true;
      }

bool BarsParse::parseSizeBlock(int length) {
      if( !jump(length) ) { return false; }

      return true;
      }

bool BarsParse::parseMidiCommon(MidiData* ptr) {
      Block placeHolder;

      if( !jump(3) ) { return false; }

      // start position
      if( !readBuffer(placeHolder, 2) ) { return false; }
      ptr->setTick(placeHolder.toUnsignedInt());

      return true;
      }

bool BarsParse::parseCommonBlock(MusicData* ptr) {
      Block placeHolder;

      // start tick
      if( !readBuffer(placeHolder, 2) ) { return false; }
      ptr->setTick(placeHolder.toInt());

      // start unit
      if( !readBuffer(placeHolder, 2) ) { return false; }
      ptr->start()->setOffset(placeHolder.toInt());

      if( ove_->getIsVersion4() ) {
            // color
            if( !readBuffer(placeHolder, 1) ) { return false; }
            ptr->setColor(placeHolder.toUnsignedInt());

            if( !jump(1) ) { return false; }
            }

      return true;
      }

bool BarsParse::parseOffsetCommonBlock(MusicData* ptr) {
      Block placeHolder;

      // offset measure
      if( !readBuffer(placeHolder, 2) ) { return false; }
      ptr->stop()->setMeasure(placeHolder.toUnsignedInt());

      // end unit
      if( !readBuffer(placeHolder, 2) ) { return false; }
      ptr->stop()->setOffset(placeHolder.toInt());

      return true;
      }

bool BarsParse::parsePairLinesBlock(PairEnds* ptr) {
      Block placeHolder;

      // left line
      if( !readBuffer(placeHolder, 2) ) { return false; }
      ptr->getLeftLine()->setLine(placeHolder.toInt());

      // right line
      if( !readBuffer(placeHolder, 2) ) { return false; }
      ptr->getRightLine()->setLine(placeHolder.toInt());

      return true;
      }

bool BarsParse::parseOffsetElement(OffsetElement* ptr) {
      Block placeHolder;

      // x offset
      if( !readBuffer(placeHolder, 2) ) { return false; }
      ptr->setXOffset(placeHolder.toInt());

      // y offset
      if( !readBuffer(placeHolder, 2) ) { return false; }
      ptr->setYOffset(placeHolder.toInt());

      return true;
      }

bool BarsParse::getCondElementType(unsigned int byteData, CondType& type) {
      if( byteData == 0x09 ) {
            type = CondType::Time_Parameters;
            } else if (byteData == 0x0A) {
            type = CondType::Bar_Number;
            } else if (byteData == 0x16) {
            type = CondType::Decorator;
            } else if (byteData == 0x1C) {
            type = CondType::Tempo;
            } else if (byteData == 0x1D) {
            type = CondType::Text;
            } else if (byteData == 0x25) {
            type = CondType::Expression;
            } else if (byteData == 0x30) {
            type = CondType::Barline_Parameters;
            } else if (byteData == 0x31) {
            type = CondType::Repeat;
            } else if (byteData == 0x32) {
            type = CondType::Numeric_Ending;
            } else {
            return false;
            }

      return true;
      }

bool BarsParse::getBdatElementType(unsigned int byteData, BdatType& type) {
      if (byteData == 0x70) {
            type = BdatType::Raw_Note;
            } else if (byteData == 0x80) {
            type = BdatType::Rest;
            } else if (byteData == 0x90) {
            type = BdatType::Note;
            } else if (byteData == 0x10) {
            type = BdatType::Beam;
            } else if (byteData == 0x11) {
            type = BdatType::Harmony;
            } else if (byteData == 0x12) {
            type = BdatType::Clef;
            } else if (byteData == 0x13) {
            type = BdatType::Wedge;
            } else if (byteData == 0x14) {
            type = BdatType::Dynamics;
            } else if (byteData == 0x15) {
            type = BdatType::Glissando;
            } else if (byteData == 0x16) {
            type = BdatType::Decorator;
            } else if (byteData == 0x17) {
            type = BdatType::Key;
            } else if (byteData == 0x18) {
            type = BdatType::Lyric;
            } else if (byteData == 0x19) {
            type = BdatType::Octave_Shift;
            } else if (byteData == 0x1B) {
            type = BdatType::Slur;
            } else if (byteData == 0x1D) {
            type = BdatType::Text;
            } else if (byteData == 0x1E) {
            type = BdatType::Tie;
            } else if (byteData == 0x1F) {
            type = BdatType::Tuplet;
            } else if (byteData == 0x21) {
            type = BdatType::Guitar_Bend;
            } else if (byteData == 0x22) {
            type = BdatType::Guitar_Barre;
            } else if (byteData == 0x23) {
            type = BdatType::Pedal;
            } else if (byteData == 0x24) {
            type = BdatType::KuoHao;
            } else if (byteData == 0x25) {
            type = BdatType::Expressions;
            } else if (byteData == 0x26) {
            type = BdatType::Harp_Pedal;
            } else if (byteData == 0x27) {
            type = BdatType::Multi_Measure_Rest;
            } else if (byteData == 0x28) {
            type = BdatType::Harmony_GuitarFrame;
            } else if (byteData == 0x40) {
            type = BdatType::Graphics_40;
            } else if (byteData == 0x41) {
            type = BdatType::Graphics_RoundRect;
            } else if (byteData == 0x42) {
            type = BdatType::Graphics_Rect;
            } else if (byteData == 0x43) {
            type = BdatType::Graphics_Round;
            } else if (byteData == 0x44) {
            type = BdatType::Graphics_Line;
            } else if (byteData == 0x45) {
            type = BdatType::Graphics_Curve;
            } else if (byteData == 0x46) {
            type = BdatType::Graphics_WedgeSymbol;
            } else if (byteData == 0xAB) {
            type = BdatType::Midi_Controller;
            } else if (byteData == 0xAC) {
            type = BdatType::Midi_Program_Change;
            } else if (byteData == 0xAD) {
            type = BdatType::Midi_Channel_Pressure;
            } else if (byteData == 0xAE) {
            type = BdatType::Midi_Pitch_Wheel;
            } else if (byteData == 0xFF) {
            type = BdatType::Bar_End;
            } else {
            return false;
            }

      return true;
      }

///////////////////////////////////////////////////////////////////////////////
LyricChunkParse::LyricChunkParse(OveSong* ove) :
      BasicParse(ove) {
      }

void LyricChunkParse::setLyricChunk(SizeChunk* chunk) {
      chunk_ = chunk;
      }

// only ove3 has this chunk
bool LyricChunkParse::parse() {
      unsigned int i;
      Block* dataBlock = chunk_->getDataBlock();
      unsigned int blockSize = chunk_->getSizeBlock()->toSize();
      StreamHandle handle(dataBlock->data(), blockSize);
      Block placeHolder;

      handle_ = &handle;

      if( !jump(4) ) { return false; }

      // Lyric count
      if( !readBuffer(placeHolder, 2) ) { return false; }
      unsigned int count = placeHolder.toUnsignedInt();

      for( i=0; i<count; ++i ) {
            LyricInfo info;

            if( !readBuffer(placeHolder, 2) ) { return false; }
            //unsigned int size = placeHolder.toUnsignedInt();

            // 0x0D00
            if( !jump(2) ) { return false; }

            // voice
            if( !readBuffer(placeHolder, 1) ) { return false; }
            info.voice_ = placeHolder.toUnsignedInt();

            // verse
            if( !readBuffer(placeHolder, 1) ) { return false; }
            info.verse_ = placeHolder.toUnsignedInt();

            // track
            if( !readBuffer(placeHolder, 1) ) { return false; }
            info.track_ = placeHolder.toUnsignedInt();

            if( !jump(1) ) { return false; }

            // measure
            if( !readBuffer(placeHolder, 2) ) { return false; }
            info.measure_ = placeHolder.toUnsignedInt();

            // word count
            if( !readBuffer(placeHolder, 2) ) { return false; }
            info.wordCount_ = placeHolder.toUnsignedInt();

            // lyric size
            if( !readBuffer(placeHolder, 2) ) { return false; }
            info.lyricSize_ = placeHolder.toUnsignedInt();

            if( !jump(6) ) { return false; }

            // name
            if( !readBuffer(placeHolder, 32) ) { return false; }
            info.name_ = ove_->getCodecString(placeHolder.fixedSizeBufferToStrByteArray());

            if( info.lyricSize_ > 0 ) {
                  // lyric
                  if( info.lyricSize_ > 0 ) {
                        if( !readBuffer(placeHolder, info.lyricSize_) ) { return false; }
                        info.lyric_ = ove_->getCodecString(placeHolder.fixedSizeBufferToStrByteArray());
                        }

                  if( !jump(4) ) { return false; }

                  // font
                  if( !readBuffer(placeHolder, 2) ) { return false; }
                  info.font_ = placeHolder.toUnsignedInt();

                  if( !jump(1) ) { return false; }

                  // font size
                  if( !readBuffer(placeHolder, 1) ) { return false; }
                  info.fontSize_ = placeHolder.toUnsignedInt();

                  // font style
                  if( !readBuffer(placeHolder, 1) ) { return false; }
                  info.fontStyle_ = placeHolder.toUnsignedInt();

                  if( !jump(1) ) { return false; }

                  for( int j=0; j<info.wordCount_; ++j ) {
                        if( !jump(8) ) { return false; }
                        }
                  }

            processLyricInfo(info);
            }

      return true;
      }

bool isSpace(char c) {
      return c == ' ' || c == '\n';
      }

void LyricChunkParse::processLyricInfo(const LyricInfo& info) {
      int i;
      int j;
      int index = 0; //words

      int measureId = info.measure_-1;
      bool changeMeasure = true;
      MeasureData* measureData = 0;
      int trackMeasureCount = ove_->getTrackBarCount();
      QStringList words = info.lyric_.split(" ", QString::SkipEmptyParts);

      while ( index < words.size() && measureId+1 < trackMeasureCount ) {
            if( changeMeasure ) {
                  ++measureId;
                  measureData = ove_->getMeasureData(info.track_, measureId);
                  changeMeasure = false;
                  }

            if( measureData == 0 ) { return; }
            QList<NoteContainer*> containers = measureData->getNoteContainers();
            QList<MusicData*> lyrics = measureData->getMusicDatas(MusicDataType::Lyric);

            for( i=0; i<containers.size() && index<words.size(); ++i ) {
                  if( containers[i]->getIsRest() ) {
                        continue;
                        }

                  for( j=0; j<lyrics.size(); ++j ) {
                        Lyric* lyric = static_cast<Lyric*>(lyrics[j]);

                        if( containers[i]->start()->getOffset() == lyric->start()->getOffset() &&
                            (int)containers[i]->getVoice() == info.voice_ &&
                            lyric->getVerse() == info.verse_ ) {
                              if(index<words.size()) {
                                    QString l = words[index].trimmed();
                                    if(!l.isEmpty()) {
                                          lyric->setLyric(l);
                                          lyric->setVoice(info.voice_);
                                          }
                                    }

                              ++index;
                              }
                        }
                  }

            changeMeasure = true;
            }
      }

///////////////////////////////////////////////////////////////////////////////
TitleChunkParse::TitleChunkParse(OveSong* ove) :
      BasicParse(ove) {
      titleType_ = 0x00000001;
      annotateType_ = 0x00010000;
      writerType_ = 0x00020002;
      copyrightType_ = 0x00030001;
      headerType_ = 0x00040000;
      footerType_ = 0x00050002;
      }

void TitleChunkParse::setTitleChunk(SizeChunk* chunk) {
      chunk_ = chunk;
      }

QByteArray getByteArray(const Block& block) {
      QByteArray array((char*)block.data(), block.size());
      int index0 = array.indexOf('\0');
      array = array.left(index0);

      return array;
      }

bool TitleChunkParse::parse() {
      Block* dataBlockP = chunk_->getDataBlock();
      unsigned int blockSize = chunk_->getSizeBlock()->toSize();
      StreamHandle handle(dataBlockP->data(), blockSize);
      Block typeBlock;
      unsigned int titleType;

      handle_ = &handle;

      if( !readBuffer(typeBlock, 4) ) { return false; }

      titleType = typeBlock.toUnsignedInt();

      if( titleType == titleType_ || titleType == annotateType_ || titleType == writerType_ || titleType == copyrightType_ ) {
            Block offsetBlock;

            if( !readBuffer(offsetBlock, 4) ) { return false; }

            const unsigned int itemCount = 4;
            unsigned int i;

            for( i=0; i<itemCount; ++i ) {
                  if( i>0 ) {
                        //0x 00 AB 00 0C 00 00
                        if( !jump(6) ) { return false; }
                        }

                  Block countBlock;
                  if( !readBuffer(countBlock, 2) ) { return false; }
                  unsigned int titleSize = countBlock.toUnsignedInt();

                  Block dataBlock;
                  if( !readBuffer(dataBlock, titleSize) ) { return false; }

                  QByteArray array = getByteArray(dataBlock);
                  if(!array.isEmpty()) {
                        addToOve(ove_->getCodecString(array), titleType);
                        }
                  }

            return true;
            }

      if( titleType == headerType_ || titleType == footerType_ ) {
            if( !jump(10) ) { return false; }

            Block countBlock;
            if( !readBuffer(countBlock, 2) ) { return false; }
            unsigned int titleSize = countBlock.toUnsignedInt();

            Block dataBlock;
            if( !readBuffer(dataBlock, titleSize) ) { return false; }

            QByteArray array = getByteArray(dataBlock);
            addToOve(ove_->getCodecString(array), titleType);

            //0x 00 AB 00 0C 00 00
            if( !jump(6) ) { return false; }

            return true;
            }

      return false;
      }

void TitleChunkParse::addToOve(const QString& str, unsigned int titleType) {
      if( str.isEmpty() ) { return; }

      if (titleType == titleType_) {
            ove_->addTitle(str);
            }

      if (titleType == annotateType_) {
            ove_->addAnnotate(str);
            }

      if (titleType == writerType_) {
            ove_->addWriter(str);
            }

      if (titleType == copyrightType_) {
            ove_->addCopyright(str);
            }

      if (titleType == headerType_) {
            ove_->addHeader(str);
            }

      if (titleType == footerType_) {
            ove_->addFooter(str);
            }
      }

}