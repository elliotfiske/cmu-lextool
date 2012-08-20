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
	
	function datecheck($str) {
		if(date('Y-m-d', strtotime($str)) == $str) {
			return true;
		} else {
			return false;
		}
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
	
	if($debug) {
		print("<br>Posts:<br>");
		print_r($_POST);
	}
	
	// all the information are available do the update
	
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
	
	$useremail = clean($_SESSION['SESS_MEMBER_EMAIL']);
	// prepare the session info
	$_SESSION['SESS_MEMBER_NAME']=clean($_POST['namesignup']);
	$_SESSION['SESS_MEMBER_SEX']=clean($_POST['sexsignup']);
	$_SESSION['SESS_MEMBER_LANG']=clean($_POST['langlist']);
	$_SESSION['SESS_MEMBER_REGION']=clean($_POST['regionlist']); 
	
	$birthdate = clean($_POST['birthdatesignup']);
	if($debug) print "<br>birthdate: ".$birthdate."<br>";
	if(datecheck($birthdate)) {
		$_SESSION['SESS_MEMBER_BIRTHDATE']=$birthdate;
	}else {
		$_SESSION['SESS_MEMBER_BIRTHDATE']='';
		$errmsg_arr[] = 'Invalide birthday date!';
		$_SESSION['ERRMSG_ARR'] = $errmsg_arr;
		session_write_close();
		if($debug) {
			print("<br>");
			print_r($_SESSION['ERRMSG_ARR']);
		}
		header("location:extrainfo.php");
		exit;
	}
	
	$qry = "update users set name='".$_SESSION['SESS_MEMBER_NAME']."', sex='".$_SESSION['SESS_MEMBER_SEX']."', birthdate='".$birthdate."', langid=".$_SESSION['SESS_MEMBER_LANG'].", regionid=".$_SESSION['SESS_MEMBER_REGION']." where email='".$useremail."'";
	if($debug) print($qry."<br>");
	$result = mysql_query($qry);
	if($result) {
		if($debug) print("User info updated successfully!<br>");
		$_SESSION['SESS_NEW_MEMBER']='n';
		unset($_SESSION['SESS_NEW_MEMBER']);
	}else {
		print(mysql_error()."<br>");
		die("Update user extra info failed unexpectedly!");
	}

	if($errflag == false) {
		// check whether the current student user has other priviledges or not
		$t = strspn($_SESSION['SESS_MEMBER_TYPE'], '1');
		
		if($debug) print('## checking user type ##');
		
		if($t > 1) {
			// redirect to selection page
			if($debug) print('##Multiple roles##');
			session_write_close();
			header("location:selection.php");
		} elseif(strcmp($_SESSION['SESS_MEMBER_TYPE'], '10000000')==0) {
			// redirect to student page
			if($debug) print('## Student ##');
			session_write_close();
			header("location:student_main.php");
		} elseif(strcmp($_SESSION['SESS_MEMBER_TYPE'], '01000000')==0) {
			// redirect to exemplary page
			if($debug) print('## Exemplary ##');
			session_write_close();
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
		header("location:findpasswd.php#toerrorpagefromreset");
	}
	
	mysql_close($link);
	
?>



