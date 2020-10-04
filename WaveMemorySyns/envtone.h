#include "memtone.h"
#include "envtone_def.h"

#define WAVE_TBL_SIZE  64

enum midistat {MIDI_POLY, MIDI_MONO};
enum deftonename {TONE_SPACE, TONE_SAX, TONE_FLUTE, TONE_PIANO};
enum envtype {ATK, DECY, SL, SR, RR, ECHO};

class EnvTone
{
  protected:

  private:
    /* MIDI ポリフォニック発音割り振り用　*/
    uint8_t queue_cnt = 0;
    uint8_t voice_queue[MAX_TONE];
    uint8_t voice_queue_top = 0;
    uint8_t voice_queue_tail = 0;

    uint8_t ch_midino[MAX_TONE];
    uint8_t active_voice_num = 0;

  public:
    EnvTone();
    setup_hardware();
    note_on(uint8_t, uint8_t, uint8_t);
    note_off(uint8_t );
    midi_command(uint8_t com, char dat1, char dat2, char dat3);
    midi_set_envelope(uint8_t atk, uint8_t decy, uint8_t sul, uint8_t sus, uint8_t rel);
    set_midistate(midistat);

    set_envelope(uint8_t , uint8_t atk, uint8_t decy, uint8_t sul, uint8_t sus, uint8_t rel);

    change_envelope(uint8_t ch, uint8_t type, uint8_t val);
    change_wavevalue(uint8_t p, uint8_t val);

    midi_setTone(uint8_t source[]);
    setTone(uint8_t ch, uint8_t source[]);
    defTone(uint8_t no);
    setEchoMul(uint8_t mul);
  private:
    midi_note_off(uint8_t);
    midistat midi_state = MIDI_POLY;

};

extern EnvTone ENVTONE;




