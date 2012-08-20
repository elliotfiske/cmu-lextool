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
	
	$useremail = $_SESSION['SESS_MEMBER_EMAIL'];
	
	if($debug) print('##Current User:'.$useremail.'##');
	
	$userid = $_SESSION['SESS_MEMBER_ID'];
	
	if(!isset($_SESSION['SESS_CURRENT_PHRASE_ID'])){
		// get the phrases user has recorded
		$qry = "select phraseid from recordings where userid=".$userid." order by phraseid";
		$result = mysql_query($qry);
		if($debug) print($qry);
		$startid = -1;
		if($result) {
			if(mysql_num_rows($result)>0){
				$recorded = mysql_fetch_all($result);
				for($i = 0; $i < count($recorded); $i ++) {
					if($debug) {
						print_r($recorded[$i]);
						print('<br>');
					}
					if($recorded[$i]['phraseid'] != $i +1) {
						break;
					}else {
						$startid = $i;
					}
				}
			}
		}
		$startid = $startid + 1;
	}else {
		$startid=$_SESSION['SESS_CURRENT_PHRASE_ID']-1;
	}
	if($debug) print('startid='.$startid.'<br>');
	
	// get the phrases
	$qry = "select * from phrases order by id";
	$result = mysql_query($qry);
	if($debug) print($qry);
	if($result) {
		if(mysql_num_rows($result)>0) {
			// get phrase list
			$phrases = mysql_fetch_all($result);
			
			if($startid == -1) {
				$startid = 0;
			}else if ($startid >= count($phrases) -1){
				$startid = count($phrases) -1;
			}
			$_SESSION['SESS_CURRENT_PHRASE_ID']=$startid+1;
			
			if($debug) print('startid='.$_SESSION['SESS_CURRENT_PHRASE_ID'].'<br>');
			for($i = 0; $i < count($phrases); $i ++) {
				if($i +1 == $_SESSION['SESS_CURRENT_PHRASE_ID'] ) {
					echo '<option selected value="'.$phrases[$i]['id'].'" label="Sentence '.$i.'">'.$phrases[$i]['name'].' </option>';
				}else {
					echo '<option value="'.$phrases[$i]['id'].'" label="Sentence '.$i.'">'.$phrases[$i]['name'].' </option>';
				}
				if($debug) {
					print_r($phrases[$i]);
					print('<br>');
				}
			}
		}
	}else {
		die("Query languages failed!");
	}
	
	
	if($errflag == false) {
	
	} 
	
	if($errflag) {	
		print('Unexpected error occurred!');
	}
	
	mysql_close($link);
?>



