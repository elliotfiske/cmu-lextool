<?php

	// function to sanitize values received from the form 
	function clean($str) {
		$str = @trim($str);
		if(get_magic_quotes_gpc()) {
			$str = stripslashes($str);
		}
		return mysql_real_escape_string($str);
	}
	
	// function to fetch all the rows
	function mysql_fetch_all($res) {
		while($row=mysql_fetch_assoc($res)) {
			$return[] = $row;
		}
		return $return;
	}
	
	$debug = false;

	// start session
	session_start();
	
	// array to store validation errors
	$errmsg_arr = array();
	
	// validation error flag
	$errflag = false;
	
	// error type 
	$errtype = 'default';
	
	// connect to mysql server
	$link = mysql_connect('localhost', 'li-bo', '1cat2dogs');
	if(!$link) {
		die('Failed to connect to server: ' . mysql_error());
	} else {
		if($debug) print('##connection setup!##');
	}
	
	// select database
	$db = mysql_select_db('gsoc');
	if(!$db) {
		die("Unable to select database");
	} else {
		if($debug) print('##database selected!##');
	}

	// get the phoneme list, mapping from id to phone
	$qry = "select name from phonemes order by id";
	$result = mysql_query($qry);
	if($debug) print($qry."<br/>");
	if ($result) {
		if(mysql_num_rows($result)>0){
			$itms=mysql_fetch_all($result);
			if($debug) print_r($itms);
			for($k = 0; $k < count($itms); $k ++) {
				$phn2id[$itms[$k]['name']]=$k+1;
			}
			if($debug) {
				print('<br/>');
				print_r($phn2id);
			}
		}
	}else{
		print('MySQL query error!');
		exit(1);
	}
	
	if($debug) {
		print('<br/>');
		print_r($_POST);
	}
	
	// check the word pronunciation first
	for($wi=0;$wi<count($_SESSION['SESS_ADDPHRASE_WORDS']);$wi++){
		// array of the user inputed phone list
		$pro_user=str_word_count(trim(clean($_POST['prow'.$wi])), 1);
		
		$qry = "select * from words where name='\"".clean($_SESSION['SESS_ADDPHRASE_WORDS'][$wi])."\"'";
		$result = mysql_query($qry);
		$proid[$wi]=-1;
		$wrdid[$wi]=-1;
		
		if($debug) print($qry."<br/>");
		if ($result) {
			if(mysql_num_rows($result)<=0){
				// word not exist, insert with 0 pronunciation
				$ins_qry="insert into words (name, numpro) values ('\"".clean($_SESSION['SESS_ADDPHRASE_WORDS'][$wi])."\"', 0)";
				$pro_res=mysql_query($pro_qry);
				if($debug) print($pro_qry."<br/>");
				if($pro_res){
					if($debug) print('Insert '.$_SESSION['SESS_ADDPHRASE_WORDS'][$wi].' into database successfully!<br/>');
					$proid[$wi]=0;
				}else{
					print('Insert '.$_SESSION['SESS_ADDPHRASE_WORDS'][$wi].' into database failed!<br/>');
					exit(1);
				}
				
				// redo this iteration
				$wi=$wi-1;
			}else {
				$itm = mysql_fetch_assoc($result);
				$wrdid[$wi]=$itm['id'];
				
				// process each pronunciation
				for($pi=0;$pi<$itm['numpro'] && $proid[$wi]==-1;$pi ++){
					$pro_qry="select phnid from pronunciations where wordid=".$itm['id']." and proid=".$pi." order by phnpos";
					$pro_res=mysql_query($pro_qry);
					if($debug) print($pro_qry."<br/>");
					if($pro_res){
						if(mysql_num_rows($pro_res)>0){
							$phns=mysql_fetch_all($pro_res);
							
							// continue only if they have same number of phones
							if(count($pro_user)==count($phns)){
								
								for($pj=0;$pj<count($phns);$pj++){
									if($phns[$pj][phnid]!=$phn2id[$pro_user[$pj]]) break;
								}
								
								if($pj==count($phns)){
									// find it
									$proid[$wi]=$pi;
								}
							}
						}
					}
				}
				
				if($proid[$wi]==-1){
					// the pronunciation not exists, update the database
					$upd_qry="update words set numpro=".($itm['numpro']+1)." where id=".$itm['id'];
					$upd_res=mysql_query($upd_qry);
					if($debug) print($upd_qry."<br/>");
					if($upd_res) print('Update word numpro successfully!<br/>');
					else{
						print('Update word numpro failed!');
						exit(1);
					}
					
					$ins_qry="insert into pronunciations (wordid, proid, phnid, phnpos) values";
					for($pi=0;$pi<count($pro_user);$pi++){
						$ins_qry=$ins_qry." (".$wrdid[$wi].", ".$itm['numpro'].", ".$phn2id[$pro_user[$pi]].", ".$pi.")";
						if($pi!=count($pro_user)-1) $ins_qry=$ins_qry.",";
					}
					$ins_qry=$ins_qry.";";
					$ins_res=mysql_query($ins_qry);
					if($debug)print($ins_qry."<br/>");
					if($ins_res) print("Update pronunciation successfully!<br/>");
					else{
						print("update pronunciation failed!");
						exit(1);
					}
					$proid[$wi]=$itm['numpro'];
				}
			}
		}else{
			print("mysql execution error!<br/>");
			exit(1);
		}
	}
	
	// add the phrase to database first
	$qry = "insert into phrases (name) values ('".clean($_SESSION['SESS_ADDPHRASE_PHRASE'])."')";
	$result = mysql_query($qry);
	if($debug) print($qry."<br/>");
	if ($result) {
		if($debug) print('Insert phrase successfully!<br/>');
	}else{
		print('Insert phrase failed!<br/>');
		exit(1);
	}
	
	// get phrase id
	$phraseid=-1;
	$qry = "select id from phrases where name='".clean($_SESSION['SESS_ADDPHRASE_PHRASE'])."'";
	$result = mysql_query($qry);
	if($debug) print($qry."<br/>");
	if ($result) {
		if(mysql_num_rows($result)>0){
			$itm=mysql_fetch_assoc($result);
			$phraseid=$itm['id'];
		}
	}
	if($phraseid==-1){
		print("Error retrieving phrase from database!");
		exit(1);
	}
	
	// add the phrase info
	$ins_qry="insert into phraseinfo (phraseid, wordid, wordpos, proid, postag) values";
	for($wi=0;$wi<count($_SESSION['SESS_ADDPHRASE_WORDS']);$wi++){
		$ins_qry=$ins_qry."(".$phraseid.", ".$wrdid[$wi].", ".$wi.", ".$proid[$wi].", '".clean($_POST['posw'.$wi])."')";
		if($wi!=count($_SESSION['SESS_ADDPHRASE_WORDS'])-1) $ins_qry=$ins_qry.",";
		else $ins_qry=$ins_qry.";";
	}
	$ins_res=mysql_query($ins_qry);
	if($debug)print($ins_qry."<br/>");
	if($ins_res) {
		if($debug) print("Phraseinfo updated successfully!<br/>");
	}else{
		print("Phraseinfo updated failed!");
		exit(1);
	}
	
	mysql_close($link);
	
	header('location:newphrase_succeed.php');
?>
	
	
