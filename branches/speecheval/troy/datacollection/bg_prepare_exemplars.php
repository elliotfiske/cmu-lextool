<?php

	// function to sanitize values received from the form 
	function clean($str) {
		$str = @trim($str);
		if(get_magic_quotes_gpc()) {
			$str = stripslashes($str);
		}
		return mysql_real_escape_string($str);
	}
	
	// function to fetch all the rows
	function mysql_fetch_all($res) {
		while($row=mysql_fetch_assoc($res)) {
			$return[] = $row;
		}
		return $return;
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
	
	// get the phrases user has recorded
	$qry = "select id from phrases";
	$result = mysql_query($qry);
	if($debug) print($qry);
	if($result) {
		if(mysql_num_rows($result)>0){
			$phraseids = mysql_fetch_all($result);
			for($i = 0; $i < count($phraseids); $i ++) {
				if($debug) {
					print_r($phraseids[$i]);
					print('<br>');
				}
				
				$curid=$phraseids[$i][id];
				$exp_qry="select audio from recordings where phraseid=".$curid." and exemplar='y' order by time";
				$exp_result=mysql_query($exp_qry);
				if ($debug) print($exp_qry);
				if ($exp_result) {
					if(mysql_num_rows($result)>0){
						$exemplars=mysql_fetch_all($exp_result);
						echo "<p id=\"exemplars_phrase".$i."\">";
						echo "<audio controls=\"controls\">";
						for($k = 0; $k < 5 && $k < count($exemplars); $k ++) {
							// change to corresponding path
							//$fname=str_replace("/home/troy/Documents/GSoC/launchapd/", "http://localhost/", $exemplars[$k][audio]);
							$fname=str_replace("/home/li-bo/web/", "http://talknicer.net/~li-bo/", $exemplars[$k][audio]);
							$wav=str_replace(".flv", ".wav", $fname);
							if($debug) print $wav;
							echo "<source src=\"".$wav."\" type=\"audio/wav\" />";
						}
						echo "</audio>";
						echo "</p>";
					}else{
						echo "<p id=\"exemplars_phrase".$i."\" hidden>";
						echo "<br>Whoops! Currently there is no exemplar recordings in the database for this phrase.<br>";
						echo "</p>";
					}
				}
			}
		}
	}else {
		die("Query languages failed!");
	}
	
	
	if($errflag == false) {
	
	} 
	
	if($errflag) {	
		print('Unexpected error occurred!');
	}
	
	mysql_close($link);
	
?>


