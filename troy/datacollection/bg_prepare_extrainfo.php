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
	
	// init the session info
	$_SESSION['SESS_MEMBER_NAME']='';
	$_SESSION['SESS_MEMBER_SEX']='f';
	$_SESSION['SESS_MEMBER_BIRTHDATE']='';
	$_SESSION['SESS_MEMBER_LANG']='1'; // default English
	$_SESSION['SESS_MEMBER_REGION']='233'; // default United States
	
	// array to store validation errors
	$errmsg_arr = array();
	$_SESSION['ERRMSG_ARR'] = $errmsg_arr;
	
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
	
	// get the languages
	$qry = "select * from languages";
	$result = mysql_query($qry);
	if($debug) print($qry);
	if($result) {
		if(mysql_num_rows($result)>0) {
			// get language list
			$langs = mysql_fetch_all($result);
			$_SESSION['SESS_LANGLIST'] = $langs;
			if($debug) {
				print("<br>langs:<br>");
				print_r($langs);
				print("<br>session langs:<br>");
				print_r($_SESSION['SESS_LANGLIST']);
			}
		}
	}else {
		die("Query languages failed!");
	}
	
	// get the regions
	$qry = "select * from regions";
	$result = mysql_query($qry);
	if($debug) print($qry);
	if($result) {
		if(mysql_num_rows($result)>0) {
			// get region list
			$regions = mysql_fetch_all($result);
			$_SESSION['SESS_REGIONLIST'] = $regions;
			if($debug) {
				print("<br>regions:<br>");
				print_r($regions);
				print("<br>session regions:<br>");
				print_r($_SESSION['SESS_REGIONLIST']);
			}
		}
	}else {
		die("Query regions failed!");
	}
	
	// check whether current user has extra info
	$qry = "select * from users where email='".$useremail."'";
	$result = mysql_query($qry);
	if($debug) print($qry);
	if($result) {
		if(mysql_num_rows($result)>0) {
			$member = mysql_fetch_assoc($result);
			if(strcmp($member['name'], '')!=0) {
				$_SESSION['SESS_MEMBER_NAME'] = $member['name'];
				if($debug) print('<br>Name:'.$member['name'].'<br>');
			}
			if(strcmp($member['birthdate'], '')!=0) {
				$_SESSION['SESS_MEMBER_BIRTHDATE'] = $member['birthdate'];
			}
			if(strcmp($member['sex'], '')!=0) {
				$_SESSION['SESS_MEMBER_SEX'] = $member['sex'];
			}
			if(strcmp($member['langid'], '')!=0) {
				$_SESSION['SESS_MEMBER_LANG'] = $member['langid'];
			}
			if(strcmp($member['regionid'], '')!=0) {
				$_SESSION['SESS_MEMBER_REGION'] = $member['regionid'];
			}
		}
	}
		
	
	if($errflag == false) {
		session_write_close();
		header("location:extrainfo.php");
	} 
	
	if($errflag) {	
		print('Unexpected error occurred!');
	}
	
	mysql_close($link);
	
?>


