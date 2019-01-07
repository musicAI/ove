#ifndef _OVE_ENUM_H
#define _OVE_ENUM_H

namespace OVE{

enum class CondType : char {
      None,
      Time_Parameters    = 0x09, // size - 7, TimeSignature
      Bar_Number         = 0x0A, // size, compatible with previous version
      Decorator          = 0x16,
      Tempo              = 0x1C, // size - 7
      Text               = 0x1D, // size - 7, Rehearsal | SystemText
      Expression         = 0x25, // size - 7, if set playback parameters in Expression, will store in COND
      Barline_Parameters = 0x30, // size - 7, include :|| repeat count
      Repeat             = 0x31, //
      Numeric_Ending     = 0x32,
      };

enum class BdatType : unsigned char {
      None,
      Raw_Note              = 0x70,
      Rest                  = 0x80,
      Note                  = 0x90,
      Beam                  = 0x10,
      Harmony               = 0x11,
      Clef                  = 0x12,
      Dynamics              = 0x13,
      Wedge                 = 0x14, // cresendo, decresendo
      Glissando             = 0x15,
      Decorator             = 0x16, // measure repeat | piano pedal | dotted barline
      Key                   = 0x17,
      Lyric                 = 0x18,
      Octave_Shift          = 0x19,
      Slur                  = 0x1B,
      Text                  = 0x1D,
      Tie                   = 0x1E,
      Tuplet                = 0x1F,
      Guitar_Bend           = 0x21, //
      Guitar_Barre          = 0x22, //
      Pedal                 = 0x23,
      KuoHao                = 0x24, // () [] {}
      Expressions           = 0x25,
      Harp_Pedal            = 0x26,
      Multi_Measure_Rest    = 0x27,
      Harmony_GuitarFrame   = 0x28,
      Graphics_40           = 0x40, // unknown
      Graphics_RoundRect    = 0x41,
      Graphics_Rect         = 0x42,
      Graphics_Round        = 0x43,
      Graphics_Line         = 0x44,
      Graphics_Curve        = 0x45,
      Graphics_WedgeSymbol  = 0x46,
      Midi_Controller       = 0xAB,
      Midi_Program_Change   = 0xAC,
      Midi_Channel_Pressure = 0xAD,
      Midi_Pitch_Wheel      = 0xAE,
      Bar_End               = 0xFF,
      };

////////////////////////////////////////

enum class MusicDataType : char {
      None,

      // attributes
      Clef,
      Key,
      Measure_Repeat,

      // sound
      Tempo,

      // direction
      Dynamics,
      Wedge,
      Wedge_EndPoint,
      OctaveShift,
      OctaveShift_EndPoint,
      Expressions,
      Repeat,
      Text,
      Harp_Pedal,
      Pedal,

      // note & harmony
      Note_Container,
      Harmony,

      // note's children
      Beam,
      Glissando,
      Lyric,
      Slur,
      Tie,
      Tuplet,

      // barline
      Numeric_Ending,

      KuoHao,
      Bar_End,
      Decorator,
      Multi_Measure_Rest,
      };

enum class MidiType : signed char {
      None = -1,
      Controller,
      Program_Change,
      Channel_Pressure,
      Pitch_Wheel,
      };

enum class ClefType : char {
      Treble = 0x00, //0x00
      Bass,          //0x01
      Alto,          //0x02
      UpAlto,        //0x03
      DownDownAlto,  //0x04
      DownAlto,      //0x05
      UpUpAlto,      //0x06
      Treble8va,     //0x07
      Bass8va,       //0x08
      Treble8vb,     //0x09
      Bass8vb,       //0x0A
      Percussion1,   //0x0B
      Percussion2,   //0x0C
      TAB            //0x0D
      };

enum class GroupType : char {
      None = 0,
      Brace,
      Bracket
      };

enum class AccidentalType : char {
      Normal              = 0x0,
      Sharp               = 0x1,
      Flat                = 0x2,
      Natural             = 0x3,
      DoubleSharp         = 0x4,
      DoubleFlat          = 0x5,
      Sharp_Caution       = 0x9,
      Flat_Caution        = 0xA,
      Natural_Caution     = 0xB,
      DoubleSharp_Caution = 0xC,
      DoubleFlat_Caution  = 0xD
      };

enum class NoteHeadType : char {
      Standard = 0x00,
      Invisible,
      Rhythmic_Slash,
      Percussion,
      Closed_Rhythm,
      Open_Rhythm,
      Closed_Slash,
      Open_Slash,
      Closed_Do,
      Open_Do,
      Closed_Re,
      Open_Re,
      Closed_Mi,
      Open_Mi,
      Closed_Fa,
      Open_Fa,
      Closed_Sol,
      Open_Sol,
      Closed_La,
      Open_La,
      Closed_Ti,
      Open_Ti
      };

enum class TiePos : char {
      None     = 0x0,
      LeftEnd  = 0x1,
      RightEnd = 0x2
      };

enum class ArticulationType : char {
      Major_Trill                  = 0x00,
      Minor_Trill                  = 0x01,
      Trill_Section                = 0x02,
      Inverted_Short_Mordent       = 0x03,
      Inverted_Long_Mordent        = 0x04,
      Short_Mordent                = 0x05,
      Turn                         = 0x06,
      Finger_1                     = 0x07,
      Finger_2                     = 0x08,
      Finger_3                     = 0x09,
      Finger_4                     = 0x0A,
      Finger_5                     = 0x0B,
      Flat_Accidental_For_Trill    = 0x0C,
      Sharp_Accidental_For_Trill   = 0x0D,
      Natural_Accidental_For_Trill = 0x0E,
      Marcato                      = 0x0F,
      Marcato_Dot                  = 0x10,
      Heavy_Attack                 = 0x11,
      SForzando                    = 0x12,
      SForzando_Dot                = 0x13,
      Heavier_Attack               = 0x14,
      SForzando_Inverted           = 0x15,
      SForzando_Dot_Inverted       = 0x16,
      Staccatissimo                = 0x17,
      Staccato                     = 0x18,
      Tenuto                       = 0x19,
      Up_Bow                       = 0x1A,
      Down_Bow                     = 0x1B,
      Up_Bow_Inverted              = 0x1C,
      Down_Bow_Inverted            = 0x1D,
      Arpeggio                     = 0x1E,
      Tremolo_Eighth               = 0x1F,
      Tremolo_Sixteenth            = 0x20,
      Tremolo_Thirty_Second        = 0x21,
      Tremolo_Sixty_Fourth         = 0x22,
      Natural_Harmonic             = 0x23,
      Artificial_Harmonic          = 0x24,
      Plus_Sign                    = 0x25,
      Fermata                      = 0x26,
      Fermata_Inverted             = 0x27,
      Pedal_Down                   = 0x28,
      Pedal_Up                     = 0x29,
      Pause                        = 0x2A,
      Grand_Pause                  = 0x2B,
      Toe_Pedal                    = 0x2C,
      Heel_Pedal                   = 0x2D,
      Toe_To_Heel_Pedal            = 0x2E,
      Heel_To_Toe_Pedal            = 0x2F,
      Open_String                  = 0x30, // finger 0 in guitar
      Guitar_Lift                  = 0x46,
      Guitar_Slide_Up              = 0x47,
      Guitar_Rip                   = 0x48,
      Guitar_Fall_Off              = 0x49,
      Guitar_Slide_Down            = 0x4A,
      Guitar_Spill                 = 0x4B,
      Guitar_Flip                  = 0x4C,
      Guitar_Smear                 = 0x4D,
      Guitar_Bend                  = 0x4E,
      Guitar_Doit                  = 0x4F,
      Guitar_Plop                  = 0x50,
      Guitar_Wow_Wow               = 0x51,
      Guitar_Thumb                 = 0x64,
      Guitar_Index_Finger          = 0x65,
      Guitar_Middle_Finger         = 0x66,
      Guitar_Ring_Finger           = 0x67,
      Guitar_Pinky_Finger          = 0x68,
      Guitar_Tap                   = 0x69,
      Guitar_Hammer                = 0x6A,
      Guitar_Pluck                 = 0x6B,

      None

      /*	Detached_Legato,
      Spiccato,
      Scoop,
      Plop,
      Doit,
      Falloff,
      Breath_Mark,
      Caesura,*/
      };

enum class NoteType : char {
      Note_DoubleWhole = 0x0,
      Note_Whole       = 0x1,
      Note_Half        = 0x2,
      Note_Quarter     = 0x3,
      Note_Eight       = 0x4,
      Note_Sixteen     = 0x5,
      Note_32          = 0x6,
      Note_64          = 0x7,
      Note_128         = 0x8,
      Note_256         = 0x9,

      Note_None
      };



enum class DynamicsType : char {
      PPPP = 0,
      PPP,
      PP,
      P,
      MP,
      MF,
      F,
      FF,
      FFF,
      FFFF,
      SF,
      FZ,
      SFZ,
      SFFZ,
      FP,
      SFP
      };

enum class WedgeType : char {
      Cres_Line = 0, // <
      Double_Line,   // <>, not appear in xml
      Decresc_Line,  // >
      Cres,          // cresc., not appear in xml, will create Expression
      Decresc        // decresc., not appear in xml, will create Expression
      };

enum class KuoHaoType : char {
      Parentheses = 0,
      Brace,
      Bracket
      };

enum class OctaveShiftType : char {
      OS_8 = 0,
      OS_Minus_8,
      OS_15,
      OS_Minus_15
      };

enum class OctaveShiftPosition : char {
      Start = 0 ,
      Continue,
      Stop
      };

enum class RepeatType : char {
      Segno = 0,
      Coda,
      ToCoda,
      DSAlCoda,
      DSAlFine,
      DCAlCoda,
      DCAlFine,
      Fine,

      Null
      };

enum class BarLineType : char {
      Default = 0, //0x00 will be | or final (at last measure)
      Double,      //0x01 ||
      RepeatLeft,  //0x02 ||:
      RepeatRight, //0x03 :||
      Final,       //0x04
      Dashed,      //0x05
      Null         //0x06
      } ;

enum class NoteDuration {
      D_256 = 15,
      D_128 = NoteDuration::D_256 * 2,           // 30
      D_64 = NoteDuration::D_128 * 2,            // 60
      D_32 = NoteDuration::D_64 * 2,             // 120
      D_16 = NoteDuration::D_32 * 2,             // 240
      D_8 = NoteDuration::D_16 * 2,              // 480
      D_4 = NoteDuration::D_8 * 2,               // 960
      D_2 = NoteDuration::D_4 * 2,               // 1920
      D_Whole = NoteDuration::D_2 * 2,           // 3840
      D_Double_Whole = NoteDuration::D_Whole * 2 // 7680
      };

enum class ToneType : char {
      C = 0,
      D,
      E,
      F,
      G,
      A,
      B
      };

enum class KeyType : char {
      Key_C = 0,		// C
      Key_Bass_1,		// F
      Key_Bass_2,		// Bb
      Key_Bass_3,		// Eb
      Key_Bass_4,		// Ab
      Key_Bass_5,		// Db
      Key_Bass_6,		// Gb
      Key_Bass_7,		// Cb
      Key_Sharp_1,	// G
      Key_Sharp_2,	// D
      Key_Sharp_3,	// A
      Key_Sharp_4,	// E
      Key_Sharp_5,	// B
      Key_Sharp_6,	// F#
      Key_Sharp_7		// C#
      };

enum class ChunkType : char {
      OVSC = 00 ,
      TRKL,
      TRAK,
      PAGL,
      PAGE,
      LINL,
      LINE,
      STAF,
      BARL,
      MEAS,
      COND,
      BDAT,
      PACH,
      FNTS,
      ODEV,
      TITL,
      ALOT,
      ENGR,
      FMAP,
      PCPR,

      // Overture 3.6
      LYRC,

      NONE
      };


enum class XmlDataType : char {
            Attributes = 0, 
            NoteBeam, 
            Notations, 
            Direction, 
            None
};


}



#endif
      