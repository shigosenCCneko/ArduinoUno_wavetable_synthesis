#include "memtone.h"

#define MAX_TONE 4
#define WAVE_TBL_SIZE  64





enum midistat{MIDI_POLY,MIDI_MONO};
enum wavetype {SIN, FM2OP, RECT, TRI};

class EnvTone
{

  protected:
    static const  char wave_sin[];
    static const  char wave_2OP[];
    static const  char wave_rect[];
    static const  char wave_tri[];

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
    midi_setwave(wavetype wave);
    midi_set_envelope(uint8_t atk, uint8_t decy, uint8_t sul, uint8_t sus, uint8_t rel);
    set_midistate(midistat);

    set_wave(uint8_t, wavetype );
    set_envelope(uint8_t , uint8_t atk, uint8_t decy, uint8_t sul, uint8_t sus, uint8_t rel);
  private:
    
    midi_note_off(uint8_t);
    midistat midi_state = MIDI_POLY;

};

extern EnvTone ENVTONE;


