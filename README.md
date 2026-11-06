# Spresense WiFi Halow ラッパーライブラリ

Spresense ボード上の WiFi Halow モジュール用 Arduino 互換 WiFi ライブラリです。

Spresense SDK のネットワークドライバを Arduino 風のシンプルなインターフェースでラップしています。SpresenseIMU ライブラリと同じスタイルで設計されています。

## 機能

- Arduino 標準スタイルの WiFi API
- WiFi ネットワークスキャン
- WiFi 接続・切断
- IP アドレス設定
- TCP/UDP ソケット通信
- HTTP クライアント機能
- DNS 名前解決

## インストール方法

1. `SpresenseWiFiHalow` フォルダ全体を Arduino ライブラリディレクトリにコピー：
   - Windows: `%APPDATA%\Arduino\libraries\`
   - Mac: `~/Documents/Arduino/libraries/`
   - Linux: `~/Arduino/libraries/`

2. Arduino IDE を再起動

## クイックスタート

### 基本的な WiFi 接続

```cpp
#include "SpresenseWiFiHalow.h"

void setup() {
  Serial.begin(115200);
  
  // WiFi 初期化
  if (SpresenseWiFiHalow.begin() < 0) {
    Serial.println("WiFi初期化失敗");
    return;
  }
  
  // WiFi に接続
  if (SpresenseWiFiHalow.connect("SSID", "パスワード") < 0) {
    Serial.println("接続失敗");
    return;
  }
  
  // 接続情報を表示
  Serial.print("接続済み: ");
  Serial.println(SpresenseWiFiHalow.SSID());
  Serial.print("IP: ");
  Serial.println(SpresenseWiFiHalow.localIP().toString());
}

void loop() {
  delay(1000);
}
```

### WiFi ネットワークスキャン

```cpp
void scanWiFi() {
  int networks = SpresenseWiFiHalow.scanNetworks();
  
  Serial.print("見つかったネットワーク数: ");
  Serial.println(networks);
  
  for (int i = 0; i < networks; i++) {
    Serial.print("SSID: ");
    Serial.println(SpresenseWiFiHalow.SSID(i));
    Serial.print("RSSI: ");
    Serial.print(SpresenseWiFiHalow.RSSI(i));
    Serial.println(" dBm");
  }
}
```

### TCP クライアント通信

```cpp
void httpGet() {
  WiFiClient client;
  
  if (client.connect("example.com", 80)) {
    client.write((const uint8_t*)"GET / HTTP/1.1\r\n", 16);
    client.write((const uint8_t*)"Host: example.com\r\n\r\n", 20);
    
    while (client.connected()) {
      if (client.available()) {
        Serial.write(client.read());
      }
    }
    client.stop();
  }
}
```

## サンプルコード

`examples/` ディレクトリに以下のサンプルを用意しています：

- **BasicWiFi** - 基本的な WiFi 接続と IP アドレス表示
- **ScanNetworks** - 利用可能な WiFi ネットワークをスキャン
- **HTTPClient** - HTTP GET リクエストを送信し、レスポンスを表示

## API リファレンス

### 接続管理

| メソッド | 説明 |
|---------|------|
| `begin()` | WiFi モジュール初期化 |
| `connect(ssid, password)` | WiFi ネットワークに接続 |
| `disconnect()` | WiFi から切断 |
| `status()` | WiFi 接続状態を取得 |
| `end()` | WiFi モジュール終了 |

戻り値：
- 0以上 = 成功
- 負の値 = エラー

### ネットワーク情報

| メソッド | 説明 |
|---------|------|
| `SSID()` | 現在接続中の SSID を取得 |
| `SSID(index)` | スキャン結果の SSID を取得 |
| `localIP()` | ローカル IP アドレス |
| `gatewayIP()` | ゲートウェイ IP |
| `subnetMask()` | サブネットマスク |
| `dnsIP()` | DNS サーバー IP |
| `macAddress(mac)` | MAC アドレス（バッファに格納） |
| `RSSI()` | 現在の信号強度（dBm） |
| `RSSI(index)` | スキャン結果の信号強度 |

### WiFi スキャン

| メソッド | 説明 |
|---------|------|
| `scanNetworks()` | 利用可能なネットワークをスキャン（結果数を返す） |
| `encryptionType(index)` | スキャン結果の暗号化方式を取得 |
| `scanDelete()` | スキャン結果をクリア |

### ソケット通信

#### WiFiClient（TCP クライアント）

```cpp
WiFiClient client;
client.connect(host, port)      // 接続
client.write(data, size)        // 送信
client.read()                   // 受信
client.available()              // 受信可能なバイト数
client.stop()                   // 切断
client.connected()              // 接続状態確認
```

#### WiFiServer（TCP サーバー）

```cpp
WiFiServer server(80);
server.begin()                  // リッスン開始
WiFiClient client = server.available()  // 新しい接続を受け入れ
server.stop()                   // リッスン停止
```

#### WiFiUDP（UDP 通信）

```cpp
WiFiUDP udp;
udp.begin(port)                 // ポートをバインド
udp.beginMulticast(ip, port)    // マルチキャスト受信開始
udp.write(data, size)           // 送信
udp.parsePacket()               // パケット受信確認
udp.read(buffer, size)          // 受信データを読み込み
udp.remoteIP()                  // 送信元IP を取得
udp.remotePort()                // 送信元ポートを取得
udp.endPacket()                 // パケット送信完了
udp.stop()                      // ソケットクローズ
```

## 状態定数

```cpp
WL_IDLE_STATUS          // アイドル状態
WL_NO_SSID_AVAIL        // SSID が見つからない
WL_SCAN_COMPLETED       // スキャン完了
WL_CONNECTED            // 接続済み
WL_CONNECT_FAILED       // 接続失敗
WL_CONNECTION_LOST      // 接続が切れた
WL_DISCONNECTED         // 切断状態
WL_AP_LISTENING         // APモード待機中
WL_AP_CONNECTED         // APモード接続中
WL_AP_FAILED            // APモード失敗
```

## 暗号化方式定数

```cpp
ENC_TYPE_OPEN          // オープン（暗号化なし）
ENC_TYPE_WEP           // WEP
ENC_TYPE_TKIP          // WPA (TKIP)
ENC_TYPE_CCMP          // WPA2 (CCMP)
ENC_TYPE_AUTO          // 自動選択
```

## トラブルシューティング

### WiFi 初期化が失敗する

**症状：** `SpresenseWiFiHalow.begin()` が負の値を返す

**原因：** 
- WiFi Halow ハードウェアが正しく接続されていない
- Spresense SDK のドライバが正しく実装されていない

**対策：**
- ハードウェア接続を確認
- シリアルモニタで詳細なエラーメッセージを確認

### WiFi スキャンが完了しない

**症状：** `scanNetworks()` がハング

**原因：** 
- スキャンのタイムアウト実装が不完全
- 無線環境の問題

**対策：**
- スキャン処理に時間制限を設ける
- 数秒待機してから再試行

### IP アドレスが 0.0.0.0 のままになる

**症状：** 接続後も `localIP()` が 0.0.0.0 を返す

**原因：** 
- DHCP による IP 取得がまだ完了していない
- SDK の IP 設定取得関数が実装されていない

**対策：**
- 接続後、数秒待機してから IP を取得
- 静的 IP 設定の実装を検討

### コンパイルエラー：「undefined reference to SpresenseWiFiHalow」

**原因：** CPP ファイルが正しく検出されていない

**対策：**
- Arduino IDE を再起動
- `boards.txt` でライブラリを明示的に指定

### コンパイルエラー：「wifi_halow.h: No such file or directory」

**原因：** SDK ヘッダが正しくインクルードされていない

**対策：**
- SDK インストール位置を確認
- パスを調整してください

## ライブラリ構成

```
SpresenseWiFiHalow/
├── library.properties              # Arduino ライブラリ情報
├── LICENSE                         # LGPL v2.1 ライセンス
├── README.md                       # このファイル
├── keywords.txt                    # Arduino IDE キーワード定義
│
├── src/
│   ├── SpresenseWiFiHalow.h        # ヘッダファイル（API定義）
│   └── SpresenseWiFiHalow.cpp      # 実装ファイル
│
└── examples/
    ├── BasicWiFi/
    │   └── BasicWiFi.ino          # 基本接続サンプル
    ├── ScanNetworks/
    │   └── ScanNetworks.ino        # スキャンサンプル
    └── HTTPClient/
        └── HTTPClient.ino          # HTTP通信サンプル
```

## 実装情報（開発者向け）

### API 設計

このライブラリは Arduino 標準の WiFi ライブラリ（WiFi101, WiFiNINA など）と互換性のあるAPI設計を採用しています。既存の Arduino WiFi コードの多くはほぼそのまま動作します。

### シングルトンパターン

SpresenseIMU と同じシングルトンパターンを採用：

```cpp
#include "SpresenseWiFiHalow.h"

void setup() {
  SpresenseWiFiHalow.begin();
  SpresenseWiFiHalow.connect("SSID", "Password");
}
```

### SDK 統合のステップ

#### 1. Spresense SDK ドライバ確認

Spresense SDK のWiFi Halowドライバの以下を確認：

```bash
# SDK内のWiFiヘッダを探す
find ~/spresensekit-tools -name "*wifi*" -o -name "*halow*"
```

#### 2. ドライバ関数の特定

以下のような関数を探します：

- 初期化関数（`wifi_halow_initialize()` など）
- 接続関数（`wifi_halow_connect()` など）
- スキャン関数（`wifi_halow_scan()` など）
- ステータス取得（`wifi_halow_get_status()` など）
- IP設定取得（`wifi_halow_get_ipconfig()` など）

#### 3. マッピング実装

`src/SpresenseWiFiHalow.cpp` の TODO マーク部分にSDK呼び出しを実装：

```cpp
int SpresenseWiFiHalowClass::_initializeModem() {
  // TODO: WiFi初期化
  // wifi_halow_initialize() を呼び出す
  return 0;
}

int SpresenseWiFiHalowClass::_connectNetwork(const char* ssid, const char* password) {
  // TODO: WiFi接続
  int ret = wifi_halow_connect(ssid, password);
  if (ret < 0) {
    _status = WL_CONNECT_FAILED;
    return -1;
  }
  
  // 接続完了待機とIP設定取得
  // ...
  
  _status = WL_CONNECTED;
  return 0;
}
```

#### 4. インクルード設定

SDK ヘッダを `SpresenseWiFiHalow.cpp` の上部に追加：

```cpp
#include <wifi_halow.h>  // 実際の名前に置き換え
#include <sys/socket.h>
#include <netdb.h>
```

### ステータス管理

WiFi接続状態の遷移：

```
WL_IDLE_STATUS
  ↓
WL_DISCONNECTED / WL_NO_SSID_AVAIL
  ↓
WL_CONNECTED (成功) または 
WL_CONNECT_FAILED / WL_CONNECTION_LOST (失敗)
```

### IP アドレス管理

`IPAddress` 構造体の機能：

- コンストラクタでの初期化
- `toString()` で文字列表現
- `print()` / `println()` でシリアル出力

### パフォーマンス考慮

- スキャン結果は最大15ネットワーク格納（`WL_MAX_SCAN_RESULTS` で調整可能）
- SSID最大32文字（IEEE 802.11規格に準拠）
- バッファサイズはメモリ制約に応じて調整可能

### 今後の拡張

- AP モード（アクセスポイント化）のサポート
- WPS接続サポート
- 証明書ベースの接続（WPA2-Enterprise）
- 低電力モード

## ライセンス

GNU Lesser General Public License v2.1 以降

このライブラリは自由に変更・配布できますが、修正版も LGPL v2.1 の下で提供する必要があります。

## 関連リンク

- [Spresense 開発ガイド](https://developer.sony.com/develop/spresense/documentation)
- [SpresenseIMU Library](https://github.com/Interested-In-Spresense/SpresenseIMU)
- [Arduino WiFi 標準ライブラリ](https://www.arduino.cc/en/Reference/WiFi)
