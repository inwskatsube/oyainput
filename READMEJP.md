# このソフトについて

Linuxで親指シフト入力を可能にするためのソフトウェアです。  
MozcやAnthy、libkccなど、主要な日本語入力システムに対応します。  
インプットメソッドフレームワークはfcitxを想定していますが、ibusやuimでも利用可能です。  
新しめのLinuxならだいたい動くはずですが、開発時はUbuntu16.04LTSで動作確認しています。
  
# インストール方法
  
ターミナルから以下のコマンドを入力します。

    $ make clean; make; sudo make install

インストールには管理者(root)権限が必要です。  
またGCC、makeが必要ですので、もしエラーになる場合は apt-get などで事前に導入しておいてください。

    $ sudo apt-get install -y make gcc


# 起動方法

    $ oyainput [username]

usernameは利用するユーザー名を指定します。  
省略した場合は、現在のログインユーザー名になります。  
もしsuコマンドなどで管理者(root)モードでターミナルにログインしている場合はrootユーザのホームディレクトリを見に行くことになりますので、[username]は省略しないことをおすすめします。


# 設定ファイルについて
設定ファイルは、初回起動時にホームディレクトリ下に以下の名前で作成されます。

    (HOMEDIR)/.oyainputconf

## 項目の説明 

1行ごとに、 `設定項目=設定値 ` という書式で記述します。  
各設定項目の説明を以下に記述します。  
 
    LOYAKEY=キー名 

左親指シフトキーを設定します。
 
    ROYAKEY=キー名 

右親指シフトキーを設定します。
 
    ONKEY=キー名 

oyainputを一時停止状態から再開します。
 
    OFFKEY=キー名 

oyainputを一時停止状態にします。
 
    CHARTIME=数字

文字->親指同時打鍵検出許容期間(ミリ秒)を設定します。(デフォルト200)

    OYATIME=数字

親指->文字同時打鍵検出許容期間(ミリ秒)を設定します(デフォルト200)

    NICOLATIME=数字 

親指キー単独打鍵の判定時間(ミリ秒)を設定します。(デフォルト50)
 
    DEVFILENO=数字 

キーボードのデバイスイベントファイルの番号の数字を指定します。  
oyainputはキーボードを自動認識しますが、複数キーボードが接続されているとうまく動作しない可能性があります。  
その場合、接続されているキーボードのデバイスイベント番号を調べる必要があります。  
ターミナルなどで `$ cat /proc/bus/input/devices` と入力すればデバイス情報一覧を表示できます。  

    IM=連携対象のインプットメソッドフレームワーク名 

日本語モードのONとOFFに合わせて、親指シフトモードのONとOFFを自動で切り替えます。  
現在設定可能なのはfcitxおよびibusになります。 (「IM=fcitx」「IM=ibus」)  
  
fcitxの場合は設定が下記のようになっていることを確認してください。  
「入力メソッドの設定」で  

* １番最初に直接入力（キーボードまたはキーボードレイアウト名） 
* ２番目に変換エンジン（Mozc、Anthyなど） 

となるように設定します。 
これで入力メソッドのオンとオフに連動して親指シフトもオン状態とオフ状態が自動で切り替わります。  

Mozcで入力していると、上段の記号類が正しく入力出来ない場合があります。  
これはMozcが利用するキーボードが「既定の入力メソッド」になっている場合に起こるようです。  
入力メソッドの設定画面で「Mozc」をダブルクリックすると「キーボートレイアウト」が選択出来ますので、これを「英語（US）」にしてみてください。

    KEYADD=(キー名):(出力ローマ字) 

通常打鍵時のキーを追加（または置換）します。
 
    LKEYADD=(キー名):(出力ローマ字) 

左親指キー同時打鍵時のキーを追加（置換）します。

    RKEYADD=(キー名):(出力ローマ字) 

右親指キー同時打鍵時のキーを追加（置換）します
 

### 設定で指定可能なキー
キー名は以下が指定可能です。  

    SPACE,A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,
    1,2,3,4,5,6,7,8,9,0,F1,F2,F3,F4,F5,F6,F7,F8,F9,F10,F11,F12,F13,
    MINUS,SEMICOLON,APOSTROPHE,GRAVE,BACKSLASH,COMMA,DOT,SLASH,
    ZENKAKUHANKAKU,KATAKANA,HIRAGANA,HENKAN,KATAKANAHIRAGANA,MUHENKAN,
    ESC,EQUAL,BACKSPACE,TAB,LEFTBRACE,RIGHTBRACE,ENTER,
    LEFTCTRL,RIGHTCTRL,LEFTSHIFT,RIGHTSHIFT,LEFTALT,RIGHTALT,
    HOME,SCREENLOCK,CAPSLOCK,NUMLOCK,SCROLLLOCK,
    SCROLLUP,SCROLLDOWN,UP,DOWN,LEFT,RIGHT,PAGEUP,PAGEDOWN,
    KP0,KP1,KP2,KP3,KP4,KP5,KP6,KP7,KP8,KP9,KPMINUS,KPPLUS,KPDOT,
    KPJPCOMMA,KPENTER,KPSLASH,KPLEFTPAREN,KPRIGHTPAREN,KPASTERISK,
    102ND,RO,SYSRQ,END,INSERT,DELETE,YEN,LEFTMETA,RIGHTMETA,
 
出力ローマ字は以下が指定可能です。  

    A,I,U,E,O,KA,KI,KU,KE,KO,SA,SI,SU,SE,SO,TA,TI,TU,TE,TO,
    NA,NI,NU,NE,NO,HA,HI,HU,HE,HO,MA,MI,MU,ME,MO,YA,YI,YU,YE,YO,
    RA,RI,RU,RE,RO,GA,GI,GU,GE,GO,ZA,ZI,ZU,ZE,ZO,DA,DI,DU,DE,DO,
    BA,BI,BU,BE,BO,PA,PI,PU,PE,PO,WA,WI,WU,WE,WO,XA,XI,XU,XE,XO,
    XYA,XYI,XYU,XYE,XYO,XTU,NN,VU,DEL,BS,QUESTION,
    SLASH,TILDE,LKAGI,RKAGI,LBRACKET,RBRACKET,LPAREN,RPAREN,
    0,1,2,3,4,5,6,7,8,9,MINUS,PERIOD,COMMA,KUTEN,KUTOUTEN,
    NAKAGURO,DAKUTEN,HANDAKUTEN


# 謝辞
当ソフトの同時打鍵アルゴリズムは日笠健氏開発の『親指ひゅんＱ』の
付属資料である「同時打鍵ステートマシン」の項にあるアルゴリズムに
基づき実装しました。貴重な資料を公開いただいた日笠氏に深く感謝
いたします。

