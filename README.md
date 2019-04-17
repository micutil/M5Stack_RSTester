# M5Stack RSTester
RSTesterは、双葉電子工業さんのRS304系のサーボの動作テスト、ID変更および、各種設定の表示や変更を行う為の、M5Stackのプログラムおよび、RS304系のサーボを接続するためのアダプターです。

どのようなプログラムなのかは、以下の像をクリックすると操作中の動画をで観ることができます。<br>
[![preview](images/preview01s.jpg)](https://www.youtube.com/watch?v=g7JMcP97mWo)


## インストール

### ① M5StackのmicroSDに入れるもの
1. 同梱のファイルの中にmicroSDフォルダがあります、その中のファイルやフォルダをmicroSDにコピーをして下さい。ちなみに、**RSTester.bin**というファイルがRSTesterのプログラムになります。jpgやjsonのフォルダに入っているファイルはメニュプログラムが使うデータです。もし、microSDの中に、既にjpgフォルダや、jsonフォルダがある場合は、それぞれの中に入っているファイルをコピーするようにして下さい。またバージョンのファイルで置き換えてしまわない様にコピーする際は、修正日を比べてコピーする様にして下さい。
2. 次に、RSTesterが使っている**フォント**（**FONT.BIN**, **FONTLCD.BIN**)が必要になります。これらのファイツをまだ入れてない場合は、ブラウザで、[Tamakichi/Arduino-KanjiFont-Library-SD](https://git.io/fjYst)を開いて下さい。
3. 「**Clone or download**」でD**ownload ZIP**を選び、ダウンロードし、zipを解凍します。
4. そのファイルの中に、fontbinフォルダがあり、中に、**FONT.BIN**, **FONTLCD.BIN**というファイルがあるので、これら２つのファイルをmicroSDにコピーします。
5. これでM5Stackに入れるmicroSDの作成は終了です。このmicroSDをM5Stackに入れて下さい。


### ② USBドライバーをインストールする
M5Stackをコンピュータと繋いで、プログラムの転送など通信するためには、Silicon LabsのUSBDriverをインストールする必要があります。もしまだインストールしてなければ
[シリアル接続の確立方法](https://docs.m5stack.com/#/ja/related_documents/establish_serial_connection)
のサイトを参考にしてインストールして下さい。

> なお、Macの場合、インストールしただけではセキュリティが通ってないので、インストール後、環境設定のセキュリティとプライバシーの一般で、インストールしたドライバーの許可をして下さい。
> 
![セキュリティ](images/kyoka.jpg)


### ③ M5Burner_Micで、M5StackにSD-Menuをインストールする
"M5Burner\_Mic"というプログラムでSD-MenuをM5Stackにインストールします。すでにSD-MenuまたはLovyanLauncherをインストールしてある場合はこのステップは必要ありません。

1. [M5Burner_Mic](https://github.com/micutil/M5Burner_Mic) のページから「Download [here](http://micutil.com/download/M5Burner_Mic.zip)」の所でM5Burner\_Micをダウンロードし、解凍して下さい。M5Burner\_Micフォルダはお好みの場所に入れて下さい。
2. M5Stackに付いてきたUSB-CケーブルでパソコンとM5Stackを繋げて下さい。
3. M5Burner\_Micをダブルクリックして起動します。
4. USBシリアルポートをM5Stackのポートに設定します。
 - Macの場合はポートに名前がつくので「**SLAB_USBtoUART**」という名前のポートを選んで下さい。
 - Windowsの場合は、**COM3**とか、COM4とかの名前になっています。ひとつしか表示されてなかったら、それがM5Stackのポートでしょう。もしいくつか表示されているようだったら、コントロールパネルから、デバイスマネージャーのポートをみて番号を確認して下さい。例えば以下の図の場合だと**COM4**であるということになります。<br>![ポート番号](images/portnum.jpg)
5. 「Priset」のポップアップメニューで「**SD-Menu**」の最新版を選択します。
6. 「**Start**」ボタンをクリックすると、プログラムの転送が開始します。
7. プログラムの転送が終わるとM5Stackがリセットされ、インストールした**SD-Menu**が起動します。
8. M5StackのCボタン（右）を何回か押して、RSTesterを選択し、Bボタン（中央）のボタンを押すと、RSTesterが起動します。操作方法は後で説明します。
9. 再度、メニューを表示する場合は、Aボタン（左）を押しながらリセットボタン（左上側面）を押すとSD-Menuが起動します。

## 接続
M5Stackとサーボをジャンパーワイヤなどでつなぎます。

	M5Stack		RS304系サーボ（写真の場合上から）
	T2		…	シグナル
	5V		…	パワー
	GND		…	グランド

1. 写真のようなコネクターを作る場合は、ROBOMICのブログの[「週刊 ロビ」 RS308MD 基板側端子の入手とコネクター作成例](http://micono.cocolog-nifty.com/blog/2013/04/rs308md-f205.html)
を参考にして下さい。**ただし、この場合はサーボの情報の取得はできません。**<br>
 - ロビ１の基板側サーボコネクタの型番：BM03B-ACHSS-GAN-ETF(LF)(SN) [購入先例](https://www.marutsu.co.jp/pc/i/31767074/)
 - ロビ２の基板側サーボコネクタの型番：BM03B-ADHKS-GAN-ETB(LF)(SN) [購入先例](https://www.marutsu.co.jp/GoodsDetail.jsp?q=BM03B-ADHKS-GAN-ETB%28LF%29%28SN%29&salesGoodsCode=15432731&shopNo=3)
 - ＳＯＴ２３変換基板 [購入先例](http://akizukidenshi.com/catalog/g/gP-03660/)
 - 分割ロングピンソケット [購入先例](http://akizukidenshi.com/catalog/g/gC-05779/)

		<IMG SRC=images/connector.png width="160">

2. **M5Stack RSTester専用アンダプターについて**
	- サーボ情報を取得するためには半２重回路にしなくてはいけません。M5Stackに簡単に付けて使えるアダプタを設計しました。アダプターは２種類あり、黒いアダプタと赤いアダプタがあります（図）。
	- 黒いアダプターは、M5Stackの右側面のGPIOに挿して使います（図）。
	- 赤いアダプターは、FireなどのGROVE UARTのコネクタをつなげて使います（図）。

それぞれのアダプターが対応している、M5STack, サーボは、以下の通りです。

	アダプター	　M5Stack				　サーボ
		黒	…	BASIC,GRAY				RS303SR,RS304MD, ロボゼロ, ロビ１、ロビ２
		赤	…	BASIC,GRAY,FIRE,M5GO	ロビ１、ロビ２

<IMG SRC=images/m5sadptr.jpg width="300">

回路図：

<IMG SRC=images/type1.png width="300">
<IMG SRC=images/type2.png width="300">


パーツ：

 - ロビ１の基板側サーボコネクタの型番：BM03B-ACHSS-GAN-ETF(LF)(SN) [購入先例](http://d.digikey.com/qKKS070KNe1p3YL0X0d800K)
 - ロビ２の基板側サーボコネクタの型番：BM03B-ADHKS-GAN-ETB(LF)(SN) [購入先例](http://d.digikey.com/qKKS070KNf1p3YL0X0d900K)
 - 3ステートバッファーIC SN74LV4T125PWR / 74??125?などなど [購入先例](http://d.digikey.com/o00ap00gY10KdSL3KKX7KN0)
 - 抵抗 100Ω (1608)
 - 抵抗 1kΩ (1608)
 - コンデンサー 0.1µF (1608) [購入先例](http://akizukidenshi.com/catalog/g/gP-05621/)
 - GROVE用-4Pコネクタ Ｌ型（オス）[購入先例](http://akizukidenshi.com/catalog/g/gC-12634/)
 
**M5Stack RSTester専用アンダプターに関してのご質問、ご希望等は、miconoまで、問い合わせください。**



## RSTesterの簡単な操作方法


1. サーボをつなげる。注意：複数のサーボがつながっている場合、パソコンからのUSBでは電流不足になる場合があるので、モバイルバッテリーをM5Stackにつなげてください。
2. M5Stackの電源を入れる。
3. TobozoLauncher, LovyanLauncherなどで、RSTesterを起動します。
4. A: 動作テスト、B: ID変更、C: 各種設定（の表示と変更）の機能があります。

### A: 動作テスト
接続されているサーボを回転させて、正しく機能するかテストします。
1. サーボのIDを上下矢印ボタンで選択し、OKボタンを押します。なお**255**は、繋がっているすべてのIDのサーボが対象になります。
2. 動作角度を上下矢印ボタンで選択し、OKボタンをおします。
3. サーボが指定した角度て動作をします。

### B: ID変更
接続されているサーボのIDを変更します。
1. IDを変更する対象のサーボを上下矢印ボタンで選択し、OKボタンを押します。なお**255**は、繋がっているすべてのIDのサーボが対象になります。255を選ぶと繋がっているIDは次の「2」操作で設定したID番号に変わってしまいますのでご注意ください。
2. 変更するID番号を上下矢印ボタンで選択し、OKボタンをおします。
3. サーボのIDを変更して、ROMに書き込んで、サーボをリセットします。

### C: 各種設定
選択したサーボの各種設定値を表示し、変更可能な項目の値を変更することができます。<br>
**以下の像をクリックすると操作中の動画をで観ることができます。**<br>
[![preview](images/preview01s.jpg)](https://www.youtube.com/watch?v=g7JMcP97mWo)

- 各種設定項目の詳細に関しては、[ロビ１、ロボゼロなどは、RS303MR／RS304MD取扱説明書](http://www.futaba.co.jp/img/uploads/files/robot/command_type_servos/RS303MR_RS304MD_118.pdf)、[ロビ２は、RS204MD取扱説明書](http://www.futaba.co.jp/img/uploads/files/robot/download/RS204MD_104.pdf)を読んで下さい。
- 下および右矢印で、設定項目を選択できます。選択された項目は**黄色**の文字になります。また下の方に、簡単な説明が表示されます。
- 「選択」ボタンを押すと、その設定に関してのデフォルト値など詳細が表示されます。
	- 白色で表示されていた項目は値の変更はできません。
	- 緑色で表示されていた項目の値は変更できます。
		1. 上下矢印ボタンで設定値を変えて「OK」ボタンを押して下さい。
		2. 「変更しないで一覧に戻る」場合はAボタンを、「変更だけして一覧に戻る」場合はBボタンを、「変更してさらにROMに書き込んでトップメニューに戻る」場合はCボタンを押して下さい。

- なお、変更した値は、ROMへの書き込み操作を行わないと変更しないので、書き込みを行わないで一覧に戻った場合、その変更した設定値を反映させたい場合は、一覧の1番右下の「保存して戻る」を選択してから戻って下さい。
- ROMに書き込まない場合は、サーボの電源をいれなおせば元の値に戻ります。


## ライセンス
CC 4.0 BY-NC-ND https://github.com/micutil/M5Stack_RSTester

- 同梱されている書類すべてに関して、無断転載、掲載、改変を禁じます。
- これらを使ったことにより生じるいかなるトラブルに関しても自己責任においておこなって下さい。
- 作ったばかりなので、まだソースコードがぐちゃぐちゃなので、しばらく、改変禁止とさせて頂きます。リクエスト、アドバイスなどあれば、教えてください。

