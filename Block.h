#ifndef _BLOCK_H
#define _BLOCK_H

#include <QByteArray>
#include <QList>
#include <QString>

// Block.h
// base block, or resizable block in ove to store data

namespace OVE {

class Block {
public:
      Block();
      explicit Block(unsigned int size);
      virtual ~Block() {
            }

public:
      // size > 0, check this in use code
      virtual void resize(unsigned int count);

      const unsigned char* data() const;
      unsigned char* data();
      int size() const;

      bool operator ==(const Block& block) const;
      bool operator !=(const Block& block) const;

      bool toBoolean() const;
      unsigned int toUnsignedInt() const;
      int toInt() const;
      QByteArray toStrByteArray() const;					// string
      QByteArray fixedSizeBufferToStrByteArray() const;	// string

private:
      void doResize(unsigned int count);

private:
      // char [-128, 127], unsigned char [0, 255]
      QList<unsigned char> data_;
      };

class FixedBlock: public Block {
public:
      explicit FixedBlock(unsigned int count);
      virtual ~FixedBlock() {
            }

private:
      FixedBlock();

private:
      // can't resize
      virtual void resize(unsigned int count);
      };

///////////////////////////////////////////////////////////////////////////////
// 4 byte block in ove to store size
class SizeBlock: public FixedBlock {
public:
      SizeBlock();
      virtual ~SizeBlock() {
            }

public:
      //	void fromUnsignedInt(unsigned int count) ;

      unsigned int toSize() const;
      };

// 4 bytes block in ove to store name
class NameBlock: public FixedBlock {
public:
      NameBlock();
      virtual ~NameBlock() {
            }

public:
      // ignore data more than 4 bytes
      bool isEqual(const QString& name) const;
      };

// 2 bytes block in ove to store count
class CountBlock: public FixedBlock {
public:
      CountBlock();
      virtual ~CountBlock() {
            }

public:
      //	void setValue(unsigned short count) ;

      unsigned short toCount() const;
      };

}

#endif  // _BLOCK_H