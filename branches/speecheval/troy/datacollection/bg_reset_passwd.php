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
	
	// function to generate a random alphanumeric string
	function generate_random_string($str_length = 8) {
        $alpha = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz';
        $numeric = '0123456789';
        
        $alpha_len = rand(1, $str_length - 1);
        $alpha_str = substr(str_shuffle($alpha), 0, $alpha_len);
        $numeric_str = substr(str_shuffle($numeric), 0, $str_length - $alpha_len);
        
        return str_shuffle($alpha_str.$numeric_str); 
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
	$useremail = strtolower(clean($_POST['useremail']));
	
	if($debug) print('##Original[email:'.$_POST['useremail'].']##');
	if($debug) print('##cleaned[email:'.$useremail.']##');
	
	// Input validations
	if($useremail == ''){
		$errmsg_arr[] = 'Please input an Email address to login!';
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
				// user exists, change the password
				$newpasswd = generate_random_string();
				$password = sha1($newpasswd);
				
				// query to change password
				$update_qry = "update users set password='".$password."' where email='".$useremail."'";
				$update_result = mysql_query($update_qry);
				
				if($update_result) {
					if($debug) print('## password changed!##');
					
					// send the password to the mail address
					$subject = "Your New Password";
					$message = "Your new password is as follows:\n---------------------------------\nPassword: ".$newpasswd;
					$message = $message."\n---------------------------------\nPlease change the password as soon as possible.\n\nThis email was automatically generated.";
					if(mail($useremail, $subject, $message, "FROM: talknicer.net")) {
						if($debug) print('## mail sent successfully!##');
						$_SESSION['SESS_MEMBER_EMAIL'] = $useremail;
					}else{
						if($debug) print('## send email failed ##');
						$errmsg_arr[] = 'Sending Email Failed, Please Contact Site Admin!';
						$errflag = true;
					}
					
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
					print('##no user exists##');
				} 
				$errmsg_arr[] = 'The user identified by this email: <strong>'.$useremail.'</strong> does not exist!';
				$errflag = true;
			}
		} else {
			die("Query failed");
		}
		
	}

	if($errflag == false) {
		session_write_close();
		header("location:findpasswd.php#tosuccesspagefromreset");
	} 
	
	if($errflag) {	
		// if there are input validations, redirect back to the login form
		$_SESSION['ERRMSG_ARR'] = $errmsg_arr;
		$_SESSION['ERRMSG_TYPE'] = $errtype;
		session_write_close();
		if($debug) print_r($_SESSION['ERRMSG_ARR']);
		if($debug) print_r($errmsg_arr);
		header("location:findpasswd.php#toerrorpagefromreset");
	}
	
	mysql_close($link);
	
?>


