# WEBOTS2023

## 概要

このプロジェクトは、大学の授業「ロボットシミュレーション演習」内で制作した自由課題作品です。オープンソースのロボットシミュレータ **Webots** を用いて、複数種類のコントローラとロボット動作を実装しています。

## ディレクトリ構成

```
WEBOTS2023/
├─ controllers/ # 各種ロボットのコントローラ（C/C++）
│ ├─ crane/
│ ├─ four_wheels_vehicle/
│ ├─ grabber_base/
│ ├─ my_controllerX_X/ # 自作制御ロジック群
│ ├─ tinkerbots_demo/
│ └─ ...他
├─ libraries/ # 再利用可能な制御モジュール
├─ plugins/ # Webots プラグイン（物理エンジン、リモート操作等）
├─ protos/ # カスタムロボットや部品の定義ファイル
│ ├─ icons/
│ └─ textures/
└─ worlds/ # シーン定義ファイルとテクスチャ
```

## 動作環境

- Webots 2023 以降
- C/C++ ビルド環境（例：MSVC、GCC）

## 使用方法

1. Webots を起動します。
2. `WEBOTS2023/worlds/` にある `.wbt` ファイルを開きます。
3. シミュレーションを開始すると、コントローラが動作します。

## 主なコントローラ一覧

| ディレクトリ名                    | 内容（概要）         |
| -------------------------- | -------------- |
| my\_controller1\_1         | 自律移動制御の基本      |
| my\_controller2\_1〜2\_ex   | ランダムウォーク、障害物回避 |
| my\_controller3\_1〜3\_ex   | 複数ロボットによる協調制御  |
| my\_controller5\_3         | ビジュアルフィードバック制御 |
| my\_controller\_display    | 表示機能付きコントローラ   |
| my\_controller\_randomwalk | 自律ランダムウォーク     |
| crane / grabber\_base      | アーム・クレーン操作の制御  |
| four\_wheels\_vehicle      | 車両型ロボットの基本運動   |


## ライセンス
このプロジェクトは教育目的で制作されており、自由に閲覧・参考利用できます。ただし、商用利用はご遠慮ください。