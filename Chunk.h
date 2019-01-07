#ifndef _CHUNK_H
#define _CHUNK_H

#include <QByteArray>
#include <QList>
#include <QString>
#include "Block.h"

// Chunk.h

namespace OVE {

// content : name
class Chunk {
public:
      Chunk();
      virtual ~Chunk() {
            }

public:
      const static QString TrackName;
      const static QString PageName;
      const static QString LineName;
      const static QString StaffName;
      const static QString MeasureName;
      const static QString ConductName;
      const static QString BdatName;

      NameBlock getName() const;

protected:
      NameBlock nameBlock_;
      };

// content : name / size / data
class SizeChunk: public Chunk {
public:
      SizeChunk();
      virtual ~SizeChunk();

public:
      SizeBlock* getSizeBlock() const;
      Block* getDataBlock() const;

      const static unsigned int version3TrackSize;

protected:
      SizeBlock* sizeBlock_;
      Block* dataBlock_;
      };

// content : name / count
class GroupChunk: public Chunk {
public:
      GroupChunk();
      virtual ~GroupChunk();

public:
      CountBlock* getCountBlock() const;

protected:
      CountBlock* childCount_;
      };

}

#endif // _CHUNK_H 