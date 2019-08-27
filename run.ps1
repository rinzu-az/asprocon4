Param( [string]$inputfile )
#if($inputfile -as "bool"){
#  write-host "引数にファイル名(拡張子なし)を入力してください。"
#  exit
#}
cd "e:\user\Documents\GitHub\asprocon4\"
g++ -g -O2 -std=c++14 -Wall SA.cpp -o SA
$cmd = "get-content .\tester\case\${inputfile}\${inputfile}.txt | .\SA.exe | out-file .\tester\case\${inputfile}\result.txt -encoding utf8"
iex $cmd