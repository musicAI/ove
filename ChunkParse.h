#ifndef _CHUNK_PARSE_H
#define _CHUNK_PARSE_H

// ChunkParse.h

#include "OveScore.h"

#include "Block.h"

#include "Chunk.h"

#include "IOveNotify.h"

#include <QtCore>


namespace OVE {

class Page;
class Line;
class Staff;

class BasicParse {
public:
      BasicParse(OveSong* ove);
      virtual ~BasicParse();

private:
      BasicParse();

public:
      void setNotify(IOveNotify* notify);
      virtual bool parse();

protected:
      bool readBuffer(Block& placeHolder, int size);
      bool jump(int offset);

      void messageOut(const QString& str);

protected:
      OveSong* ove_;
      StreamHandle* handle_;
      IOveNotify* notify_;
      };

///////////////////////////////////////////////////////////////////////////////

class OvscParse: public BasicParse {
public:
      OvscParse(OveSong* ove);
      virtual ~OvscParse();

public:
      void setOvsc(SizeChunk* chunk);

      virtual bool parse();

private:
      SizeChunk* chunk_;
      };

class TrackParse: public BasicParse {
public:
      TrackParse(OveSong* ove);
      virtual ~TrackParse();

public:
      void setTrack(SizeChunk* chunk);

      virtual bool parse();

private:
      SizeChunk* chunk_;
      };

class GroupParse: BasicParse {
public:
      GroupParse(OveSong* ove);
      virtual ~GroupParse();

public:
      void addSizeChunk(SizeChunk* sizeChunk);

      virtual bool parse();

private:
      QList<SizeChunk*> sizeChunks_;
      };

class PageGroupParse: public BasicParse {
public:
      PageGroupParse(OveSong* ove);
      virtual ~PageGroupParse();

public:
      void addPage(SizeChunk* chunk);

      virtual bool parse();

private:
      bool parsePage(SizeChunk* chunk, Page* page);

private:
      QList<SizeChunk*> pageChunks_;
      };

class StaffCountGetter: public BasicParse {
public:
      StaffCountGetter(OveSong* ove);
      virtual ~StaffCountGetter() {}

public:
      unsigned int getStaffCount(SizeChunk* chunk);
      };

class LineGroupParse: public BasicParse {
public:
      LineGroupParse(OveSong* ove);
      virtual ~LineGroupParse();

public:
      void setLineGroup(GroupChunk* chunk);
      void addLine(SizeChunk* chunk);
      void addStaff(SizeChunk* chunk);

      virtual bool parse();

private:
      bool parseLine(SizeChunk* chunk, Line* line);
      bool parseStaff(SizeChunk* chunk, Staff* staff);

private:
      GroupChunk* chunk_;
      QList<SizeChunk*> lineChunks_;
      QList<SizeChunk*> staffChunks_;
      };

class BarsParse: public BasicParse {
public:
      BarsParse(OveSong* ove);
      virtual ~BarsParse();

public:
      void addMeasure(SizeChunk* chunk);
      void addConduct(SizeChunk* chunk);
      void addBdat(SizeChunk* chunk);

      virtual bool parse();

private:
      bool parseMeas(Measure* measure, SizeChunk* chunk);
      bool parseCond(Measure* measure, MeasureData* measureData, SizeChunk* chunk);
      bool parseBdat(Measure* measure, MeasureData* measureData, SizeChunk* chunk);

      bool getCondElementType(unsigned int byteData, CondType& type);
      bool getBdatElementType(unsigned int byteData, BdatType& type);

      // COND
      bool parseTimeSignature(Measure* measure, int length);
      bool parseTimeSignatureParameters(Measure* measure, int length);
      bool parseRepeatSymbol(MeasureData* measureData, int length);
      bool parseNumericEndings(MeasureData* measureData, int length);
      bool parseTempo(MeasureData* measureData, int length);
      bool parseBarNumber(Measure* measure, int length);
      bool parseText(MeasureData* measureData, int length);
      bool parseBarlineParameters(Measure* measure, int length);

      // BDAT
      bool parseNoteRest(MeasureData* measureData, int length, BdatType type);
      bool parseBeam(MeasureData* measureData, int length);
      bool parseTie(MeasureData* measureData, int length);
      bool parseTuplet(MeasureData* measureData, int length);
      bool parseHarmony(MeasureData* measureData, int length);
      bool parseClef(MeasureData* measureData, int length);
      bool parseLyric(MeasureData* measureData, int length);
      bool parseSlur(MeasureData* measureData, int length);
      bool parseGlissando(MeasureData* measureData, int length);
      bool parseDecorators(MeasureData* measureData, int length);
      bool parseDynamics(MeasureData* measureData, int length);
      bool parseWedge(MeasureData* measureData, int length);
      bool parseKey(MeasureData* measureData, int length);
      bool parsePedal(MeasureData* measureData, int length);
      bool parseKuohao(MeasureData* measureData, int length);
      bool parseExpressions(MeasureData* measureData, int length);
      bool parseHarpPedal(MeasureData* measureData, int length);
      bool parseMultiMeasureRest(MeasureData* measureData, int length);
      bool parseHarmonyGuitarFrame(MeasureData* measureData, int length);
      bool parseOctaveShift(MeasureData* measureData, int length);
      bool parseMidiController(MeasureData* measureData, int length);
      bool parseMidiProgramChange(MeasureData* measureData, int length);
      bool parseMidiChannelPressure(MeasureData* measureData, int length);
      bool parseMidiPitchWheel(MeasureData* measureData, int length);

      bool parseSizeBlock(int length);
      bool parseMidiCommon(MidiData* ptr);
      bool parseCommonBlock(MusicData* ptr);
      bool parseOffsetCommonBlock(MusicData* ptr);
      bool parsePairLinesBlock(PairEnds* ptr); //size==2
      bool parseOffsetElement(OffsetElement* ptr);//size==2

private:
      QList<SizeChunk*> measureChunks_;
      QList<SizeChunk*> conductChunks_;
      QList<SizeChunk*> bdatChunks_;
      };

class LyricChunkParse: public BasicParse {
public:
      LyricChunkParse(OveSong* ove);
      virtual ~LyricChunkParse() {}

public:
      void setLyricChunk(SizeChunk* chunk);

      virtual bool parse();

private:
      struct LyricInfo {
            int track_;
            int measure_;
            int verse_;
            int voice_;
            int wordCount_;
            int lyricSize_;
            QString name_;
            QString lyric_;
            int font_;
            int fontSize_;
            int fontStyle_;

            LyricInfo() :
                  track_(0), measure_(0), verse_(0), voice_(0), wordCount_(0),
                  lyricSize_(0), name_(QString()), lyric_(QString()),
                  font_(0), fontSize_(12), fontStyle_(0) {}
            };

      void processLyricInfo(const LyricInfo& info);

private:
      SizeChunk* chunk_;
      };

class TitleChunkParse: public BasicParse {
public:
      TitleChunkParse(OveSong* ove);
      virtual ~TitleChunkParse() {}

public:
      void setTitleChunk(SizeChunk* chunk);

      virtual bool parse();

private:
      void addToOve(const QString& str, unsigned int titleType);

private:
      unsigned int titleType_;
      unsigned int annotateType_;
      unsigned int writerType_;
      unsigned int copyrightType_;
      unsigned int headerType_;
      unsigned int footerType_;

      SizeChunk* chunk_;
      };

}

#endif // _CHUNK_PARSE_H