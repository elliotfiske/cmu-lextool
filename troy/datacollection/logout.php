<?php
	// start session
	session_start();
	
	// unset the variables stored in session
	unset($_SESSION['SESS_MEMBER_ID']);
	unset($_SESSION['SESS_MEMBER_EMAIL']);
	session_destroy();
	
	// redirect to login page
	header('location: login.php');
?>
