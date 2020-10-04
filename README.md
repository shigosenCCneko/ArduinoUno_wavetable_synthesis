# ArduinoUno  波形メモリ音源
ArduinoUnoで波形メモリ音源を実現するライブラリとスケッチ例

##  仕様

Timer1を使いサンプリングサンプル数64の波形データを
Timer0かTimer2を使ったPWMから出力します  
発音数や出力ポート、サンプリング周波数等の各種設定は**envtone_def.h**で定義しています。

8音発声時

|効果|最大周波数|
|--|--|
|CLIP&ECHO|28KHz|
|CLIP|30KHz|
|NONE|34KHz|

簡単な外部スピーカーなら24KHzで十分だと思います。

### ECHO
envtone_def.hでECHOを有効にする事で任意のバッファサイズを使用してECHOが可、
ディレイはバッファサイズとサンプリング周波数で変わります。

#define USE_ECHO   //エコーの使用

#define BUF_SIZE 500    //500to 1500ぐらい、大きくするとディレイが増えます
#define ECHO_MUL 108    //減衰値、50~180ぐらい、小さい方が減衰が大きい

### 出力可能PIN

|pint No| Timer | 出力レジスタ|
|:--|    --|--:|
|PIN3  |Timer2|  OCR2B|
|PIN5  |Timer0  |OCR0B|
|PIN6  |Timer0  |OCR0A |
|PIN11 |Timer2  |OCR2A  |

Timer0を使用するとdelay()等の時間管理が使えなくなります  
 　

### プリセット音色
4つのプリセット音、ENVTONE.defTone(tone)で指定。

1. リード(TONE_SPACE)
2. サックス(TONE_SAX)
3. フルート(TONE_FLUTE)
4. ピアノ(TONE_PIANO)

### エンベロープ
    attack,
    Decey,
    Sutain level,
    Sustain,
    Release　
            sustain level  0 to 31  注:31が最大音量
            other          0 to 15

## インターフェイス

#### ENVTONE.setup_hardware()
    タイマ等ハードウェアの初期設定
#### ENVTONE.midistate(miditype)
    miditype:  
            MIDI_POLY  MIDIチャンネル1 ポリフォニックモード
            MIDI_MONO  モノフォニックモード

#### ENVTONE.midi_setenvelope(atk, decey, sustain level, sutain, release)
    ポリフォニックモード用に4chすべてのエンベロープを設定します

#### ENVTONE.defTone(uint8_t tone_no)
    ポリフォニックモードでプリセット音に設定。
    0. TONE_SPACE
    1. TONE_SAX
    2. TONE_FLUTE
    3. TONE_PIANO
####  ENVTONE.midi_setTone(uint8 *tone)
      ポリフォニックモードでユーザ定義音色に設定。

#### ENVTONE..midi_command(com, dat1, dat2, dat3)
    MIDIコマンド送信
    0x80 NOTE OFF
    0x90 NOTE ON
     のみ実装

### チャンネル指定

#### ENVTONE.set_envelope(ch, atk, decey, sustain level, sutain, release)
    指定chにエンヴェロープを設定
#### ENVTONE.setTone(uint8 ch, uint8 tone)
    指定チャンネルの音色設定
#### ENVTONE.note_on(ch, note no, vel)
    チャンネル指定NOTE_ON
#### ENVTONE.note_off(ch)
    チャンネル指定NOTE_OFF

## 音色データ

69byteの配列、最初5byteはエンベロープ残りは波形データです

\\WaveMemoryディレクトリ下にJavaのexeにRAPした波形編集ツールを付けていますので色々遊んでみて下さい。

## あとがき
Arduino Uno を使ったガジェット等でTONE()の音色じゃ物足りない時とか用に？  
環境によっては12音ぐらいまでは鳴りますが
メインの処理の重さに影響を受けるのでシリアルからMIDIストリームを受けて鳴らすと24KHzで8音ぐらい32KHzは4音ぐらいと思います。  

2020/6/19
アセンブラで詰めて32KHz5音は実用になりそうです  
2020/7/5
32KhzでCLIPしても6音実用になるようになりました。
2020/10/4
音色単位で波形とエンベロープを扱えるように変更、波形を前処理してかなり高速化しました。
