#ifndef _OVE_SCORE_H
#define _OVE_SCORE_H

#include "OveEnum.h"
#include "OveElement.h"
#include <QtCore>

namespace OVE {

class Track;
class Page;
class Voice;
class Line;
class Staff;
class Measure;
class MeasureData;
class Note;
class NoteContainer;
class Beam;
class Tie;
class Tuplet;
class Harmony;
class Clef;
class Lyric;
class Slur;
class MeasureText;
class Articulation;
class Glissando;
class Decorator;
class MeasureRepeat;
class Dynamics;
class Wedge;
class WedgeEndPoint;
class Pedal;
class KuoHao;
class Expressions;
class HarpPedal;
class MultiMeasureRest;
class OctaveShift;
class OctaveShiftEndPoint;
class BarNumber;
class Tempo;
class Text;
class TimeSignature;
class Key;
class RepeatSymbol;
class NumericEnding;
class MidiData;
class MidiController;
class MidiProgramChange;
class MidiChannelPressure;
class MidiPitchWheel;
	

class OveSong {
public:
      OveSong();
      ~OveSong();

public:
      void setIsVersion4(bool version4 = true);
      bool getIsVersion4() const;

      void setQuarter(int tick);
      int getQuarter(void) const;

      void setShowPageMargin(bool show);
      bool getShowPageMargin() const;

      void setShowTransposeTrack(bool show);
      bool getShowTransposeTrack() const;

      void setShowLineBreak(bool show);
      bool getShowLineBreak() const;

      void setShowRuler(bool show);
      bool getShowRuler() const;

      void setShowColor(bool show);
      bool getShowColor() const;

      void setPlayRepeat(bool play);
      bool getPlayRepeat() const;

      enum class PlayStyle : char {
            Record, Swing, Notation
            };
      void setPlayStyle(PlayStyle style);
      PlayStyle getPlayStyle() const;

      void addTitle(const QString& str);
      QList<QString> getTitles(void) const;

      void addAnnotate(const QString& str);
      QList<QString> getAnnotates(void) const;

      void addWriter(const QString& str);
      QList<QString> getWriters(void) const;

      void addCopyright(const QString& str);
      QList<QString> getCopyrights(void) const;

      void addHeader(const QString& str);
      QList<QString> getHeaders(void) const;

      void addFooter(const QString& str);
      QList<QString> getFooters(void) const;

      void addTrack(Track* ptr);
      int getTrackCount(void) const;
      QList<Track*> getTracks() const;
      Track* getTrack(int part, int staff) const;

      void setTrackBarCount(int count);
      int getTrackBarCount() const;

      bool addPage(Page* page);
      int getPageCount() const;
      Page* getPage(int idx);

      void addLine(Line* ptr);
      int getLineCount() const;
      Line* getLine(int idx) const;

      void addMeasure(Measure* ptr);
      int getMeasureCount(void) const;
      Measure* getMeasure(int bar) const;

      void addMeasureData(MeasureData* ptr);
      int getMeasureDataCount(void) const;
      MeasureData* getMeasureData(int part, int staff/*=0*/, int bar) const;
      MeasureData* getMeasureData(int track, int bar) const;

      // tool
      void setPartStaffCounts(const QList<int>& partStaffCounts);
      int getPartCount() const;
      int getStaffCount(int part) const;
      int getPartBarCount() const;

      void clear(void);

      QPair<int, int> trackToPartStaff(int track) const;

      void setTextCodecName(const QString& codecName);
      QString getCodecString(const QByteArray& text);

private:
      int partStaffToTrack(int part, int staff) const;

private:
      bool version4_;
      int quarter_;

      bool showPageMargin_;
      bool showTransposeTrack;
      bool showLineBreak_;
      bool showRuler_;
      bool showColor_;
      bool playRepeat_;
      PlayStyle playStyle_;

      QList<QString> titles_;
      QList<QString> annotates_;
      QList<QString> writers_;
      QList<QString> copyrights_;
      QList<QString> headers_;
      QList<QString> footers_;

      QList<Track*> tracks_;
      QList<Page*> pages_;
      QList<Line*> lines_;
      QList<Measure*> measures_;
      QList<MeasureData*> measureDatas_;
      int trackBarCount_;	//equal to measures_.size()

      QList<int> partStaffCounts_;
      QTextCodec* codec_;
      };

class Voice {
public:
      Voice();
      ~Voice(){}

public:
      void setChannel(int channel);
      int getChannel() const;

      void setVolume(int volume);
      int getVolume() const;

      void setPitchShift(int pitchShift);
      int getPitchShift() const;

      void setPan(int pan);
      int getPan() const;

      void setPatch(int patch);
      int getPatch() const;

      void setStemType(int stemType);
      int getStemType() const;

      static int getDefaultPatch();
      static int getDefaultVolume();

private:
      int channel_;		// [0, 15]
      int volume_;		// [-1, 127], -1 default
      int pitchShift_;	// [-36, 36]
      int pan_;			// [-64, 63]
      int patch_;			// [0, 127]
      int stemType_;		// 0, 1, 2
      };

class Track {
public:
      Track();
      ~Track();

public:
      void setName(const QString& str);
      QString getName(void) const;

      void setBriefName(const QString& str);
      QString getBriefName(void) const;

      void setPatch(unsigned int patch); // -1: percussion
      unsigned int getPatch() const;

      void setChannel(int channel);
      int getChannel() const;

      void setShowName(bool show);
      bool getShowName() const;

      void setShowBriefName(bool show);
      bool getShowBriefName() const;

      void setMute(bool mute);
      bool getMute() const;

      void setSolo(bool solo);
      bool getSolo() const;

      void setShowKeyEachLine(bool show);
      bool getShowKeyEachLine() const;

      void setVoiceCount(int voices);
      int getVoiceCount() const;

      void addVoice(Voice* voice);
      QList<Voice*> getVoices() const;

      void setShowTranspose(bool show);
      bool getShowTranspose() const;

      void setTranspose(int transpose);
      int getTranspose() const;

      void setNoteShift(int shift);
      int getNoteShift() const;

      void setStartClef(int clef/*in ClefType*/);
      ClefType getStartClef() const;

      void setTransposeClef(int clef/*in ClefType*/);
      ClefType getTansposeClef() const;

      void setStartKey(int key/*in KeyType*/);
      int getStartKey() const;

      void setDisplayPercent(unsigned int percent/*25~100*/);
      unsigned int getDisplayPercent() const;

      void setShowLegerLine(bool show);
      bool getShowLegerLine() const;

      void setShowClef(bool show);
      bool getShowClef() const;

      void setShowTimeSignature(bool show);
      bool getShowTimeSignature() const;

      void setShowKeySignature(bool show);
      bool getShowKeySignature() const;

      void setShowBarline(bool show);
      bool getShowBarline() const;

      void setFillWithRest(bool fill);
      bool getFillWithRest() const;

      void setFlatTail(bool flat);
      bool getFlatTail() const;

      void setShowClefEachLine(bool show);
      bool getShowClefEachLine() const;

      struct DrumNode {
            int line_;
            int headType_;
            int pitch_;
            int voice_;

      public:
            DrumNode():line_(0), headType_(0), pitch_(0), voice_(0){}
            };
      void addDrum(const DrumNode& node);
      QList<DrumNode> getDrumKit() const;

      void clear(void);

      /////////////////////////////////////////////////
      void setPart(int part);
      int getPart() const;

private:
      int number_;
      QString name_;
      QString briefName_;
      unsigned int patch_;
      int channel_;
      int transpose_;
      bool showTranspose_;
      int noteShift_;
      ClefType startClef_;
      ClefType transposeClef_;
      unsigned int displayPercent_;
      int startKey_;
      int voiceCount_;
      QList<Voice*> voices_;

      bool showName_;
      bool showBriefName_;
      bool showKeyEachLine_;
      bool showLegerLine_;
      bool showClef_;
      bool showTimeSignature_;
      bool showKeySignature_;
      bool showBarline_;
      bool showClefEachLine_;

      bool fillWithRest_;
      bool flatTail_;

      bool mute_;
      bool solo_;

      QList<DrumNode> drumKit_;

      //////////////////////////////
      int part_;
      };

class Page {
public:
      Page();
      ~Page(){}

public:
      void setBeginLine(int line);
      int getBeginLine() const;

      void setLineCount(int count);
      int getLineCount() const;

      void setLineInterval(int interval);	// between system
      int getLineInterval() const;

      void setStaffInterval(int interval);
      int getStaffInterval() const;

      void setStaffInlineInterval(int interval); // between treble-bass staff
      int getStaffInlineInterval() const;

      void setLineBarCount(int count);
      int getLineBarCount() const;

      void setPageLineCount(int count);
      int getPageLineCount() const;

      void setLeftMargin(int margin);
      int getLeftMargin() const;

      void setTopMargin(int margin);
      int getTopMargin() const;

      void setRightMargin(int margin);
      int getRightMargin() const;

      void setBottomMargin(int margin);
      int getBottomMargin() const;

      void setPageWidth(int width);
      int getPageWidth() const;

      void setPageHeight(int height);
      int getPageHeight() const;

private:
      int beginLine_;
      int lineCount_;

      int lineInterval_;
      int staffInterval_;
      int staffInlineInterval_;

      int lineBarCount_;
      int pageLineCount_;

      int leftMargin_;
      int topMargin_;
      int rightMargin_;
      int bottomMargin_;

      int pageWidth_;
      int pageHeight_;
      };

class Line {
public:
      Line();
      ~Line();

public:
      void addStaff(Staff* staff);
      int getStaffCount() const;
      Staff* getStaff(int idx) const;

      void setBeginBar(unsigned int bar);
      unsigned int getBeginBar() const;

      void setBarCount(unsigned int count);
      unsigned int getBarCount() const;

      void setYOffset(int offset);
      int getYOffset() const;

      void setLeftXOffset(int offset);
      int getLeftXOffset() const;

      void setRightXOffset(int offset);
      int getRightXOffset() const;

private:
      QList<Staff*> staffs_;
      unsigned int beginBar_;
      unsigned int barCount_;
      int yOffset_;
      int leftXOffset_;
      int rightXOffset_;
      };

class Staff : public OffsetElement {
public:
      Staff();
      virtual ~Staff(){}

public:
      void setClefType(int clef);
      ClefType getClefType() const;

      void setKeyType(int key);
      int getKeyType() const;

      void setVisible(bool visible);
      bool setVisible() const;

      void setGroupType(GroupType type);
      GroupType getGroupType() const;

      void setGroupStaffCount(int count);
      int getGroupStaffCount() const;

private:
      ClefType clef_;
      int key_;
      bool visible_;
      GroupType groupType_;
      int groupStaffCount_;
      };

///////////////////////////////////////////////////////////////////////////////

class Note : public LineElement {
public:
      Note();
      virtual ~Note(){}

public:
      void setIsRest(bool rest);
      bool getIsRest() const;

      void setNote(unsigned int note);
      unsigned int getNote() const;

      void setAccidental(int type);		//AccidentalType
      AccidentalType getAccidental() const;

      void setShowAccidental(bool show);
      bool getShowAccidental() const;

      void setOnVelocity(unsigned int velocity);
      unsigned int getOnVelocity() const;

      void setOffVelocity(unsigned int velocity);
      unsigned int getOffVelocity() const;

      void setHeadType(int type);		//NoteHeadType
      NoteHeadType getHeadType() const;

      void setTiePos(int tiePos);
      TiePos getTiePos() const;

      void setOffsetStaff(int offset);	// cross staff notes
      int getOffsetStaff() const;

      void setShow(bool show);
      bool getShow() const;

      void setOffsetTick(int offset);
      int getOffsetTick() const;

private:
      bool rest_;
      unsigned int note_;
      AccidentalType accidental_;
      bool showAccidental_;
      unsigned int onVelocity_;
      unsigned int offVelocity_;
      NoteHeadType headType_;
      TiePos tiePos_;
      int offsetStaff_;
      bool show_;
      int offsetTick_;//for playback
      };

class Articulation : public OffsetElement {
public:
      Articulation();
      virtual ~Articulation(){}

public:
      void setArtType(int type);//ArticulationType
      ArticulationType getArtType() const;

      void setPlacementAbove(bool above);
      bool getPlacementAbove() const;

      // for midi
      bool willAffectNotes() const;

      static bool isTrill(ArticulationType type);

      // for xml
      enum class XmlType : char {
            Articulation,
            Technical,
            Arpeggiate,
            Ornament,
            Fermata,
            Direction,

            Unknown
            };
      XmlType getXmlType() const;

      // sound setting
      bool getChangeSoundEffect() const;
      void setSoundEffect(int soundFrom, int soundTo);
      QPair<int, int> getSoundEffect() const;

      bool getChangeLength() const;
      void setLengthPercentage(int percentage);
      int getLengthPercentage() const;

      bool getChangeVelocity() const;
      enum class VelocityType : char {
            Offset,
            SetValue,
            Percentage
            };
      void setVelocityType(VelocityType type);
      VelocityType getVelocityType() const;

      void setVelocityValue(int value);
      int getVelocityValue() const;

      bool getChangeExtraLength() const;
      void setExtraLength(int length);
      int getExtraLength() const;

      // trill
      enum class TrillInterval : char {
            Diatonic = 0,
            Chromatic,
            Whole
            };
      void setTrillInterval(int interval);
      TrillInterval getTrillInterval() const;

      void setAuxiliaryFirst(bool first);
      bool getAuxiliaryFirst() const;

      void setTrillRate(NoteType rate);
      NoteType getTrillRate() const;

      void setTrillNoteLength(int length);
      int getTrillNoteLength() const;

      enum class AccelerateType : char {
            None = 0 ,
            Slow,
            Normal,
            Fast
            };
      void setAccelerateType(int type);
      AccelerateType getAccelerateType() const;

private:
      ArticulationType type_;
      bool above_;

      bool changeSoundEffect_;
      QPair<int, int> soundEffect_;
      bool changeLength_;
      int lengthPercentage_;
      bool changeVelocity_;
      VelocityType velocityType_;
      int velocityValue_;
      bool changeExtraLength_;
      int extraLength_;

      // trill
      TrillInterval trillInterval_;
      bool auxiliaryFirst_;
      NoteType trillRate_;
      int trillNoteLength_;
      AccelerateType accelerateType_;
      };

class NoteContainer : public MusicData, public LengthElement {
public:
      NoteContainer();
      virtual ~NoteContainer();

public:
      void setIsGrace(bool grace);
      bool getIsGrace() const;

      void setIsCue(bool cue);
      bool getIsCue() const;

      void setIsRest(bool rest/*or note*/);
      bool getIsRest() const;

      void setIsRaw(bool raw);
      bool getIsRaw() const;

      void setNoteType(NoteType type);
      NoteType getNoteType() const;

      void setDot(int dot);
      int getDot() const;

      void setGraceNoteType(NoteType type);
      NoteType getGraceNoteType() const;

      void setInBeam(bool in);
      bool getInBeam() const;

      void setStemUp(bool up);
      bool getStemUp(void) const;

      void setShowStem(bool show);
      bool getShowStem() const;

      void setStemLength(int line);
      int getStemLength() const;

      void setTuplet(int tuplet);
      int getTuplet() const;

      void setSpace(int space);
      int getSpace() const;

      void addNoteRest(Note* note);
      QList<Note*> getNotesRests() const;

      void addArticulation(Articulation* art);
      QList<Articulation*> getArticulations() const;

      void setNoteShift(int octave);
      int getNoteShift() const;

      int getOffsetStaff() const;

      int getDuration() const;

private:
      bool grace_;
      bool cue_;
      bool rest_;
      bool raw_;
      NoteType noteType_;
      int dot_;
      NoteType graceNoteType_;
      int tuplet_;
      int space_;
      bool inBeam_;
      bool stemUp_;
      bool showStem_;
      int stemLength_;	// line count span
      int noteShift_;

      QList<Note*> notes_;
      QList<Articulation*> articulations_;
      };

class Beam : public MusicData, public PairEnds {
public:
      Beam();
      virtual ~Beam(){}

public:
      void setIsGrace(bool grace);
      bool getIsGrace() const;

      void addLine(const MeasurePos& startMp, const MeasurePos& endMp);
      const QList<QPair<MeasurePos, MeasurePos> > getLines() const;

private:
      bool grace_;
      QList<QPair<MeasurePos, MeasurePos> > lines_;
      };

class Tie : public MusicData, public PairEnds {
public:
      Tie();
      virtual ~Tie(){}

public:
      void setShowOnTop(bool top);
      bool getShowOnTop() const;

      void setNote(int note);// note value tie point to
      int getNote() const;

      void setHeight(int height);
      int getHeight() const;

private:
      bool showOnTop_;
      int note_;
      int height_;
      };

class Glissando : public MusicData, public PairEnds {
public:
      Glissando();
      virtual ~Glissando(){}

public:
      void setStraightWavy(bool straight);
      bool getStraightWavy() const;

      void setText(const QString& text);
      QString getText() const;

      void setLineThick(int thick);
      int getLineThick() const;

private:
      bool straight_;
      QString text_;
      int lineThick_;
      };

class Decorator : public MusicData {
public:
      Decorator();
      virtual ~Decorator(){}

public:
      enum class Type : char {
            Dotted_Barline = 0,
            Articulation
            };
      void setDecoratorType(Type type);
      Type getDecoratorType() const;

      void setArticulationType(ArticulationType type);
      ArticulationType getArticulationType() const;

private:
      Type decoratorType_;
      ArticulationType artType_;
      };

class MeasureRepeat : public MusicData {
public:
      MeasureRepeat();
      virtual ~MeasureRepeat(){}

public:
      void setSingleRepeat(bool single); // false : double
      bool getSingleRepeat() const;

private:
      bool singleRepeat_;
      };

class Tuplet : public MusicData, public PairEnds {
public:
      Tuplet();
      virtual ~Tuplet();

public:
      void setTuplet(int tuplet=3);
      int getTuplet() const;

      void setSpace(int space=2);
      int getSpace() const;

      void setHeight(int height);
      int getHeight() const;

      void setNoteType(NoteType type);
      NoteType getNoteType() const;

      OffsetElement* getMarkHandle() const;

private:
      int tuplet_;
      int space_;
      int height_;
      NoteType noteType_;
      OffsetElement* mark_;
      };

class Harmony : public MusicData, public LengthElement {
public:
      Harmony();
      virtual ~Harmony(){}

public:
      void setHarmonyType(QString type);
      QString getHarmonyType() const;

      void setRoot(int root=0);//C
      int getRoot() const;

      void setBass(int bass);
      int getBass() const;

      void setAlterRoot(int val);
      int getAlterRoot() const;

      void setAlterBass(int val);
      int getAlterBass() const;

      void setBassOnBottom(bool on);
      bool getBassOnBottom() const;

      void setAngle(int angle);
      int getAngle() const;

private:
      QString harmonyType_;
      int root_;
      int bass_;
      int alterRoot_;
      int alterBass_;
      bool bassOnBottom_;
      int angle_;
      };

class Clef : public MusicData, public LineElement {
public:
      Clef();
      virtual ~Clef(){}

public:
      void setClefType(int type);	// ClefType
      ClefType getClefType() const;

private:
      ClefType clefType_;
      };

class Lyric : public MusicData {
public:
      Lyric();
      virtual ~Lyric(){}

public:
      void setLyric(const QString& lyricText);
      QString getLyric() const;

      void setVerse(int verse);
      int getVerse() const;

private:
      QString lyric_;
      int verse_;
      };

class Slur: public MusicData, public PairEnds {
public:
      Slur();
      virtual ~Slur();

public:
      void setContainerCount(int count); // span
      int getContainerCount() const;

      void setShowOnTop(bool top);
      bool getShowOnTop() const;

      OffsetElement* getHandle2() const;
      OffsetElement* getHandle3() const;

      void setNoteTimePercent(int percent); // 50% ~ 200%
      int getNoteTimePercent() const;

private:
      int containerCount_;
      bool showOnTop_;
      int noteTimePercent_;
      OffsetElement* handle_2_;
      OffsetElement* handle_3_;
      };

class Dynamics: public MusicData {
public:
      Dynamics();
      virtual ~Dynamics() {}

public:
      void setDynamicsType(int type);//DynamicsType
      DynamicsType getDynamicsType() const;

      void setIsPlayback(bool play);
      bool getIsPlayback() const;

      void setVelocity(int vel);
      int getVelocity() const;

private:
      DynamicsType dynamicsType_;
      bool playback_;
      int velocity_;
      };

class WedgeEndPoint: public MusicData {
public:
      WedgeEndPoint();
      virtual ~WedgeEndPoint() {}

public:
      void setWedgeType(WedgeType type);
      WedgeType getWedgeType() const;

      void setHeight(int height);
      int getHeight() const;

      void setWedgeStart(bool wedgeStart);
      bool getWedgeStart() const;

private:
      int height_;
      WedgeType wedgeType_;
      bool wedgeStart_;
      };

class Wedge: public MusicData {
public:
      Wedge();
      virtual ~Wedge() {}

public:
      void setWedgeType(WedgeType type);
      WedgeType getWedgeType() const;

      void setHeight(int height);
      int getHeight() const;

private:
      int height_;
      WedgeType wedgeType_;
      };

class Pedal: public MusicData, public PairEnds {
public:
      Pedal();
      virtual ~Pedal();

public:
      void setHalf(bool half);
      bool getHalf() const;

      void setIsPlayback(bool playback);
      bool getIsPlayback() const;

      void setPlayOffset(int offset); // -127~127
      int getPlayOffset() const;

      OffsetElement* getPedalHandle() const; //only on half pedal

private:
      bool half_;
      bool playback_;
      int playOffset_;
      OffsetElement* pedalHandle_;
      };

class KuoHao: public MusicData, public PairEnds {
public:
      KuoHao();
      virtual ~KuoHao() {}

public:
      void setHeight(int height);
      int getHeight() const;

      void setKuohaoType(int type);// KuoHaoType
      KuoHaoType getKuohaoType() const;

private:
      int height_;
      KuoHaoType kuohaoType_;
      };

class Expressions: public MusicData {
public:
      Expressions();
      virtual ~Expressions() {}

public:
      void setText(const QString& str);
      QString getText() const;

private:
      QString text_;
      };

class HarpPedal: public MusicData {
public:
      HarpPedal();
      virtual ~HarpPedal() {}

public:
      void setShowType(int type);//0:graph, 1:char, 2:char cut, 3:change
      int getShowType() const;

      void setShowCharFlag(int flag);//each bit is a bool, total 7 bools
      int getShowCharFlag() const;

private:
      int showType_;
      int showCharFlag_;
      };

class OctaveShift: public MusicData, public LengthElement {
public:
      OctaveShift();
      virtual ~OctaveShift() {}

public:
      void setOctaveShiftType(OctaveShiftType type);
      OctaveShiftType getOctaveShiftType() const;

      void setOctaveShiftPosition(OctaveShiftPosition position);
      OctaveShiftPosition getOctaveShiftPosition() const;

      int getNoteShift() const;

      void setEndTick(int tick);
      int getEndTick() const;

private:
      OctaveShiftType octaveShiftType_;
      OctaveShiftPosition octaveShiftPosition_;
      int endTick_;
      };

class OctaveShiftEndPoint: public MusicData, public LengthElement {
public:
      OctaveShiftEndPoint();
      virtual ~OctaveShiftEndPoint() {}

public:
      void setOctaveShiftType(OctaveShiftType type);
      OctaveShiftType getOctaveShiftType() const;

      void setOctaveShiftPosition(OctaveShiftPosition position);
      OctaveShiftPosition getOctaveShiftPosition() const;

      void setEndTick(int tick);
      int getEndTick() const;

private:
      OctaveShiftType octaveShiftType_;
      OctaveShiftPosition octaveShiftPosition_;
      int endTick_;
      };

class MultiMeasureRest: public MusicData {
public:
      MultiMeasureRest();
      virtual ~MultiMeasureRest() {}

public:
      void setMeasureCount(int count);
      int getMeasureCount() const;

private:
      int measureCount_;
      };

class Tempo: public MusicData {
public:
      Tempo();
      virtual ~Tempo() {}

public:
      void setLeftNoteType(int type);//NoteType
      NoteType getLeftNoteType() const;

      void setShowMark(bool show);
      bool getShowMark() const;

      void setShowBeforeText(bool show);
      bool getShowBeforeText() const;

      void setShowParenthesis(bool show);
      bool getShowParenthesis() const;

      void setTypeTempo(double tempo); //0x2580 = 96.00
      double getTypeTempo() const;
      double getQuarterTempo() const;

      void setLeftText(const QString& str);// string at left of the mark
      QString getLeftText() const;

      void setRightText(const QString& str);
      QString getRightText() const;

      void setSwingEighth(bool swing);
      bool getSwingEighth() const;

      void setRightNoteType(int type);
      NoteType getRightNoteType() const;

      void setLeftNoteDot(bool showDot);
      bool getLeftNoteDot() const;

      void setRightNoteDot(bool showDot);
      bool getRightNoteDot() const;

      void setRightSideType(int type);
      int getRightSideType() const;

private:
      int leftNoteType_;
      bool showMark_;
      bool showText_;
      bool showParenthesis_;
      double typeTempo_;
      QString leftText_;
      QString rightText_;
      bool swingEighth_;
      int rightNoteType_;
      bool leftNoteDot_;
      bool rightNoteDot_;
      int rightSideType_;
      };

class Text: public MusicData, public LengthElement {
public:
      Text();
      virtual ~Text() {}

public:
      enum class Type : char {
            Rehearsal,
            SystemText,
            MeasureText
            };

      void setTextType(Type type);
      Type getTextType() const;

      void setHorizontalMargin(int margin);
      int getHorizontalMargin() const;

      void setVerticalMargin(int margin);
      int getVerticalMargin() const;

      void setLineThick(int thick);
      int getLineThick() const;

      void setText(const QString& text);
      QString getText() const;

      void setWidth(int width);
      int getWidth() const;

      void setHeight(int height);
      int getHeight() const;

private:
      Type textType_;
      int horiMargin_;
      int vertMargin_;
      int lineThick_;
      QString text_;
      int width_;
      int height_;
      };

///////////////////////////////////////////////////////////////////////////////

class TimeSignature: public MusicData {
public:
      TimeSignature();
      virtual ~TimeSignature() {}

public:
      void setNumerator(int numerator);
      int getNumerator() const;

      void setDenominator(int denominator);
      int getDenominator() const;

      void setIsSymbol(bool symbol); //4/4:common, 2/2:cut
      bool getIsSymbol() const;

      void setBeatLength(int length); // tick
      int getBeatLength() const;

      void setBarLength(int length); // tick
      int getBarLength() const;

      void addBeat(int startUnit, int lengthUnit, int startTick);
      void endAddBeat();
      int getUnits() const;

      void setReplaceFont(bool replace);
      bool getReplaceFont() const;

      void setShowBeatGroup(bool show);
      bool getShowBeatGroup() const;

      void setGroupNumerator1(int numerator);
      void setGroupNumerator2(int numerator);
      void setGroupNumerator3(int numerator);
      void setGroupDenominator1(int denominator);
      void setGroupDenominator2(int denominator);
      void setGroupDenominator3(int denominator);

      void setBeamGroup1(int count);
      void setBeamGroup2(int count);
      void setBeamGroup3(int count);
      void setBeamGroup4(int count);

      void set16thBeamCount(int count);
      void set32thBeamCount(int count);

private:
      int numerator_;
      int denominator_;
      bool isSymbol_;
      int beatLength_;
      int barLength_;

      struct BeatNode {
            int startUnit_;
            int lengthUnit_;
            int startTick_;

            BeatNode() :
                  startUnit_(0),
                  lengthUnit_(0),
                  startTick_(0) {
                  }
            };
      QList<BeatNode> beats_;
      int barLengthUnits_;

      bool replaceFont_;
      bool showBeatGroup_;

      int groupNumerator1_;
      int groupNumerator2_;
      int groupNumerator3_;
      int groupDenominator1_;
      int groupDenominator2_;
      int groupDenominator3_;

      int beamGroup1_;
      int beamGroup2_;
      int beamGroup3_;
      int beamGroup4_;

      int beamCount16th_;
      int beamCount32th_;
      };

class Key: public MusicData {
public:
      Key();
      virtual ~Key() {}

public:
      void setKey(int key); //C=0x0, G=0x8, C#=0xE, F=0x1, Db=0x7
      int getKey() const;
      bool getSetKey() const;

      void setPreviousKey(int key);
      int getPreviousKey() const;

      void setSymbolCount(int count);
      int getSymbolCount() const;

private:
      int key_;
      bool set_;
      int previousKey_;
      int symbolCount_;
      };

class RepeatSymbol: public MusicData {
public:
      RepeatSymbol();
      virtual ~RepeatSymbol() {}

public:
      void setText(const QString& text);
      QString getText() const;

      void setRepeatType(int repeatType);
      RepeatType getRepeatType() const;

private:
      QString text_;
      RepeatType repeatType_;
      };

class NumericEnding: public MusicData, public PairEnds {
public:
      NumericEnding();
      virtual ~NumericEnding();

public:
      OffsetElement* getNumericHandle() const;

      void setHeight(int height);
      int getHeight() const;

      void setText(const QString& text);
      QString getText() const;
      QList<int> getNumbers() const;
      int getJumpCount() const;

private:
      int height_;
      QString text_;
      OffsetElement* numericHandle_;
      };

class BarNumber: public MusicData {
public:
      BarNumber();
      virtual ~BarNumber() {}

public:
      void setIndex(int index);
      int getIndex() const;

      void setShowOnParagraphStart(bool show);
      bool getShowOnParagraphStart() const;

      void setAlign(int align);// 0:left, 1:center, 2:right
      int getAlign() const;

      void setShowFlag(int flag); // 0:page, 1:staff, 2:bar, 3:none
      int getShowFlag() const;

      void setShowEveryBarCount(int count);
      int getShowEveryBarCount() const;

      void setPrefix(const QString& str);
      QString getPrefix() const;

private:
      int index_;
      bool showOnParagraphStart_;
      int align_;
      int showFlag_;
      int barRange_;
      QString prefix_;
      };

///////////////////////////////////////////////////////////////////////////////
// MIDI
class MidiController: public MidiData {
public:
      MidiController();
      virtual ~MidiController() {}

public:
      void setController(int number);
      int getController() const;

      void setValue(int value);
      int getValue() const;

private:
      int controller_;
      int value_;
      };

class MidiProgramChange: public MidiData {
public:
      MidiProgramChange();
      virtual ~MidiProgramChange() {}

public:
      void setPatch(int patch);
      int getPatch() const;

private:
      int patch_;
      };

class MidiChannelPressure: public MidiData {
public:
      MidiChannelPressure();
      virtual ~MidiChannelPressure() {}

public:
      void setPressure(int pressure);
      int getPressure() const;

private:
      int pressure_;
      };

class MidiPitchWheel: public MidiData {
public:
      MidiPitchWheel();
      virtual ~MidiPitchWheel() {}

public:
      void setValue(int value);
      int getValue() const;

private:
      int value_;
      };

///////////////////////////////////////////////////////////////////////////////
class Measure: public LengthElement {
public:
      Measure(int index = 0);
      virtual ~Measure();

private:
      Measure();

public:
      BarNumber* getBarNumber() const;
      TimeSignature* getTime() const;

      void setLeftBarline(int barline/*in BarLineType*/);
      BarLineType getLeftBarline() const;

      void setRightBarline(int barline/*in BarLineType*/);
      BarLineType getRightBarline() const;

      // set when rightBarline == Baline_Backward
      void setBackwardRepeatCount(int repeatCount);
      int getBackwardRepeatCount() const;

      void setTypeTempo(double tempo);
      double getTypeTempo() const;

      void setIsPickup(bool pickup);
      bool getIsPickup() const;

      void setIsMultiMeasureRest(bool rest);
      bool getIsMultiMeasureRest() const;

      void setMultiMeasureRestCount(int count);
      int getMultiMeasureRestCount() const;

private:
      void clear();

      BarNumber* barNumber_;
      TimeSignature* time_;

      BarLineType leftBarline_;
      BarLineType rightBarline_;
      int repeatCount_;
      double typeTempo_; // based on some type
      bool pickup_;
      bool multiMeasureRest_;
      int multiMeasureRestCount_;
      };

class MeasureData {
public:
      MeasureData();
      ~MeasureData();

public:
      Clef* getClef() const;
      Key* getKey() const;

      void addNoteContainer(NoteContainer* ptr);
      QList<NoteContainer*> getNoteContainers() const;

      // put Tempo, Text, RepeatSymbol to MeasureData at part=0 && staff=0
      void addMusicData(MusicData* ptr);
      // if type==MusicData_None, return all
      QList<MusicData*> getMusicDatas(MusicDataType type);//MusicXml: note|direction|harmony

      // put NumericEnding to MeasureData at part=0 && staff=0
      void addCrossMeasureElement(MusicData* ptr, bool start);
      enum class PairType : char {
            Start,
            Stop,
            All
            };
      QList<MusicData*> getCrossMeasureElements(MusicDataType type, PairType pairType);

      // for midi
      void addMidiData(MidiData* ptr);
      QList<MidiData*> getMidiDatas(MidiType type);

private:
      Key* key_;
      Clef* clef_;
      QList<MusicData*> musicDatas_;
      QList<NoteContainer*> noteContainers_;
      QList<QPair<MusicData*, bool> > crossMeasureElements_;
      QList<MidiData*> midiDatas_;
      };
      

}

#endif