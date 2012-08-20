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
	
	// get the phrases user has recorded
	$qry = "select distinct(users.id), users.email from users, recordings where users.id=recordings.userid and users.type='10000000' order by users.id";
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
				if($i==0){
					echo '<option selected value="'.$recorded[$i]['id'].'" label="Student '.$i.'">'.$recorded[$i]['email'].' </option>';
				}else {
					echo '<option value="'.$recorded[$i]['id'].'" label="Student '.$i.'">'.$recorded[$i]['email'].' </option>';
				}
			}
		}
	}else {
		die("Query languages failed!");
	}
	
	if($errflag) {	
		print('Unexpected error occurred!');
	}
	
	mysql_close($link);
?>




