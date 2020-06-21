# ArduinoUno  波形メモリ音源
ArduinoUnoで波形メモリ音源を実現するライブラリとスケッチ例

##  仕様

Timer1を使いサンプリング周波数16KHz,32Khzサンプル数64の波形データを  
4チャンネル合成して、Timer0かTimer2を使ったPWMから出力します  
発音数や出力ポート等の各種設定は**envtone_def.h**で定義しています。

### 出力可能PIN
PinNo | Timer | 出力レジスタ|
:--   |    --:|        --:|
PIN3  |  Timer2|  OCR2B|
PIN5  |Timer0  |OCR0B|
PIN6  |Timer0  |OCR0A |
PIN11 |Timer2  |OCR2A  |
 Timer0を使用するとdelay()等の時間管理が使えなくなります  
### サンプリング周波数
サンプリング周波数はデフォルト16KHz 可能同時発音数が12音ぐらい  
　32KHz設定時には同時発音が5～6音になります 　

### 波形の種類
波形は各チャンネルごとに５種類から選択可
1. Sin Wave
2. FM音源2OPからのサンプリング
3. 矩形波
4. 三角波
5. FM音源からのサンプリング

### エンベロープ
    attack,
    Decey,
    Sutain level,
    Sustain,
    Release　
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
            FM2OP2  管楽器ぽい音

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
Arduino Uno を使ったガジェット等でTONE()の音色じゃ物足りないとかにいかが  
16KHzの時は割り込み処理部にnopを400ぐらい埋め込んでも動いているので処理に余裕は有りそうです 、32KHzになるとnopが80ぐらいなので重い処理はどうかと？ 
同時発音数とサンプリング周波数はenvtoe_def.hで定義し直しました  
環境によっては12音ぐらいまでは鳴りますが
メインの処理の重さに影響を受けるのでシリアルからMIDIストリームを受けて鳴らすと16KHzで10音ぐらい32KHzは4音が限界でした。  

2020/6/19 
アセンブラで詰めて32KHz5音は実用になりそうです



    
    

