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
	$qry = "select * from recordings where userid=".$userid." and phraseid=".$phraseid;
	$result = mysql_query($qry);
	
	if($debug) print($qry);
	
	// check whether the query was successful or not
	if($result) {
		if(mysql_num_rows($result) == 1) {
		
			// login successful
			$record = mysql_fetch_assoc($result);
			
			$flvfile=$record['audio'];
			$record_id=$record['id'];
			
			if($debug) print "<br> FLV: ".$flvfile;
			
			if(file_exists($flvfile)){
				exec('bash models/s1_student_scoring.sh '.$flvfile, $out, $ret);
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
	
	// get the scores and update to database
	// as this is system generated score, we use robot@talknicer.net as judge,
	$judgeid=4;
	
	$scofile=str_replace(".flv", ".sco", $flvfile);
	$fp=fopen($scofile, 'r');
	if($fp){
		$cnt=0;
		$tot_acscore=0.0;
		$tot_durscore=0.0;
		$insertmode=true;
		while(fscanf($fp, "%s %f %f", $phn, $acscore, $durscore)){
			// get the phone id
			$phn_qry="select id from phonemes where name='".$phn."'";
			$phn_result=mysql_query($phn_qry);
			if($debug) print('<br>'.$phn_qry);
			if($phn_result){
				if(mysql_num_rows($phn_result) == 1) {
					$record = mysql_fetch_assoc($phn_result);
					$phnid=$record['id'];
					if($debug) print '<br>Phone: '.$phn.', id: '.$phnid;
					
					if($cnt==0){
						// the first one check whether should insert or update
						$chk_qry="select * from scores where recordingid=".$record_id." and type='p' and judge=".$judgeid;
						$chk_result=mysql_query($chk_qry);
						if($debug) print '<br>'.$chk_qry;
						if($chk_result){
							if(mysql_num_rows($chk_result)>0){
								$insertmode=false;
							}
						}
					}
					
					if($insertmode){
						$upd_qry="insert into scores (recordingid, type, phoneid, phonepos, judge, acousticscore, durationscore) values (".$record_id.", 'p', ".$phnid.", ".$cnt.", ".$judgeid.", ".$acscore.", ".$durscore.")";
					}else{
						$upd_qry="update scores set acousticscore=".$acscore.", durationscore=".$durscore." where recordingid=".$record_id." and type='p' and phoneid=".$phnid." and phonepos=".$cnt." and judge=".$judgeid;
					}
					$upd_result=mysql_query($upd_qry);
					if($debug) print '<br>'.$upd_qry;
					if($upd_result){
						if($debug) print '<br>Score for '.$phn.' updated successfully!';
					}else{
						if($debug) {
							print('<br>##Phone'.$phn.' is not updated in database!##');
						} 
						$errmsg_arr[] = '<br>##Phone'.$phn.' is not updated in database!##';
						$errflag = true;
						$errtype = "phone_not_updated";
					}
					
				}else{
					if($debug) {
						print('<br>##Phone'.$phn.' is not in database!##');
					} 
					$errmsg_arr[] = '<br>##Phone'.$phn.' is not in database!##';
					$errflag = true;
					$errtype = "phone_not_exists";
				}

			}else{
				die("Phone not existing.");
			}
			
			$cnt=$cnt+1;
			$tot_acscore=$tot_acscore+$acscore;
			$tot_durscore=$tot_durscore+$durscore;
		}
		
		// insert record for the whole sentence
		$tot_acscore=$tot_acscore/$cnt;
		$tot_durscore=$tot_durscore/$cnt;
		if($insertmode){
			$upd_qry="insert into scores (recordingid, type, judge, acousticscore, durationscore) values (".$record_id.", 's', ".$judgeid.", ".$tot_acscore.", ".$tot_durscore.")";
		}else{
			$upd_qry="update scores set acousticscore=".$tot_acscore.", durationscore=".$tot_durscore." where recordingid=".$record_id." and type='s' and judge=".$judgeid;
		}
		$upd_result=mysql_query($upd_qry);
		if($debug) print '<br>'.$upd_qry;
		if($upd_result){
			if($debug) print '<br>Score for recording '.$record_id.' updated successfully!';
		}else{
			if($debug) {
				print('<br>##Recording '.$record_id.' is not updated in database!##');
			} 
			$errmsg_arr[] = '<br>##Recording'.$record_id.' is not updated in database!##';
			$errflag = true;
			$errtype = "recording_not_updated";
		}
	}else {
		if($debug) {
			print('<br>##Cannot open score file!##');
		} 
		$errmsg_arr[] = 'Cannot open score file!';
		$errflag = true;
		$errtype = "file_not_exists";
	}
	fclose($fp);
	
	if($errflag) {	
		// if there are input validations, redirect back to the login form
		$_SESSION['ERRMSG_ARR'] = $errmsg_arr;
		$_SESSION['ERRMSG_TYPE'] = $errtype;
		if($debug) print_r($_SESSION['ERRMSG_ARR']);
		if($debug) print_r($errmsg_arr);
	}else{
		$_SESSION['ERRMSG_ARR'] = array();
		if($debug) print "<br>Success!"	;
		//store the score in session
		$_SESSION['SESS_STUDENT_AC_SCORE']=$tot_acscore;
		$_SESSION['SESS_STUDENT_DUR_SCORE']=$tot_durscore;
		// go to next phrase by default
		$_SESSION['SESS_CURRENT_PHRASE_ID']=$_SESSION['SESS_CURRENT_PHRASE_ID']+1;
	}
	
	header("location:student_score.php");
	
	mysql_close($link);
	
?>
	



