// OveSerialize.cpp

#include "OveSerialize.h"

namespace OVE {


ChunkType nameToChunkType(const NameBlock& name) {
      ChunkType type = ChunkType::NONE;

      if (name.isEqual("OVSC")) {
            type = ChunkType::OVSC;
            } else if (name.isEqual("TRKL")) {
            type = ChunkType::TRKL;
            } else if (name.isEqual("TRAK")) {
            type = ChunkType::TRAK;
            } else if (name.isEqual("PAGL")) {
            type = ChunkType::PAGL;
            } else if (name.isEqual("PAGE")) {
            type = ChunkType::PAGE;
            } else if (name.isEqual("LINL")) {
            type = ChunkType::LINL;
            } else if (name.isEqual("LINE")) {
            type = ChunkType::LINE;
            } else if (name.isEqual("STAF")) {
            type = ChunkType::STAF;
            } else if (name.isEqual("BARL")) {
            type = ChunkType::BARL;
            } else if (name.isEqual("MEAS")) {
            type = ChunkType::MEAS;
            } else if (name.isEqual("COND")) {
            type = ChunkType::COND;
            } else if (name.isEqual("BDAT")) {
            type = ChunkType::BDAT;
            } else if (name.isEqual("PACH")) {
            type = ChunkType::PACH;
            } else if (name.isEqual("FNTS")) {
            type = ChunkType::FNTS;
            } else if (name.isEqual("ODEV")) {
            type = ChunkType::ODEV;
            } else if (name.isEqual("TITL")) {
            type = ChunkType::TITL;
            } else if (name.isEqual("ALOT")) {
            type = ChunkType::ALOT;
            } else if (name.isEqual("ENGR")) {
            type = ChunkType::ENGR;
            } else if (name.isEqual("FMAP")) {
            type = ChunkType::FMAP;
            } else if (name.isEqual("PCPR")) {
            type = ChunkType::PCPR;
            } else if (name.isEqual("LYRC")) {
            type = ChunkType::LYRC;
            }

      return type;
      }

int chunkTypeToMaxTimes(ChunkType type) {
      int maxTimes = -1; // no limit

      switch (type) {
            case ChunkType::OVSC: {
                  maxTimes = 1;
                  break;
                  }
            case ChunkType::TRKL: {// case ChunkType::TRAK :
                  maxTimes = 1;
                  break;
                  }
            case ChunkType::PAGL: {// case ChunkType::PAGE :
                  maxTimes = 1;
                  break;
                  }
                  // case ChunkType::LINE :
                  // case ChunkType::STAF :
            case ChunkType::LINL: {
                  maxTimes = 1;
                  break;
                  }
                  // case ChunkType::MEAS :
                  // case ChunkType::COND :
                  // case ChunkType::BDAT :
            case ChunkType::BARL: {
                  maxTimes = 1;
                  break;
                  }
            case ChunkType::PACH:
            case ChunkType::FNTS:
            case ChunkType::ODEV:
            case ChunkType::ALOT:
            case ChunkType::ENGR:
            case ChunkType::FMAP:
            case ChunkType::PCPR: {
                  maxTimes = 1;
                  break;
                  }
            case ChunkType::TITL: {
                  maxTimes = 8;
                  break;
                  }
            case ChunkType::LYRC: {
                  maxTimes = 1;
                  break;
                  }
                  // case ChunkType::NONE :
            default:
                  break;
            }

      return maxTimes;
      }

///////////////////////////////////////////////////////////////////////////////////////////

OveSerialize::OveSerialize() :
      ove_(0),
      streamHandle_(0),
      notify_(0) {
      }

OveSerialize::~OveSerialize() {
      if(streamHandle_ != 0) {
            delete streamHandle_;
            streamHandle_ = 0;
            }
      }

void OveSerialize::setOve(OveSong* ove) {
      ove_ = ove;
      }

void OveSerialize::setFileStream(unsigned char* buffer, unsigned int size) {
      streamHandle_ = new StreamHandle(buffer, size);
      }

void OveSerialize::setNotify(IOveNotify* notify) {
      notify_ = notify;
      }

void OveSerialize::messageOutError() {
      if (notify_ != NULL) {
            notify_->loadError();
            }
      }

void OveSerialize::messageOut(const QString& str) {
      if (notify_ != NULL) {
            notify_->loadInfo(str);
            }
      }

bool OveSerialize::load(void) {
      if(streamHandle_ == 0)
            return false;

      if( !readHeader() ) {
            messageOutError();
            return false;
            }

      unsigned int i;
      QMap<ChunkType, int> chunkTimes;
      //bool firstEnter = true;

      for( i=(int)ChunkType::OVSC; i<(int)ChunkType::NONE; ++i ) {
            chunkTimes[(ChunkType)i] = 0;
            }

      ChunkType chunkType = ChunkType::NONE;

      do {
            NameBlock nameBlock;
            SizeChunk sizeChunk;

            if( !readNameBlock(nameBlock) ) { return false; }

            chunkType = nameToChunkType(nameBlock);
            ++chunkTimes[chunkType];
            int maxTime = chunkTypeToMaxTimes(chunkType);

            if( maxTime > 0 && chunkTimes[chunkType] > maxTime ) {
                  messageOut("format not support, chunk appear more than accept.\n");
                  return false;
                  }

            switch (chunkType) {
                  /*case ChunkType::OVSC :
       {
       if( !readHeadData(&sizeChunk) )
       {
       messageOut_error();
       return false;
       }

       break;
       }*/
                  case ChunkType::TRKL: {
                        if (!readTracksData()) {
                              messageOutError();
                              return false;
                              }

                        break;
                        }
                  case ChunkType::PAGL: {
                        if (!readPagesData()) {
                              messageOutError();
                              return false;
                              }

                        break;
                        }
                  case ChunkType::LINL: {
                        if (!readLinesData()) {
                              messageOutError();
                              return false;
                              }

                        break;
                        }
                  case ChunkType::BARL: {
                        if (!readBarsData()) {
                              messageOutError();
                              return false;
                              }

                        break;
                        }
                  case ChunkType::TRAK:
                  case ChunkType::PAGE:
                  case ChunkType::LINE:
                  case ChunkType::STAF:
                  case ChunkType::MEAS:
                  case ChunkType::COND:
                  case ChunkType::BDAT: {
                        return false;
                        break;
                        }
                  case ChunkType::LYRC: {
                        SizeChunk lyricChunk;
                        if (!readSizeChunk(&lyricChunk)) {
                              messageOutError();
                              return false;
                              }

                        LyricChunkParse parse(ove_);

                        parse.setLyricChunk(&lyricChunk);
                        parse.parse();

                        break;
                        }
                  case ChunkType::TITL: {
                        SizeChunk titleChunk;
                        if (!readSizeChunk(&titleChunk)) {
                              messageOutError();
                              return false;
                              }

                        TitleChunkParse titleChunkParse(ove_);

                        titleChunkParse.setTitleChunk(&titleChunk);
                        titleChunkParse.parse();

                        break;
                        }
                  case ChunkType::PACH:
                  case ChunkType::FNTS:
                  case ChunkType::ODEV:
                  case ChunkType::ALOT:
                  case ChunkType::ENGR:
                  case ChunkType::FMAP:
                  case ChunkType::PCPR: {
                        if (!readSizeChunk(&sizeChunk)) {
                              messageOutError();
                              return false;
                              }

                        break;
                        }
                  default:
                        /*if( firstEnter )
          {
          QString info = "Not compatible file, try to load and save with newer version, Overture 4 is recommended.\n";
          messageOut(info);
          messageOutError();

          return false;
          }*/

                        break;
                  }

            //firstEnter = false;
            }
      while ( chunkType != ChunkType::NONE );

      // if( !readOveEnd() ) { return false; }

      // organize OveData
      OVE::OveOrganizer organizer(ove_);
      organizer.organize();

      return true;
      }

void OveSerialize::release() {
      delete this;
      }

bool OveSerialize::readHeader() {
      ChunkType chunkType = ChunkType::NONE;
      NameBlock nameBlock;
      SizeChunk sizeChunk;

      if (!readNameBlock(nameBlock)) {
            return false;
            }

      chunkType = nameToChunkType(nameBlock);
      //int maxTime = chunkTypeToMaxTimes(chunkType);

      if (chunkType == ChunkType::OVSC) {
            if (readHeadData(&sizeChunk)) {
                  return true;
                  }
            }

      QString info = "Not compatible file, try to load and save with newer version, Overture 4 is recommended.\n";
      messageOut(info);

      return false;
      }

bool OveSerialize::readHeadData(SizeChunk* ovscChunk) {
      if (!readSizeChunk(ovscChunk))
            return false;

      OvscParse ovscParse(ove_);

      ovscParse.setNotify(notify_);
      ovscParse.setOvsc(ovscChunk);

      return ovscParse.parse();
      }

bool OveSerialize::readTracksData() {
      GroupChunk trackGroupChunk;

      if (!readGroupChunk(&trackGroupChunk))
            return false;

      unsigned int i;
      unsigned short trackCount = trackGroupChunk.getCountBlock()->toCount();

      for (i = 0; i < trackCount; ++i) {
            SizeChunk* trackChunk = new SizeChunk();

            if (ove_->getIsVersion4()) {
                  if (!readChunkName(trackChunk, Chunk::TrackName)) {
                        return false;
                        }
                  if (!readSizeChunk(trackChunk)) {
                        return false;
                        }
                  } else {
                  if (!readDataChunk(trackChunk->getDataBlock(),
                                     SizeChunk::version3TrackSize)) {
                        return false;
                        }
                  }

            TrackParse trackParse(ove_);

            trackParse.setTrack(trackChunk);
            trackParse.parse();
            }

      return true;
      }

bool OveSerialize::readPagesData() {
      GroupChunk pageGroupChunk;

      if (!readGroupChunk(&pageGroupChunk))
            return false;

      unsigned short pageCount = pageGroupChunk.getCountBlock()->toCount();
      unsigned int i;
      PageGroupParse parse(ove_);

      for (i = 0; i < pageCount; ++i) {
            SizeChunk* pageChunk = new SizeChunk();

            if (!readChunkName(pageChunk, Chunk::PageName)) {
                  return false;
                  }
            if (!readSizeChunk(pageChunk)) {
                  return false;
                  }

            parse.addPage(pageChunk);
            }

      if (!parse.parse()) {
            return false;
            }

      return true;
      }

bool OveSerialize::readLinesData() {
      GroupChunk lineGroupChunk;
      if (!readGroupChunk(&lineGroupChunk))
            return false;

      unsigned short lineCount = lineGroupChunk.getCountBlock()->toCount();
      int i;
      unsigned int j;
      QList<SizeChunk*> lineChunks;
      QList<SizeChunk*> staffChunks;

      for (i = 0; i < lineCount; ++i) {
            SizeChunk* lineChunk = new SizeChunk();

            if (!readChunkName(lineChunk, Chunk::LineName)) {
                  return false;
                  }
            if (!readSizeChunk(lineChunk)) {
                  return false;
                  }

            lineChunks.push_back(lineChunk);

            StaffCountGetter getter(ove_);
            unsigned int staffCount = getter.getStaffCount(lineChunk);

            for (j = 0; j < staffCount; ++j) {
                  SizeChunk* staffChunk = new SizeChunk();

                  if (!readChunkName(staffChunk, Chunk::StaffName)) {
                        return false;
                        }
                  if (!readSizeChunk(staffChunk)) {
                        return false;
                        }

                  staffChunks.push_back(staffChunk);
                  }
            }

      LineGroupParse parse(ove_);

      parse.setLineGroup(&lineGroupChunk);

      for (i = 0; i < lineChunks.size(); ++i) {
            parse.addLine(lineChunks[i]);
            }

      for (i = 0; i < staffChunks.size(); ++i) {
            parse.addStaff(staffChunks[i]);
            }

      if (!parse.parse()) {
            return false;
            }

      return true;
      }

bool OveSerialize::readBarsData() {
      GroupChunk barGroupChunk;
      if (!readGroupChunk(&barGroupChunk))
            return false;

      unsigned short measCount = barGroupChunk.getCountBlock()->toCount();
      int i;

      QList<SizeChunk*> measureChunks;
      QList<SizeChunk*> conductChunks;
      QList<SizeChunk*> bdatChunks;

      ove_->setTrackBarCount(measCount);

      // read chunks
      for (i = 0; i < measCount; ++i) {
            SizeChunk* measureChunkPtr = new SizeChunk();

            if (!readChunkName(measureChunkPtr, Chunk::MeasureName)) {
                  return false;
                  }
            if (!readSizeChunk(measureChunkPtr)) {
                  return false;
                  }

            measureChunks.push_back(measureChunkPtr);
            }

      for (i = 0; i < measCount; ++i) {
            SizeChunk* conductChunkPtr = new SizeChunk();

            if (!readChunkName(conductChunkPtr, Chunk::ConductName))
                  return false;

            if (!readSizeChunk(conductChunkPtr))
                  return false;

            conductChunks.push_back(conductChunkPtr);
            }

      int bdatCount = ove_->getTrackCount() * measCount;
      for (i = 0; i < bdatCount; ++i) {
            SizeChunk* batChunkPtr = new SizeChunk();

            if (!readChunkName(batChunkPtr, Chunk::BdatName)) {
                  return false;
                  }
            if (!readSizeChunk(batChunkPtr)) {
                  return false;
                  }

            bdatChunks.push_back(batChunkPtr);
            }

      // parse bars
      BarsParse barsParse(ove_);

      for (i = 0; i < (int) measureChunks.size(); ++i) {
            barsParse.addMeasure(measureChunks[i]);
            }

      for (i = 0; i < (int) conductChunks.size(); ++i) {
            barsParse.addConduct(conductChunks[i]);
            }

      for (i = 0; i < (int) bdatChunks.size(); ++i) {
            barsParse.addBdat(bdatChunks[i]);
            }

      barsParse.setNotify(notify_);
      if (!barsParse.parse()) {
            return false;
            }

      return true;
      }

bool OveSerialize::readOveEnd() {
      if (streamHandle_ == 0)
            return false;

      const unsigned int END_OVE1 = 0xffffffff;
      const unsigned int END_OVE2 = 0x00000000;
      unsigned int buffer;

      if (!streamHandle_->read((char*) &buffer, sizeof(unsigned int)))
            return false;

      if (buffer != END_OVE1)
            return false;

      if (!streamHandle_->read((char*) &buffer, sizeof(unsigned int)))
            return false;

      if (buffer != END_OVE2)
            return false;

      return true;
      }

/////////////////////////////////////////////////////////////////////////////////////////
bool OveSerialize::readNameBlock(NameBlock& nameBlock) {
      if (streamHandle_ == 0)
            return false;

      if (!streamHandle_->read((char*) nameBlock.data(), nameBlock.size()))
            return false;

      return true;
      }

bool OveSerialize::readChunkName(Chunk* /*chunk*/, const QString& name) {
      if (streamHandle_ == 0)
            return false;

      NameBlock nameBlock;

      if (!streamHandle_->read((char*) nameBlock.data(), nameBlock.size()))
            return false;

      if (!(nameBlock.toStrByteArray() == name))
            return false;

      return true;
      }

bool OveSerialize::readSizeChunk(SizeChunk* sizeChunk) {
      if (streamHandle_ == 0)
            return false;

      SizeBlock* sizeBlock = sizeChunk->getSizeBlock();

      if (!streamHandle_->read((char*) sizeBlock->data(), sizeBlock->size()))
            return false;

      unsigned int blockSize = sizeBlock->toSize();

      sizeChunk->getDataBlock()->resize(blockSize);

      Block* dataBlock = sizeChunk->getDataBlock();

      if (!streamHandle_->read((char*) dataBlock->data(), blockSize))
            return false;

      return true;
      }

bool OveSerialize::readDataChunk(Block* block, unsigned int size) {
      if (streamHandle_ == 0)
            return false;

      block->resize(size);

      if (!streamHandle_->read((char*) block->data(), size))
            return false;

      return true;
      }

bool OveSerialize::readGroupChunk(GroupChunk* groupChunk) {
      if (streamHandle_ == 0)
            return false;

      CountBlock* countBlock = groupChunk->getCountBlock();

      if (!streamHandle_->read((char*) countBlock->data(), countBlock->size()))
            return false;

      return true;
      }

IOVEStreamLoader* createOveStreamLoader() {
      return new OveSerialize;
      }

}
