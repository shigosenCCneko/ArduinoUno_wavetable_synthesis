#   WaveMemory Editor



使い方はスライダを変更してパラメータをリアルタイムに変更します,波形の編集は
基本波形ボタンで波形を加算したり、マウスでフリーハンドで波形を描く事も可能。

### 設定

仮想MIDIケーブル(LoopMIDI等)を使いブリッジするデバイスを作り
waveSyn.propertiesファイルでブリッジする入力のMIDIデバイス名,COMポートの通信速度
ブリッジするMIDIポートを指定します。
プロパティファイルは実行パスに置いてください。


(MIDIブリッジin)  
	midiStreamDeviceName=Trans  
COMポート指定  
	midiComPort=COM9  
ボーレート指定  
	baudRate=115200
音色ディレクトリ指定  
workDir=.\\tonedata\\

## 機能


![waveMemoryEditor](https://user-images.githubusercontent.com/28349102/95017371-e6f97780-0693-11eb-947f-3540afe2030a.jpg)



#### エンベロープ
エンベロープの変更


#### 波形加算
ボタンでセレクトした波形を波形に加算、デフォルトで1/4
1/2チェックボタンで1/8値

#### 	デューテイー比変更
矩形波(Rect)と鋸波(Saw)のデューテイー比？を変更

#### テキスト書き出し
テキストエリアへ音色データを書き出し。
#### テキスト読み込み
テキストエリアから音色データを読み込
