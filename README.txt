
-----------------------------
4th Asprova Procon Visualizer
-----------------------------

使い方
・index.htmlをブラウザで開き、input fileに入力データファイルを、output fileに出力データファイルを指定するとガントチャートを表示します。
・各工程の上にマウスを持って行くと最早開始日、納期、粗利、ペナルティ額などが表示されます。
・納期遅れが発生しているオーダに関しては、'!'がそのオーダの全工程の上に表示されます。
・画面右上にあるボタンでチャートの大きさを変更できます。
・チャートをドラッグすることで表示位置を変更できます。

使用上の注意
・このビジュアライザはすべてのブラウザで作動することを保証していません。これまでのテストで、問題が発見されていないブラウザは以下です。
　　Google Chrome、Mozilla FireFox、Microsoft Edge
・このビジュアライザで計算された得点が、AtCoder上で計算される得点と一致することは保証されていません。大きな実数を扱った場合、有効桁数の問題で差異が出ることがあります。
・自分のプログラムの出力データの評価はoutput_checker.cppを使用し、ビジュアライザはあくまで出力データの視覚化のための補助的なツールとして使って頂けると幸いです。　
・output_checker.cppとビジュアライザの評価結果が異なる場合はoutput_checker.cppの方を信用してください。
・特にプログラムの出力データが制約を満たしていない場合、ビジュアライザが正しく作動しないケースがあることが確認されています。

ライセンス
・© Asprova Corporation
・MIT License (URL: https://opensource.org/licenses/mit-license.php)
・longcontest visualizer framework を使って作成しています。(URL: https://github.com/kmyk/longcontest-visualizer-framework)
・デザインや初期のコードは rco-contest-2019 を参考にさせて頂きました。(URL: https://github.com/recruit-communications/rco-contest-2019)
