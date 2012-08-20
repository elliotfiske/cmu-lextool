<?php

	// functions
	
	// function to sanitize values received from the form 
	function clean($str) {
		$str = @trim($str);
		if(get_magic_quotes_gpc()) {
			$str = stripslashes($str);
		}
		return mysql_real_escape_string($str);
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
	
	// sanitize the POST values
	$oldpassword = sha1(clean($_POST['oldpassword']));
	$newpassword = sha1(clean($_POST['newpassword']));
	
	if($debug) print '<br>old:'.$_POST['oldpassword']."<br>New:".$_POST['newpassword']."<br>";
	
	$useremail = $_SESSION['SESS_MEMBER_EMAIL'];
	
	if($errflag == false ) {
		// query to check database for record
		$qry = "select * from users where email='".$useremail."' and password='".$oldpassword."'";
		$result = mysql_query($qry);
		
		if($debug) print($qry);
		
		// check whether the query was successful or not
		if($result) {
			if(mysql_num_rows($result) == 1) {
				// user exists, change the password
				
				// query to change password
				$update_qry = "update users set password='".$newpassword."' where email='".$useremail."'";
				$update_result = mysql_query($update_qry);
				
				if($update_result) {
					if($debug) print('## password changed!##');

				} else {
					if($debug) {
						print('##password change failed!##');
						print mysql_error();
					}
					$errmsg_arr[] = 'Password change was failed! Please retry later!';
					$errflag = true;
				}

			} else {
				if($debug) {
					print('##incorrect old password##');
				} 
				$errmsg_arr[] = 'Incorrect old password!';
				$errflag = true;
			}
		} else {
			die("Query failed");
		}
		
	}

	if($errflag == false) {
		session_write_close();
		header("location:changepasswd.php#tosuccesspagefromreset");
	} 
	
	if($errflag) {	
		// if there are input validations, redirect back to the login form
		$_SESSION['ERRMSG_ARR'] = $errmsg_arr;
		$_SESSION['ERRMSG_TYPE'] = $errtype;
		session_write_close();
		if($debug) print_r($_SESSION['ERRMSG_ARR']);
		if($debug) print_r($errmsg_arr);
		header("location:changepasswd.php#toerrorpagefromreset");
	}
	
	mysql_close($link);
	
?>


