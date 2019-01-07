#include "Chunk.h"

// Chunk.cpp

namespace OVE {


const QString Chunk::TrackName   = "TRAK";
const QString Chunk::PageName    = "PAGE";
const QString Chunk::LineName    = "LINE";
const QString Chunk::StaffName   = "STAF";
const QString Chunk::MeasureName = "MEAS";
const QString Chunk::ConductName = "COND";
const QString Chunk::BdatName    = "BDAT";

Chunk::Chunk() {
      }

NameBlock Chunk::getName() const {
      return nameBlock_;
      }

////////////////////////////////////////////////////////////////////////////////
const unsigned int SizeChunk::version3TrackSize = 0x13a;

SizeChunk::SizeChunk() :
      Chunk() {
      sizeBlock_ = new SizeBlock();
      dataBlock_ = new Block();
      }

SizeChunk::~SizeChunk() {
      delete sizeBlock_;
      delete dataBlock_;
      }

SizeBlock* SizeChunk::getSizeBlock() const {
      return sizeBlock_;
      }

Block* SizeChunk::getDataBlock() const {
      return dataBlock_;
      }

/////////////////////////////////////////////////////////////////////////////////
GroupChunk::GroupChunk() : Chunk() {
      childCount_ = new CountBlock();
      }

GroupChunk::~GroupChunk() {
      delete childCount_;
      }

CountBlock* GroupChunk::getCountBlock() const {
      return childCount_;
      }

}



