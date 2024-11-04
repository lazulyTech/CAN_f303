# 追記
(2024/11/4)  
「フィルター設定は必須」「拡張フォーマットの場合は16bitモードが使えない」という指摘を受けて修正したところ、受信するようになりました！！  
皆様ありがとうございました！

参考にしたところ 豊田高専学ロボチームT.G.Tのファームウェアとライブラリ
- [フィルタ設定使用部分](https://github.com/gakurobo-taro/C610_C620_controller/blob/7ee4d136e167d002680acfe43d7b0cdbe5db42da/UserLib/board_task.cpp#L65)
- [ライブラリ](https://github.com/gakurobo-taro/STM32HAL_CommonLib/blob/7b034a12669c3be5f9b2885db8ab15a20c81b410/can_comm.hpp#L190)

# CAN_f303
Nucleo-F303K8でCAN通信をしようと思って色々やってみたところ~~うまくいかなかった~~(後にできるようになった)ので、公開をしてみます。  
ご指摘ございましたらよろしくお願いします 🙇

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

