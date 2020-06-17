# ArduinoUno  波形メモリ音源
ArduinoUnoで波形メモリ音源を実現するライブラリとスケッチ例

##  仕様

Timer1を使いサンプリング周波数16KHz、サンプル数64の波形データを4チャンネル合成してTimer2を使ったPWM(Pinn3)から出力します
波形は各チャンネルごとに4種類から選択可
### 波形の種類
1. Sin Wave
2. FM音源2OPからのサンプリング
3. 矩形波
4. 三角波

### エンベロープ
attack,Decey,Sutain,Sustain level,Release　
sustain level  0 to 31  
other          0 to 15

## インターフェイス

#### ENVTONE.setup_hardware()
    タイマ等ハードウェアの設定
#### ENVTONE.midistate(midi type) 
    miditype:  
            MIDI_POLY  MIDIチャンネル1 4音ポリフォニックモード
            MIDI_MONO  MIDIチャンネル1～4　モノフォニックモード
#### ENVTONE.midi_setwafe(wavetype)
    wavetype:
            SIN     sin波
            FM2OP   FM音源2オペレータ波形からサンプリング
            RECT    矩形波
            TRI     三角波

    *波形データはenvtone.cppでcharの64byteの配列に置いていますので
    　適当なデータに置き換えて鳴らせます。
#### ENVTONE.midi_setenvelope(atk,decey,sustai level,sutain,release)
    ポリフォニックモード用に４ｃｈすべてのエンベロープを設定します

#### ENVTONE.set_wave(ch,wavetpe)
    指定chにwavetypeを設定

#### ENVTONE..midi_command(com, dat1, dat2, dat3)
    ミディ―コマンド送信
    0x80 NOTE OFF
    0x90 NOTE ON
     のみ実装

### チャンネル指定

#### ENVTONE.set_envelope(ch,atk,decey,sustai level,sutain,release)
    指定chにエンヴェロープを設定
#### ENVTONE.note_on(ch,note no,vel)
    チャンネル指定NOTE_ON
#### ENVTONE.note_off(ch)
    チャンネル指定NOTE_OFF

## あとがき
Arduino Uno を使ったガジェット等でTONE()の音色じゃ物足りないとかに
割り込み処理部にnopを400ぐらい埋め込んでも動いているので処理に余裕は有りそうです
同時発音数はenvtone.hとISR_WAVE.Sで#define MAX_TONEで指定していますので環境によっては８音ぐらいまでは鳴りますが
メインの処理の重さに影響を受けるのでシリアルからMIDIストリームを受けて10音ぐらいが限界でした。


    
    

