-------------------------------------------------------------
HandlerSocketを使用したFile Down Loader (Apache Module)
-------------------------------------------------------------

■何が出来るか
・HandlerSocketプロトコル経由でMariaDBからデータを取得する
・URL内にDBの主キーを指定するとMIME Typeと内容を取得して表示する


■確認環境
・CentOS Stream 8 x64
・Apache 2.4.37 (rpm)
・MariaDB(10.3.27) (Handler Socketプラグイン入り)


■設定方法
※コンパイルする際は「http-devel」「apxs」「apr-devel」「rpm-build」、「gcc-c++」、「make」が必要

$ make clean
$ make
$ su
# make install


※make installを行うと、
httpd.confに以下の文字列が追加される
LoadModule hsdl_module   (Module Path)/mod_hsdl.so

■Apache Conf
http://localhost/hs/(key) でアクセスさせたい場合

<Location "/hs/">
    SetHandler hsdl
      # MariaDB Host (default:localhost)
    hsdl_dbhost  localhost
      # Handler Socket Port (default:9998)
    hsdl_hsport  9998
      # MariaDB Database
    hsdl_dbname  hstest
      # MariaDB Table
    hsdl_dbtable hstest1
      # Write Buffer (default:2097152)
    hsdl_bufsize 2097152
</Location>

※設定後Apache再起動

■MariaDB
1. MariaDBのmariadb-server.cnfに下記の設定を追記する
[mysqld]
plugin_maturity=beta

2. 上記を設定したらmysqlコマンドでMariaDBにログインし下記を実行する
install plugin handlersocket soname 'handlersocket.so';

3. MariaDBのmariadb-server.cnfの[mysqld]に下記の設定を追記する

handlersocket_port=9998
handlersocket_port_wr=9999
handlersocket_address=
handlersocket_verbose=0
handlersocket_timeout=300
handlersocket_threads=16
thread_concurrency=128
open_files_limit=65535


4. テーブル設定

CREATE TABLE `hstest1` (
  `k` varchar(20) NOT NULL DEFAULT '',
  `m` varchar(50) DEFAULT NULL,
  `v` mediumblob,
  PRIMARY KEY (`k`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

5. データ登録
Insert時
INSERT INTO hstest1 VALUES('test','image/jpeg','(binary data)')

k = Key
m = MimeType
v = Value

上記のinsert文を実行すると
http://localhost/hs/test
で「k」がtestの情報をApacheが返却する

■メリット
・ApacheにURLを記載すればデータを抽出できる
・ファイルサーバとHTTPサーバを分離できる

■デメリット
・実ファイルの直接DLより若干遅い
・ネットワーク状況に依存するため、回線が遅い場合はそれなりの速度しか出ない
・大きいファイルは苦手
・キャッシュは対応していない

■利用にあたっての免責事項
本ソフトウェアの開発者は、本ソフトウェアの不稼動、稼動不良を含む
法律上の瑕疵担保責任、その他保証責任を負わないものとします。
また、本ソフトウエアの開発者は、本ソフトウェアの商品性、
特定の目的に対する適合性について、いかなる保証も負わないものとします。
