# CAN_f303
Nucleo-F303K8でCAN通信をしようと思って色々やってみたところうまくいかなかったので、公開をしてみます。  
ご指摘ございましたらよろしくお願いします :person_bowing:

## 作業環境
- macOS 14.6.1 (M1チップ)
- ピン設定・コード生成: STM32CubeMX
- コード編集: CLion

## やりたいこと
- 拡張IDでClassic CANの送受信をする
    - VESCベースのブラシレスESCとCANで通信できるようにしたい

## 現状
### CANable・Nucleo-G431RB・Nucleo-F303K8を繋いだ実験
- 1Mbit/sで通信
- CANableの送受信、G431の送受信はできたが、F303は送信しかできない
- 受信コールバックが呼ばれていない
    - NVICでRX0の割り込みは有効にしている
    - CANバスに繋げても、"Loopback combined with Silent"(送ったメッセージが自身にそのまま帰ってくるモード)にしても呼ばれないことに変わりがない
    - フィルター・マスクを設定していない状態でも、マスクを0(全IDを通す)に設定しても変わらず

## 関係しそうなファイル
- [Core/Src/main.c](./Core/Src/main.c)
    - メインの処理を書いた
- [Core/Inc/Connectivity/can.h](Core/Inc/Connectivity/can.h)
    - CAN通信周りの関数をまとめた
- [CAN_f303.ioc](./CAN_f303.ioc)
    - CubeMXの設定ファイル

よろしくお願いいたします

