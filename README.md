# Checked
## 概要
`Checked`型同士もしくは`Checked`型を含む演算(<img src=
"https://render.githubusercontent.com/render/math?math=%5Ctextstyle+%2B%2C+-%2C+%5Ctimes%2C+%5Cdiv%2C+%5Cmod%0A" 
alt="+, -, \times, \div, \mod
">)においてオーバーフローが発生しているかを検知し、している場合は「**過去にオーバーフローを起こした**」という情報を保持したうえでラウンドアップ演算を行うクラスです。

## 使用法
### 宣言
初期化子がない場合は`0`で初期化されます。数値及び`Checked`はコンストラクタ、代入のどちらかで初期化することができます。`Checked`を使った初期化ではステータスが受け継がれます(`c, d, e`のステータスは`HadOverflowed::Yes`となります)。
```c++
Checked zero;
Checked a(10), b = 10;
Checked c(20, HadOverflowed::Yes);
Checked d(c), e = c;
```

### 関数
`get_flagged()`では`Flagged`構造体を返します。従って、以下のように構造体束縛で値を受け取ることが可能です。`get_value()`は数値、`get_status()`は`HadOverflowed`型のステータスを返します。
```c++
auto [value_a, status_a] = a.get_flagged();
auto value_b = b.get_value();
auto value_c = c.get_status();
```

### オーバーロードされた演算子
|種類|対応する演算子|
|:--|:--|
|単項演算子|`+`, `-`, 前置`++`, 後置`++`, 前置`--`, 後置`--`|
|代入演算子|`=`, `+=`, `-=`, `*=`, `/=`, `%=`|
|二項演算子|`+`, `-`, `*`, `/`, `%`|
|比較演算子|`==`, `!=`, `<`, `<=`, `>`, `>=`|
|出入力|`<<`, `>>`|

単項演算子、代入演算子、二項演算子は使用する際にオーバーフローのチェックが行われます。比較演算子、出入力はオーバーフラグに依らず純粋な値比較が行われます。
