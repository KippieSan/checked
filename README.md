# Checked
## 概要
`Checked`型同士もしくは`Checked`型を含む演算(<img src=
"https://render.githubusercontent.com/render/math?math=%5Ctextstyle+%2B%2C+-%2C+%5Ctimes%2C+%5Cdiv%2C+%5Cmod%0A" 
alt="+, -, \times, \div, \mod
">)においてオーバーフローが発生しているかを検知し、している場合は「**過去にオーバーフローを起こした**」という情報を保持したうえでラップアラウンドを行うクラスです。コンパイラオプションは`-Wall -Wextra -std=c++20 -O3 -mtune=native -march=native`です。

## 使用法
メインとなるファイルは
- `flagged.hpp`
- `checked.hpp`
- `checked.cpp`

です。`fast_checked.hpp`は`IO`関数以外すべてを`constexpr`関数化し、高速化を図ったものです。`template_checked.hpp`はこれをテンプレート化したものになります。但しこちらはオーバーフローを起こした場合ラップアラウンドではなく、値がその上界、下界で留まるようにしています。いずれもヘッダファイルに全ての実装が書かれています。

テンプレートで使用可能な型は`signed`の整数型(2の補数)のみです。`unsigned`については上手く動きません。
### 宣言
初期化子がない場合は`0`で初期化されます。数値及び`Checked`はコンストラクタ、代入のどちらかで初期化することができます。`Checked`を使った初期化ではステータスが受け継がれます(`c, d, e`のステータスは`HadOverflowed::Yes`となります)。
```c++
Checked zero;
Checked a(10), b = 10;
Checked c(20, HadOverflowed::Yes);
Checked d(c), e = c;
```

### 関数
`get_flagged()`では`Flagged`構造体を返します。従って、以下のように構造化束縛で値を受け取ることが可能です。`get_value()`は数値、`get_status()`は`HadOverflowed`型のステータスを返します。
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

## テスト
以下の五つのテストが用意されています。
- normal calculation (オーバーフローが起こらない演算)
- assignment calculation (代入演算)
- logical operation (論理演算)
- overflow (オーバーフロー可能性のある演算)
- speed (速度比較)

`test.cpp`の各名前空間先頭の`cases`がテストケースの数を表しています。テストケースの数が少ない場合は`test.cpp: 15`の`large_test`を`false`に設定することで、演算内容を出力します。

### Checked型の演算速度について
結論としては`Checked`型は通常の`std::int64_t`と比べて1.3倍程度遅くなります。
`test.cpp : speed`では
- 乱数生成機のシードを決定
- 乱数による`lhs`, `rhs`, `operator`の生成
- 演算
- `std::vector`にプッシュ

を`operation`回行った時間を測定し、更にこれを`cases`回繰り返して`int`及び`Checked`の演算時間の平均及びその時間比を出します。更にこれを`ratio_cases`回繰り返して時間比の平均を取っています。`ratio`はその最大値と最小値を保持しておきます。また、乱数生成機は`int`, `Checked`の前で同じシードで新しく生成し、全く同じ演算が行われるようにしています。
`cases = 100`, `operation = 100000`, `ratio_cases = 100`でテストを行った結果は次のようになります。

```
Speed Test ( Cases: 100, Operation: 100000, Ratio cases: 100 )
Average calculation time (int):     14.3076 (ms)
Average calculation time (Checked): 18.2656 (ms)
Average Checked : int ratio: 1.27352 => ( lowest: 1.2529, highest: 1.27672 )
Speed test time: 337 (s)
```

1.27付近に分布していますがテストケースによっては1.25～1.3までの誤差があるのでおよそ1.3倍としています。

#### fast_checked.hppを使った場合の演算速度
通常の`Checked`型は`std::int64_t`に対して1.3倍程度の遅延がありましたが、`fast_checked.hpp`を用いて同様のテストを行うと結果は次のようになります。テストの方法は`Checked`と同じです。

```
Speed Test ( Cases: 100, Operation: 100000, Ratio cases: 100 )
Average calculation time (int):     14.9858 (ms)
Average calculation time (Checked): 17.899 (ms)
Average Checked : int ratio: 1.19247 => ( lowest: 1.16972, highest: 1.19593 )
Speed test time: 341 (s)
```

1.2倍ほどの遅延となります。

#### template_checked.hppを使った場合の演算速度
`template_checked.hpp`を用いてテストを行うと結果は次のようになります。

```
Speed Test ( Cases: 100, Operation: 100000, Ratio cases: 100 )
Average calculation time (int):     14.2722 (ms)
Average calculation time (Checked): 17.775 (ms)
Average Checked : int ratio: 1.24645 ( lowest: 1.24449, highest: 1.25088 )
Speed test time: 331 (s)
```

1.25倍ほどの遅延となりますがテストケースによっては1.21～1.26倍までの誤差があります。

つまり速度は`Checked`<`template_checked`<`fast_checked`となります。

### テスト結果例(Checked)

```
Normal Calculation Test
 ( Cases: 100000000 ) => All results are correct: Yes

Assignment Calculation Test
 ( Cases: 100000000 ) => All results are correct: Yes

Logical Operation Test
 ( Cases: 100000000 ) => All results are correct: Yes

Overflow Test
 ( Cases: 100000000 ) => All results are correct: Yes

Speed Test ( Cases: 100, Operation: 100000, Ratio cases: 100 )
Average calculation time (int):     14.3076 (ms)
Average calculation time (Checked): 18.2656 (ms)
Average Checked : int ratio: 1.27352 => ( lowest: 1.2529, highest: 1.27672 )
Speed test time: 337 (s)
```

```
Normal Calculation Test
          2063853213 /          -1047862361 =                   -1 : o
          2034056425 %            -59655603 =              5765923 : o
           424213620 -            576717490 =           -152503870 : o
         -1962032855 +          -2147264039 =          -4109296894 : o
          1624898541 /           -852655701 =                   -1 : o
All results are correct: Yes

Assignment Calculation Test
Assign  /          -1970652888 to            570334671 Assigned =                    0 : o
Assign  -          -1616106452 to                    0 Assigned =           1616106452 : o
Assign  /            921901909 to           1616106452 Assigned =                    1 : o
Assign  -            639173239 to                    1 Assigned =           -639173238 : o
Assign  +          -1378317054 to           -639173238 Assigned =          -2017490292 : o
All results are correct: Yes

Logical Operation Test
l:             -3151081 r:            758661305 result: true : o
l:            410000884 r:            410000884 result: true : o equivalent
l:            931826169 r:           -665156494 result: true : o
l:           1281486447 r:           1593080811 result: true : o
l:          -1109034373 r:           -817304176 result: true : o
All results are correct: Yes

Overflow Test
test #0
     -54128758059530 /     -342593387626893 =                    0 status: HadOverflowed::No  : o
     527985803627453 *        -386459377818 = -2710358382006218674 status: HadOverflowed::Yes : o
     -83362809305034 *   725984603119004007 = -8843218936705002566 status: HadOverflowed::Yes : o
      -6366445202799 *       70982650641295 =    68885964398479871 status: HadOverflowed::Yes : o
         76860556133 *       -4952677844857 =  1437201773393289795 status: HadOverflowed::Yes : o
All results are correct: Yes

Speed Test ( Cases: 100, Operation: 100000, Ratio cases: 100 )
Average calculation time (int):     14.9858 (ms)
Average calculation time (Checked): 17.899 (ms)
Average Checked : int ratio: 1.19247 => ( lowest: 1.16972, highest: 1.19593 )
Speed test time: 341 (s)
```