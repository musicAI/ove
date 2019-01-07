#ifndef _OVE_SERIALIZE_H
#define _OVE_SERIALIZE_H

// OveSerialize.h

#include "ChunkParse.h"
#include "OveOrganizer.h"
#include <QMap>

namespace OVE {

class OveSong;




class OveSerialize: public IOVEStreamLoader {
public:
      OveSerialize();
      virtual ~OveSerialize();

public:
      virtual void setOve(OveSong* ove);
      virtual void setFileStream(unsigned char* buffer, unsigned int size);
      virtual void setNotify(IOveNotify* notify);
      virtual bool load(void);

      virtual void release();

private:
      bool readNameBlock(NameBlock& nameBlock);
      bool readChunkName(Chunk* chunk, const QString& name);
      bool readSizeChunk(SizeChunk* sizeChunk); // contains a SizeChunk and data buffer
      bool readDataChunk(Block* block, unsigned int size);
      bool readGroupChunk(GroupChunk* groupChunk);

      bool readHeader();
      bool readHeadData(SizeChunk* ovscChunk);
      bool readTracksData();
      bool readPagesData();
      bool readLinesData();
      bool readBarsData();
      bool readOveEnd();

      void messageOutError();
      void messageOut(const QString& str);

private:
      OveSong* ove_;
      StreamHandle* streamHandle_;
      IOveNotify* notify_;
      };

      
}




#endif  // _OVE_SERIALIZE_H