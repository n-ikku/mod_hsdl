<?php 
/* mod_hsdl用 データ登録サンプル 
   PHP 7.2で確認
	CREATE TABLE `hstest_table1` (
	`s` bigint(20) unsigned NOT NULL AUTO_INCREMENT,
	`k` varchar(20) NOT NULL DEFAULT '',
	`m` varchar(50) DEFAULT NULL,
	`v` mediumblob DEFAULT NULL,
	`c` timestamp NOT NULL DEFAULT current_timestamp() ON UPDATE current_timestamp(),
	`l` int(11) DEFAULT NULL,
	`n` text DEFAULT NULL,
	PRIMARY KEY (`k`),
	UNIQUE KEY `s` (`s`)
	) ENGINE=InnoDB AUTO_INCREMENT=9 DEFAULT CHARSET=utf8
*/
	include_once("./func.plib");
	if($_POST["ld"] == 1){
		if(count($_POST["dels"])>0) {
			file_del($_POST["dels"]);
			header("location:".$_SERVER["PHP_SELF"]);
		} else {
			$_SESSION['prefix'] = $_POST["prefix"];
			file_up_load("sendfile",$_POST["method"],$_POST["comment"]);
		}
		if($_POST["nv"] == 1){
			exit(0);
		}
	}
?>
<html>
<head>
<script src="./jquery.js" type="text/javascript"></script>
<script src="./plus.js" type="text/javascript"></script>
</head>
<body>
<div ondragover="onDragOver(event)" ondrop="onDrop(event)">
<form method="post" action="<?php echo($_SERVER['PHP_SELF'])?>"  ENCTYPE="multipart/form-data">
<input type="hidden" NAME="ld" value="1">
<table>
<tr>
<td nowrap>Prefix : </td><td><input type="text" id="prefix" NAME="prefix" size="24" value="<?php echo($_SESSION['prefix']); ?>"></td>
</tr>
<tr>
<td nowrap>Comment : </td><td><input type="text" NAME="comment" size="24"></td>
</tr>
<tr>
<td nowrap>Method : </td><td><input type="text" NAME="method" size="24"></td>
</tr>
<tr>
<td nowrap>Send file : </td><td><input type="FILE" NAME="sendfile[]" size="60"></td>
</tr>
<tr>
<td colspan="2" nowrap><input type="submit" value="登録／変更"></td>
</tr>
</table>
</form>
<table border="1">
<?php
	echo(make_tbl_th(array("name","mime type","timestamp","size","note","delete")));
	try{
		$db = new PDO( hsdl_dbNm, hsdl_dbUsr, hsdl_dbPwd);
		$sql = "select k,m,c,l,n,s from ".hsdl_dbTbl . " order by s desc";
		$stmt = $db->prepare($sql);
		$stmt->execute();
		$red = $stmt->fetchAll();
		for ($i=0;$i<count($red);$i++){
			echo(make_tbl_td(array("<a href='". hsdl_uri . $red[$i][0]."' target='_blank'>".$red[$i][0]."</a>",
							$red[$i][1],
							$red[$i][2],
							$red[$i][3],
							$red[$i][4],
							make_tbl_del(array($red[$i][5],$red[$i][2],$red[$i][3]))
						)));
		}
    }catch(Exception $e){
		error_log($e);
	}
?>
</table>
</div>
</body>
</html>
