/*
 * PWM out PIN3
 * 
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include "envtone_def.h"


//#define USE_DAC 
#define TABLE_SIZE_MASK 0x3f

#define ENVSTEP  4    //envelope counter incrimental   4 =  1 * 4
#define MAX_LEVEL  31

/* offset from struct head */
#define SIN_POSL 0
#define SIN_POSH 1
#define SIN_STEPL 2
#define SIN_STEPH 3
#define VOL     4
#define ENV_STATE 5
#define ENV_CNT   6
#define ATK   7
#define DECY  8
#define SUL   9
#define SUS   10
#define REL   11
#define LEVEL 12
#define WAVE_TBLL 13
#define WAVE_TBLH 14
#define ENV_STEP  15

#define SIZEOF_MEMTONE 16

#define F_ATK  1
#define F_DECY 2
#define F_SUS  3
#define F_REL  4

#define TOTAL_OUT r18
#define CHANNEL_OUT r19

.section .data
wait_cnt: .dc.b  0        //エンベロープのウェイト用カウンタ



#ifdef USE_ECHO
w_ph:  .dc.b 0
w_pl:  .dc.b 0
.global echo_mul
echo_mul: .dc.b 20
.section .bss
d_buff: .ds.b   BUF_SIZE
#endif


.section .text

.global TIMER1_COMPA_vect
TIMER1_COMPA_vect:
.func
  cli
  push r0
  push r1
  in r0,_SFR_IO_ADDR(SREG)
  push r0
  push XL
  push XH
  push YL
  push YH
  push ZL
  push ZH
  push r16
  push r17
  push TOTAL_OUT
  push CHANNEL_OUT
  push r20


#ifdef USE_DAC
  cbi 0x03, 0x02
  ldi ZL, 0x36
  out _SFR_IO_ADDR(SPDR), ZL 
#endif

 ldi XL,MAX_TONE - 1     ;loop conter
 clr TOTAL_OUT
 lds r20,wait_cnt
inc r20 
  andi r20,0x07
  sts wait_cnt,r20
//
  ldi YL,lo8(memtone)
  ldi YH,hi8(memtone)

loop:

  ldd ZL, Y+SIN_POSH     //Zレジスタを逆に使い　読み出しポインタ加算
  ldd ZH, Y+SIN_POSL
  ldd r16,Y+SIN_STEPH
  ldd r17,Y+SIN_STEPL
  add ZH,r17
  adc ZL,r16

  std Y + SIN_POSH, ZL
  std Y + SIN_POSL, ZH
  andi ZL,TABLE_SIZE_MASK
  
  clr ZH                  //Read sin table from tone pointer

  ldd r16,Y+WAVE_TBLL
  ldd r17,Y+WAVE_TBLH
  add ZL,r16
  adc ZH,r17
  
  ld CHANNEL_OUT,Z
  //lpm CHANNEL_OUT,Z         //波形をPROGMEMで置いている場合


//------- envelope   --- r16  level  r17 env_cnt  ----------------------
  ldd r16,Y + LEVEL
   cp r20,XL
   breq DO_ENVCHG
   rjmp ENV_LEVEL   
  
DO_ENVCHG:

  ldd r17,Y + ENV_CNT
  ldd XH,Y + ENV_STEP
  sub r17,XH
  brcs DO_ENVSTEP
  std Y + ENV_CNT , r17
  rjmp ENV_LEVEL

DO_ENVSTEP:
  ldd XH,Y + ENV_STATE
  
  cpi XH, F_ATK
  breq ATK_FASE

  cpi XH, F_DECY
  breq DECY_FASE

  cpi XH,F_SUS
  breq SUS_FASE
  
  
REL_FASE:
  ldi r17,250
  std Y+ENV_CNT,r17
  cpi r16,(0)
  breq STOP_TONE
  //dec r16
  subi r16,ENVSTEP
  std Y+LEVEL,r16
  rjmp ENV_LEVEL

 DECY_FASE:
  ldi r17,250
  std Y+ENV_CNT,r17
  //dec r16
  subi r16,ENVSTEP
  std Y+LEVEL,r16
  ldd XH,Y+SUL   //compare sustain level
  cp r16,XH
  brne ENV_LEVEL
  ldi r17,F_SUS         //set sustain fase
  std Y+ENV_STATE,r17
  ldd XH,Y + SUS
  std Y + ENV_STEP,XH
  rjmp ENV_LEVEL

SUS_FASE:
  ldi r17,250
  std Y+ENV_CNT,r17
  cpi r16,(0)
  breq STOP_TONE 
  //dec r16
  subi r16,ENVSTEP
  std Y+LEVEL,r16
  rjmp ENV_LEVEL 

STOP_TONE:
  clr r17
  std Y+SIN_STEPL,r17
  std Y+SIN_STEPH,r17 
  rjmp ENV_LEVEL


ATK_FASE:
  ldi r17,250
  std Y+ENV_CNT,r17
//  inc r16
  subi r16,(-(ENVSTEP))
  std Y+LEVEL,r16
  cpi r16,(MAX_LEVEL*ENVSTEP)
  brne ENV_LEVEL
 ldi r17,F_DECY       // set decay fase
  std Y+ENV_STATE,r17
  ldd XH,Y + DECY
  std Y + ENV_STEP,XH
//  rjmp ENV_LEVEL



ENV_LEVEL:  

/*
 *    CHANNEL_OUT is already multiply (32*32)/(31*31) *2  
 *    VOL is already multiply 8
 *    r16 multiply 4
 *    CHANNEL_OUT * ( VOL * 8) * (r16 * 4)) / 256 / 256
 *    
 *    (CHANNEL_OUT_SOURCE)* VOL * r16 /(31*31)
 *   
  */
  ldd XH,Y + VOL 
  mul r16,XH
  mov r16,r1
  muls CHANNEL_OUT,r16
  mov CHANNEL_OUT,r1



/* cliping output level if signed 8bit overflow */
#ifdef CLIP
  sbrc TOTAL_OUT,7
  rjmp IF_MINUS
  
IF_PLUS:
  add TOTAL_OUT,CHANNEL_OUT
  sbrc CHANNEL_OUT,7
  rjmp END_CLIP
  
  sbrc TOTAL_OUT,7
  ldi TOTAL_OUT,127
  rjmp END_CLIP
  
IF_MINUS:
  add TOTAL_OUT,CHANNEL_OUT
  sbrs CHANNEL_OUT,7
  rjmp END_CLIP
  
  sbrs TOTAL_OUT,7  
  ldi  TOTAL_OUT,(-127)

END_CLIP:
#else
  add TOTAL_OUT,CHANNEL_OUT
#endif

    subi YL,lo8(-(SIZEOF_MEMTONE))
    sbci YH,hi8(-(SIZEOF_MEMTONE))


  subi XL,(1)
  brcs  endloop
  rjmp loop
endloop:



#ifdef USE_ECHO

 // echo 
  lds YL,w_pl
  lds YH,w_ph
  adiw YL,1

  cpi YH,hi8(BUF_SIZE)
  brne save
  cpi YL,lo8(BUF_SIZE)
  brne save
  clr YH
  clr YL

save:
  sts w_pl,YL
  sts w_ph,YH
  subi YL,lo8(-(d_buff))
  sbci YH,hi8(-(d_buff))
  
  ld CHANNEL_OUT,Y
  lds r20,echo_mul

  tst CHANNEL_OUT
  brmi MUL_MINUS
  
  MULSU CHANNEL_OUT,r20
  mov CHANNEL_OUT,r1
  rjmp CLIP2

MUL_MINUS:

  MULSU CHANNEL_OUT,r20
  mov CHANNEL_OUT,r1
  neg CHANNEL_OUT
  

CLIP2:
  sbrc TOTAL_OUT,7
  rjmp E_IF_MINUS
  
E_IF_PLUS:
  add TOTAL_OUT,CHANNEL_OUT
  sbrc CHANNEL_OUT,7
  rjmp E_END_CLIP
  
  sbrc TOTAL_OUT,7
  ldi TOTAL_OUT,127
  rjmp E_END_CLIP
  
E_IF_MINUS:
  add TOTAL_OUT,CHANNEL_OUT
  sbrs CHANNEL_OUT,7
  rjmp E_END_CLIP
  
  sbrs TOTAL_OUT,7  
  ldi  TOTAL_OUT,(-127)

E_END_CLIP:


  st Y,TOTAL_OUT   

#endif   



  subi TOTAL_OUT, (-128)
  
 #ifdef USE_DAC
  out _SFR_IO_ADDR(SPDR), TOTAL_OUT
 #endif

/* PWM OUTPUT */
#if (PWM_PIN == 3)
  sts OCR2B, TOTAL_OUT
#elif (PWM_PIN == 11)
  sts OCR2A, TOTAL_OUT
#elif (PWM_PIN == 5)
  sts OCR0B, TOTAL_OUT
#elif( PWM_PIN == 6)
  sts OCR0A, TOTAL_OUT
#elif (PWM_PIN == 13)
  sts OCR4A, TOTAL_OUT
  subi TOTAL_OUT, (-128)  
  sts UDR1,TOTAL_OUT
#endif


  pop r20
  pop CHANNEL_OUT
  pop TOTAL_OUT
  pop r17
  pop r16
  pop ZH
  pop ZL
  pop YH
  pop YL
  pop XH
  pop XL
  pop r0
  out _SFR_IO_ADDR(SREG), r0
  pop r1
  pop r0
#ifdef USE_DAC  
  sbi 0x03, 0x02;
#endif  
  sei
  reti

.endfunc


