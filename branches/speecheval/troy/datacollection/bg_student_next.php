<?php

	// start session
	session_start();
	
	// back to the exemplar recording page and next phrase
	$_SESSION['SESS_CURRENT_PHRASE_ID']=$_SESSION['SESS_CURRENT_PHRASE_ID']+1;
	
	header("location:student_main.php");
	
?>
	


