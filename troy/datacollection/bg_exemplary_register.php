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
	$useremail = strtolower(clean($_POST['emailsignup']));
	$password = sha1(clean($_POST['passwordsignup']));
	
	if($debug) print('##Original[email:'.$_POST['emailsignup'].'||password:'.$_POST['passwordsignup'].']##');
	if($debug) print('##cleaned[email:'.$useremail.'||password(sha1):'.$password.']##');
	
	// Input validations
	if($useremail == ''){
		$errmsg_arr[] = 'Please input an Email address to sign up!';
		$errflag = true;
	}
	
	if($password == ''){
		$errmsg_arr[] = 'Please input a password to login!';
		$errflag = true;
	}
	
	// query to make suer there is not such user existing now
	if($errflag == false ){
		$qry = "select * from users where email='".$useremail."'";
		$result = mysql_query($qry);
		
		if($debug) { print('##'.$qry.'##'); print('##'.$result.'##');}
		
		if($result){
		
			if(mysql_num_rows($result)==1) {
			
				if($debug) print('## 1 record found ##');
				
				// user exists checking whether it is an exempary or not
				$member = mysql_fetch_assoc($result);
				
				if($debug) var_dump($member);
				
				$type = $member['type'];
				
				// exemplary has the type of '01000000'
				if(strcmp($type[1], '1')==0) {
					// already an existing exemplary user
					if($debug) printf('## user as an exemplary already registered! ##');
				
					$errmsg_arr[] = 'The email: <strong>'.$useremail.'</strong> is already registered as an exemplary!';
					$errflag = true;
					$errtype = 'user_exists';
				} else {
					// existing non-exemplary member, adding extra type, but ignoring the password
					$type[1]='1'; // add exemplary type to the current user
					$update_qry = "update users set type='".$type."' where email='".$member['email']."'";
					$update_result = mysql_query($update_qry);
					if($debug) print($update_qry);
		
					if($update_result) {
						if($debug) print('## user updated!##');
						
					} else {
						if($debug) {
							print('##update failed!##');
							print mysql_error();
						}
						$errmsg_arr[] = 'Update to exemplary user failed! Please retry later!';
						$errflag = true;
					}
				}
				
			} else {
			
				if($debug) print('## no records found ##');
				
				// insert the new user
				$insert_qry = "insert into users (email, password, type, time) values ('".$useremail."', '".$password."', '01000000', NOW())";
				$insert_result = mysql_query($insert_qry);
				
				if($debug) print($insert_qry);
				
				
				if($insert_result) {
					if($debug) print('## new user created!##');
					
				} else {
					if($debug) {
						print('##insert failed!##');
						print mysql_error();
					}
					$errmsg_arr[] = 'Creating the new user failed! Please retry later!';
					$errflag = true;
				}
			}
		} else {
			die("Query failed!");
		}
	}

	// query to get user id
	if($errflag == false) {
		$qry = "select * from users where email='".$useremail."'";
		$result = mysql_query($qry);
		
		if($debug) print($qry);
		
		// check whether the query was successful or not
		if($result) {
			if(mysql_num_rows($result) == 1) {
				// login successful
				session_regenerate_id();
				$member = mysql_fetch_assoc($result);
				
				// set session 
				$_SESSION['SESS_MEMBER_ID'] = $member['id'];
				// put email in session
				$_SESSION['SESS_MEMBER_EMAIL'] = $member['email'];
				// put user type
				$_SESSION['SESS_MEMBER_TYPE'] = $member['type'];
				
				$_SESSION['SESS_NEW_MEMBER'] = 'y';
				
				// close session writing
				session_write_close();
				
				if($debug) {
					print('##login successfully##');
				} 
			} else {
				if($debug) {
					print('##login failed##');
				} 
				$errmsg_arr[] = 'You have entered an incorrect Email address or password!';
				$errflag = true;
			}
		} 
	}
	
	if($errflag == false) {
		// as registered as an exemplary, users need to fill in extra information
		header("location:bg_prepare_extrainfo.php");
	}else {
		// if there are input validations, redirect back to the login form
		$_SESSION['ERRMSG_ARR'] = $errmsg_arr;
		$_SESSION['ERRMSG_TYPE'] = $errtype;
		session_write_close();
		if($debug) print_r($errmsg_arr);
		header("location:login.php#toerrorpagefromregister");
	}
	
	mysql_close($link);
?>
	
