<?
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
	
	$dataroot='/home/li-bo/web/data/';
	//$dataroot='../recordings/';
	//$dataroot='/Applications/MAMP/htdocs/rtmplite/';
	
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
	$phraseid = clean($_POST['phraseid']);
	
	$_SESSION['SESS_CURRENT_PHRASE_ID']=$phraseid;
	
	//$allowedExts = array("wav", "flv");
	$allowedExts = array("flv");
	$extension = end(explode(".", $_FILES["userfile"]["name"]));
	
	$audio = $dataroot . 'audioRecorder/user'.$userid.'/phrase'.$phraseid.'.'.$extension;
	
	if ((/*($_FILES["userfile"]["type"] == "audio/wav")
		|| */($_FILES["userfile"]["type"] == "video/x-flv"))
		&& ($_FILES["userfile"]["size"] < 20000000)
		&& in_array($extension, $allowedExts))
	{
		if ($_FILES["userfile"]["error"] > 0)
		{
			echo "Return Code: " . $_FILES["userfile"]["error"] . "<br />";
		}
		else
		{
			if($debug) 
			{
				echo "Upload: " . $_FILES["userfile"]["name"] . "<br />";
				echo "Type: " . $_FILES["userfile"]["type"] . "<br />";
				echo "Size: " . ($_FILES["userfile"]["size"] / 1024) . " Kb<br />";
				echo "Temp file: " . $_FILES["userfile"]["tmp_name"] . "<br />";
			}

			$ret=move_uploaded_file($_FILES["userfile"]["tmp_name"], $audio);
			if(!$ret) {
				print "Upload file failed!"."<br/>";
				print $_FILES["userfile"]["tmp_name"]."<br/>";
				print $audio."<br/>";
				exit(1);
			}else if($debug){
				echo "Stored in: " . $audio;
			}
			
		}
	}
	else
	{
		echo "Invalid file";
		exit(1);
	}
	
	// first check whether the phrased is recorded previously or not
	$qry = "select * from recordings where userid=".$userid." and phraseid=".$phraseid;
	$result = mysql_query($qry);
	if($debug) { print('##'.$qry.'##'); print('##'.$result.'##');}
	
	if($result) {
		if(mysql_num_rows($result)>=1) {
			if($debug) print('## recording existing only updating the time ##');
			
			$upd_qry = "update recordings set time=NOW() where userid=".$userid." and phraseid=".$phraseid;
			$upd_res = mysql_query($upd_qry);
			if($debug) { print('##'.$upd_qry.'##'); print('##'.$upd_res.'##');}
			
			if($upd_res){
				if($debug) print('## recording updated! ##');
			}else {
				if($debug) { 
					print('## update recording failed! ##');
					print mysql_error();
				}
			}
		}else {
			if($debug) print('## no previous recording, insert new record ##');
			
			$ins_qry = "insert into recordings (userid, phraseid, time, audio, exemplar) values (".$userid.", ".$phraseid.", NOW(), '".$audio."', 'n')";
			$ins_res = mysql_query($ins_qry);
			if($debug) { print('##'.$ins_qry.'##'); print('##'.$ins_res.'##');}
			
			if($ins_res) {
				if($debug) print('## new recording created!##');
			}else {
				if($debug) {
					print('##insert recording failed!##');
					print mysql_error();
				}
				print('Creating the new recording failed! Please retry later!');
			}
		
		}
	}
	
	mysql_close($link);
	
	header('location:bg_student_scoring.php?phraseid='.$phraseid);
?>

