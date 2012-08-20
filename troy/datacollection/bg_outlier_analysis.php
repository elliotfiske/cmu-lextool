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
	
	$userid=$_SESSION['SESS_MEMBER_ID'];
	$phraseid=$_GET['phraseid'];
	$_SESSION['SESS_CURRENT_PHRASE_ID']=$phraseid;
	
	if($debug) print '<br>userid: '.$userid.'<br>phraseid: '.$phraseid;
	
	// query to check database for record
	$qry = "select audio from recordings where userid=".$userid." and phraseid=".$phraseid;
	$result = mysql_query($qry);
	
	if($debug) print($qry);
	
	// check whether the query was successful or not
	if($result && !$errflag) {
		if(mysql_num_rows($result) == 1) {
		
			// login successful
			$record = mysql_fetch_assoc($result);
			
			$flvfile=$record['audio'];
			if($debug) print "<br> FLV: ".$flvfile;
			
			if(file_exists($flvfile)){
				//$out=system('bash models/s0_outlier_analysis.sh '.$flvfile);
				exec('bash models/s0_outlier_analysis.sh '.$flvfile, $out, $ret);
				if($debug) {
					print '<br><br>';
					print_r($out);
				}
			}else {
				if($debug) {
					print('<br>##FLV file not exists for exemplar outlier analysis##');
				} 
				$errmsg_arr[] = 'FLV file not exists for exemplar outlier analysis!';
				$errflag = true;
				$errtype = "file_not_exists";
			}
			
		}else {
			if($debug) {
				print('<br>##No recording in database for exemplar outlier analysis!##');
			} 
			$errmsg_arr[] = 'No recording in database for exemplar outlier analysis!';
			$errflag = true;
			$errtype = "recording_not_exists";
		}
	}else {
		die("Query failed");
	}
	
	// get the per
	$per=0.0;
	$perfile=str_replace(".flv", ".per", $flvfile);
	$fp=fopen($perfile, 'r');
	if($fp && !$errflag){
		if(fscanf($fp, "%f", $per)){
			if($per < 0.5){
				if($debug) print '<br> Exemplar!';
				
				// update the recording table to set it as exemplar
				$upd_qry="update recordings set exemplar='y' where userid=".$userid." and phraseid=".$phraseid;
				$upd_result = mysql_query($upd_qry);
	
				if($debug) print($upd_qry);
				
				// check whether the query was successful or not
				if($upd_result) {
					if($debug) print '<br> Update the database!';
				
					// back to the exemplar recording page and next phrase
					$_SESSION['SESS_CURRENT_PHRASE_ID']=$_SESSION['SESS_CURRENT_PHRASE_ID']+1;
					
					header("location:is_exemplar.php");
				}else {
					if($debug) {
						print('<br>##Failed to update in database to exemplar!##');
					} 
					$errmsg_arr[] = 'Failed to update in database to exemplar!';
					$errflag = true;
					$errtype = "database_update_failure";
				}
				
			}else{
				if($debug) print '<br> Not Exemplar!';
				
				// update the recording table to set it as non-exemplar
				$upd_qry="update recordings set exemplar='n' where userid=".$userid." and phraseid=".$phraseid;
				$upd_result = mysql_query($upd_qry);
	
				if($debug) print($upd_qry);
				
				// check whether the query was successful or not
				if($result) {
					if($debug) print '<br> Update the database!';
				
					// ask the user whether to redo the recording or ignore
					header("location:not_exemplar.php");
					
				}else {
					if($debug) {
						print('<br>##Failed to update in database to exemplar!##');
					} 
					$errmsg_arr[] = 'Failed to update in database to exemplar!';
					$errflag = true;
					$errtype = "database_update_failure";
				}
				
			}
			fclose($fp);
		}else {
			if($debug) {
				print('<br>##Empty PER file!##');
			} 
			$errmsg_arr[] = 'Empty PER file!';
			$errflag = true;
			$errtype = "empty_file";
		}
	}else {
		if($debug) {
			print('<br>##Cannot open PER file!##');
		} 
		$errmsg_arr[] = 'Cannot open PER file!';
		$errflag = true;
		$errtype = "file_not_exists";
	}
	
	if($errflag) {	
		// if there are input validations, redirect back to the login form
		$_SESSION['ERRMSG_ARR'] = $errmsg_arr;
		$_SESSION['ERRMSG_TYPE'] = $errtype;
		session_write_close();
		if($debug) print_r($_SESSION['ERRMSG_ARR']);
		if($debug) print_r($errmsg_arr);
		//header("location:login.php#toerrorpagefromlogin");
		header("location:not_exemplar.php");
	}
	
	mysql_close($link);
	
?>
	


