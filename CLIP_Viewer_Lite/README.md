# CLIP_Viewer_Lite API

## Description
Witten by Hirano Yuto<br>
Date: 2023/10/25<br>

## Usage
### `clip_api_conf.json`
`"debug": true` - Enables debug mode.  <br>
`"device_id"` - Set your device ID. (Unique ID)<br>
`"X-API-Key"` - Set your API key.<br>
`"username"` - Set your user name. (Mail address)<br>
`"password"` - Set your user password.<br>
`"data_selection"` <br>
- `"all"` - All data. 
- `"temperature": true` - Enable get temperature data
- `"humidity": true` - Enable get humidity data
- ... (Other data)

### `clip_api.py`
GET_token() - Get token. (expiration: 5 minutes)<br>
GET_API_data() - Get data. (get data from API selected by config file)<br>
POST_API_data() - Post data. (**NOT IMPLEMENTED YET**)<br>

## Response Body
| 項目名 | データ型 | 説明 |
|-|-|-|  
| payload | Array | ペイロードデータ配列<br>※複数データが存在する場合は、「sendDateTime」の降順で全データを取得 |
| deviceId | String | デバイスID |
| sendDateTime | String | 送信時間<br>※フォーマットはYYYY-MM-DD hh:mm:ss形式とする |
| payloadType | String | ペイロードタイプ<br>※ペイロード仕様にないペイロードデータの場合は、nullを返却 |
| payload | String | ペイロード |
| temperature | Float | 温度<br>※データが存在しない場合は、nullを返却 |
| humidity | Float | 湿度<br>※データが存在しない場合は、nullを返却 |  
| carbonDioxide | Float | CO2濃度<br>※データが存在しない場合は、nullを返却 |
| xAcceleration | Float | 加速度X軸<br>※データが存在しない場合は、nullを返却 |
| yAcceleration | Float | 加速度Y軸<br>※データが存在しない場合は、nullを返却 |
| zAcceleration | Float | 加速度Z軸<br>※データが存在しない場合は、nullを返却 |
| pitchGyro | Float | ジャイロピッチ軸<br>※データが存在しない場合は、nullを返却 |
| rollGyro | Float | ジャイロロール軸<br>※データが存在しない場合は、nullを返却 |
| yawGyro | Float | ジャイロヨー軸<br>※データが存在しない場合は、nullを返却 |
| airPressure | Float | 気圧<br>※データが存在しない場合は、nullを返却 |
| pressure | Float | 圧力<br>※データが存在しない場合は、nullを返却 |
| illuminance | Float | 照度<br>※データが存在しない場合は、nullを返却 |
| distance | Int | 距離<br>※データが存在しない場合は、nullを返却 |
| voltageInfo | Int | 電圧情報 <br>※データが存在しない場合は、nullを返却 |
| error | Int | エラー番号<br>※データが存在しない場合は、nullを返却 |
| chNumber | Int | CH番号<br>※データが存在しない場合は、nullを返却 |
| trigger1 | Float | トリガー1<br>※データが存在しない場合は、nullを返却 |
| trigger2 | Float | トリガー2<br>※データが存在しない場合は、nullを返却 |
