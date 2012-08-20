<?php

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
	
	// function to sanitize values received from the form 
	function clean($str) {
		$str = @trim($str);
		if(get_magic_quotes_gpc()) {
			$str = stripslashes($str);
		}
		return mysql_real_escape_string($str);
	}
	
	// sanitize the POST values
	$useremail = strtolower(clean($_POST['useremail']));
	$password = sha1(clean($_POST['password']));
	
	if($debug) print('##Original[email:'.$_POST['useremail'].'||password:'.$_POST['password'].']##');
	if($debug) print('##cleaned[email:'.$useremail.'||password(sha1):'.$password.']##');
	
	// Input validations
	if($useremail == ''){
		$errmsg_arr[] = 'Please input an Email address to login!';
		$errflag = true;
	}
	
	if($password == ''){
		$errmsg_arr[] = 'Please input a password to login!';
		$errflag = true;
	}
	
	if($errflag == false ) {
		// query to check database for record
		$qry = "select * from users where email='".$useremail."'";
		$result = mysql_query($qry);
		
		if($debug) print($qry);
		
		// check whether the query was successful or not
		if($result) {
			if(mysql_num_rows($result) == 1) {
			
				// login successful
				$member = mysql_fetch_assoc($result);
				
				if(strcmp($member['password'], $password)==0) {
				
					session_regenerate_id();
					// set session 
					$_SESSION['SESS_MEMBER_ID'] = $member['id'];
					// put email in session
					$_SESSION['SESS_MEMBER_EMAIL'] = $member['email'];
					// user type
					$_SESSION['SESS_MEMBER_TYPE'] = $member['type'];
					
					if($debug) {
						print('##login successfully##');
					}
				}else {
					if($debug) {
						print('##password error##');
					} 
					$errmsg_arr[] = 'You have entered a incorrect password!';
					$errflag = true;
					$errtype = "user_exists";
				}
					
			} else {
				if($debug) {
					print('##login failed##');
				} 
				$errmsg_arr[] = 'You have entered an incorrect Email address or password!';
				$errflag = true;
			}
		} else {
			die("Query failed");
		}
		
	}

	if($errflag == false) {
		// check whether the current student user has other priviledges or not
		$t = strspn($_SESSION['SESS_MEMBER_TYPE'], '1');
		
		if($debug) print('## checking user type ##');
		
		if($t > 1) {
			// redirect to selection page
			if($debug) print('##Multiple roles##');
			header("location:selection.php");
		} elseif(strcmp($_SESSION['SESS_MEMBER_TYPE'], '10000000')==0) {
			// redirect to student page
			if($debug) print('## Student ##');
			header("location:student_main.php");
		} elseif(strcmp($_SESSION['SESS_MEMBER_TYPE'], '01000000')==0) {
			// redirect to exemplary page
			if($debug) print('## Exemplary ##');
			header("location:exemplary_main.php");
		} else {
			if($debug) {
				print('##user type error##');
			}
			
			$errmsg_arr[] = 'For now, only Student and Exemplary modules are open for access!';
			$errflag = true;
		}
	} 
	
	if($errflag) {	
		// if there are input validations, redirect back to the login form
		$_SESSION['ERRMSG_ARR'] = $errmsg_arr;
		$_SESSION['ERRMSG_TYPE'] = $errtype;
		session_write_close();
		if($debug) print_r($_SESSION['ERRMSG_ARR']);
		if($debug) print_r($errmsg_arr);
		header("location:login.php#toerrorpagefromlogin");
	}
	
	mysql_close($link);
	
?>


