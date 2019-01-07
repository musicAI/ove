#include "Block.h"

namespace OVE{

//////////////////////////////////////////////////////////////////////////////////////

// Block.cpp
///////////////////////////////////////////////////////////////////////////////////////////////////
Block::Block() {
      doResize(0);
      }

Block::Block(unsigned int count) {
      doResize(count);
      }

void Block::resize(unsigned int count) {
      doResize(count);
      }

void Block::doResize(unsigned int count) {
      data_.clear();
      for(unsigned int i=0; i<count; ++i) {
            data_.push_back('\0');
            }
      //data_.resize(count);
      }

const unsigned char* Block::data() const {
      //return const_cast<unsigned char*>(&data_.front());
      return &data_.front();
      }

unsigned char* Block::data() {
      return &data_.front();
      }

int Block::size() const {
      return data_.size();
      }

bool Block::toBoolean() const {
      if (data() == NULL) {
            return false;
            }

      return size() == 1 && data()[0] == 0x01;
      }

unsigned int Block::toUnsignedInt() const {
      if (data() == NULL) {
            return 0;
            }

      int num = 0;

      for (int i = 0; i < (int)sizeof(int) && i < size(); ++i) {
            num = (num << 8) + *(data() + i);
            }

      return num;
      }

int Block::toInt() const {
      if (data() == NULL) {
            return 0;
            }

      int i;
      int num = 0;

      for (i = 0; i < (int)sizeof(int) && i < size(); ++i) {
            num = (num << 8) + (int) *(data() + i);
            }

      std::size_t minSize = sizeof(int);
      if (size() < (int)minSize) {
            minSize = size();
            }

      if ((*(data()) & 0x80) == 0x80) {
            int maxNum = 0x1 << int(minSize * 8); //int(pow(2.0, (int) minSize * 8));
            num -= maxNum;
            //num *= -1;
            }

      return num;
      }

QByteArray Block::toStrByteArray() const {
      if (data() == NULL) {
            return QByteArray();
            }

      QByteArray arr((char*) data(), size());

      return arr;
      }

QByteArray Block::fixedSizeBufferToStrByteArray() const {
      QByteArray str;

      for (int i = 0; i < size(); ++i) {
            if (*(data() + i) == '\0') {
                  break;
                  }

            str += (char) *(data() + i);
            }

      return str;
      }

bool Block::operator ==(const Block& block) const {
      if (size() != block.size()) {
            return false;
            }

      for (int i = 0; i < size() && i < block.size(); ++i) {
            if (*(data() + i) != *(block.data() + i)) {
                  return false;
                  }
            }

      return true;
      }

bool Block::operator !=(const Block& block) const {
      return !(*this == block);
      }

///////////////////////////////////////////////////////////////////////////////////////////////////
FixedBlock::FixedBlock() :
      Block() {
      }

FixedBlock::FixedBlock(unsigned int count) :
      Block(count) {
      }

void FixedBlock::resize(unsigned int /*count*/) {
      // Block::resize(size);
      }

///////////////////////////////////////////////////////////////////////////////////////////////////
SizeBlock::SizeBlock() :
      FixedBlock(4) {
      }

unsigned int SizeBlock::toSize() const {
      unsigned int i;
      unsigned int num(0);
      const unsigned int SIZE = 4;

      for (i = 0; i < SIZE; ++i) {
            num = (num << 8) + *(data() + i);
            }

      return num;
      }

/*void SizeBlock::fromUnsignedInt(unsigned int count)
 {
 unsigned_int_to_char_buffer(count, data());
 }*/

///////////////////////////////////////////////////////////////////////////////////////////////////
NameBlock::NameBlock() :
      FixedBlock(4) {
      }

/*void NameBlock::setValue(const char* const name)
 {
 unsigned int i;

 for( i=0; i<size() && *(name+i)!='\0'; ++i )
 {
 *(data()+i) = *(name+i);
 }
 }*/

bool NameBlock::isEqual(const QString& name) const {
      int nsize = name.size();

      if (nsize != size()) {
            return false;
            }

      for (int i = 0; i < size() && nsize; ++i) {
            if (data()[i] != name[i]) {
                  return false;
                  }
            }

      return true;
      }

///////////////////////////////////////////////////////////////////////////////////////////////////
CountBlock::CountBlock() :
      FixedBlock(2) {
      }

/*void CountBlock::setValue(unsigned short count)
 {
 unsigned int i;
 unsigned int SIZE = sizeof(unsigned short);

 for( i=0; i<SIZE; ++i )
 {
 data()[SIZE-1-i] = count % 256;
 count /= 256;
 }
 }*/

unsigned short CountBlock::toCount() const {
      unsigned short num = 0;

      for (int i = 0; i < size() && i < (int)sizeof(unsigned short); ++i) {
            num = (num << 8) + *(data() + i);
            }

      return num;
      }

}

