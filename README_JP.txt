-------------------------------------------------------------
HandlerSocket���g�p����File Down Loader (Apache Module)
-------------------------------------------------------------

�������o���邩
�EHandlerSocket�v���g�R���o�R��MariaDB����f�[�^���擾����
�EURL����DB�̎�L�[���w�肷���MIME Type�Ɠ��e���擾���ĕ\������


���m�F��
�ECentOS Stream 8 x64
�EApache 2.4.37 (rpm)
�EMariaDB(10.3.27) (Handler Socket�v���O�C������)


���ݒ���@
���R���p�C������ۂ́uhttp-devel�v�uapxs�v�uapr-devel�v�urpm-build�v�A�ugcc-c++�v�A�umake�v���K�v

$ make clean
$ make
$ su
# make install


��make install���s���ƁA
httpd.conf�Ɉȉ��̕����񂪒ǉ������
LoadModule hsdl_module   (Module Path)/mod_hsdl.so

��Apache Conf
http://localhost/hs/(key) �ŃA�N�Z�X���������ꍇ

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

���ݒ��Apache�ċN��

��MariaDB
1. MariaDB��mariadb-server.cnf�ɉ��L�̐ݒ��ǋL����
[mysqld]
plugin_maturity=beta

2. ��L��ݒ肵����mysql�R�}���h��MariaDB�Ƀ��O�C�������L�����s����
install plugin handlersocket soname 'handlersocket.so';

3. MariaDB��mariadb-server.cnf��[mysqld]�ɉ��L�̐ݒ��ǋL����

handlersocket_port=9998
handlersocket_port_wr=9999
handlersocket_address=
handlersocket_verbose=0
handlersocket_timeout=300
handlersocket_threads=16
thread_concurrency=128
open_files_limit=65535


4. �e�[�u���ݒ�

CREATE TABLE `hstest1` (
  `k` varchar(20) NOT NULL DEFAULT '',
  `m` varchar(50) DEFAULT NULL,
  `v` mediumblob,
  PRIMARY KEY (`k`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

5. �f�[�^�o�^
Insert��
INSERT INTO hstest1 VALUES('test','image/jpeg','(binary data)')

k = Key
m = MimeType
v = Value

��L��insert�������s�����
http://localhost/hs/test
�Łuk�v��test�̏���Apache���ԋp����

�������b�g
�EApache��URL���L�ڂ���΃f�[�^�𒊏o�ł���
�E�t�@�C���T�[�o��HTTP�T�[�o�𕪗��ł���

���f�����b�g
�E���t�@�C���̒���DL���኱�x��
�E�l�b�g���[�N�󋵂Ɉˑ����邽�߁A������x���ꍇ�͂���Ȃ�̑��x�����o�Ȃ�
�E�傫���t�@�C���͋��
�E�L���b�V���͑Ή����Ă��Ȃ�

�����p�ɂ������Ă̖Ɛӎ���
�{�\�t�g�E�F�A�̊J���҂́A�{�\�t�g�E�F�A�̕s�ғ��A�ғ��s�ǂ��܂�
�@��������r�S�ېӔC�A���̑��ۏؐӔC�𕉂�Ȃ����̂Ƃ��܂��B
�܂��A�{�\�t�g�E�G�A�̊J���҂́A�{�\�t�g�E�F�A�̏��i���A
����̖ړI�ɑ΂���K�����ɂ��āA�����Ȃ�ۏ؂�����Ȃ����̂Ƃ��܂��B
