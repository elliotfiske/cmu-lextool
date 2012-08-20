<?
	// function to sanitize values received from the form 
	function clean($str) {
		$str = @trim($str);
		if(get_magic_quotes_gpc()) {
			$str = stripslashes($str);
		}
		return mysql_real_escape_string($str);
	}
	
   	$debug = true;
	
	// start session
	session_start();
	
	// array to store validation errors
	$errmsg_arr = array();
	
	// validation error flag
	$errflag = false;
	
	// error type
	$errtype = 'default';
	
	define('FFMPEG_LIBRARY', '/usr/bin/ffmpeg ');
	
	$dataroot='/home/li-bo/web/data/';
	
	$flvname=$dataroot . $_POST["flvname"] . '.flv';
	$wavname=$dataroot . $_POST["flvname"] . '.wav';
	
	if($debug) {
		print($flvname."<br>");
		print($wavname."<br>");
	}
	
	$exec_string = FFMPEG_LIBRARY.' -y -i '.$flvname.' -vn -f wav -ar 16000 -ac 1 '.$wavname.' 2>&1';
	print($exec_string.'\n'); 
	exec($exec_string, $output, $ret_var); //where exec is the command used to execute shell command in php
	print_r($output);
	
	
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
	
	$userid = $_SESSION['SESS_MEMBER_ID'];
	$fname = clean($_POST["flvname"]);
	$fname = strstr($fname, "phrase");
	$suffix = strstr($fname, "_");
	$fname = str_replace($suffix, "", $fname);
	$phraseid = str_replace("phrase", "", $fname);
	$audio = clean($flvname);
	
	if($debug) {
		print($userid."<br>");
		print($phraseid."<br>");
	}
	
	$qry = "insert into recordings (userid, phraseid, time, audio, exemplar) values (".$userid.", ".$phraseid.", NOW(), '".$audio."', 'n')";
	$result = mysql_query($qry);
	
	if($debug) { print('##'.$qry.'##'); print('##'.$result.'##');}
	
	if($result) {
		if($debug) print('## new recording created!##');
		
	} else {
		if($debug) {
			print('##insert recording failed!##');
			print mysql_error();
		}
		print('Creating the new recording failed! Please retry later!');
	}
		
	
	mysql_close($link);
   
?>

