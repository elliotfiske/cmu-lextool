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
	
	session_start();
	
	if(!isset($_SESSION['SESS_MEMBER_EMAIL'])) {
		header("location:login.php");
		exit(1);
	}
	
	$debug = false;
	$errflag = false;
	
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
	
	// basic info
	$recid=$_SESSION['SESS_HM_SELECTED_RECID'];
	$type='p';
	$judge=$_SESSION['SESS_MEMBER_ID'];
	
	if($debug){
		print($recid."<br/>".$type."<br/>".$judge."<br/>");
		print_r($_POST);
	}
		
	// check whether there are already scores 
	$qry="select recordingid from scores where recordingid=".$recid." and judge=".$judge." and type='".$type."'";
	$result = mysql_query($qry);
	if($debug) print($qry);
	if ($result) {
		if(mysql_num_rows($result)>0){
			// delete previous results
			$del_qry="delete from scores where recordingid=".$recid." and judge=".$judge." and type='".$type."'";
			$result = mysql_query($del_qry);
			if($debug) print($del_qry);
			if ($result) {
				if($debug) print('Delete old records successfully!');
				else{
					$errflag=true;
				}
			}else {
				$errflag=true;
			}
		}
	}else{
		$errflag=true;
	}
		
	$qry="insert into scores (recordingid, type, judge, phoneid, phonepos, acousticscore, durationscore) values";
	for($i=0;$i<$_POST['totphns'];$i++){
		$qry=$qry."(".$recid.", '".$type."', ".$judge.", ".$_POST['pid_p'.$i].", ".$i.", ".$_POST['ac_p'.$i].", ".$_POST['dur_p'.$i].")";
		if($i!=$_POST['totphns']-1) $qry=$qry.",";
		else $qry=$qry.";";
	}
	$result = mysql_query($qry);
	if($debug) print($qry);
	if ($result) {
		if($debug) print('Insert scores successfully!');
	}else{
		$errflag=true;
	}
	
	mysql_close($link);
	
?>
<!DOCTYPE html>
 <html lang="en" class="no-js">
    <head>
        <meta charset="UTF-8" />
        <title>Pronunciation Evaluation Data Collection</title>
        <meta name="viewport" content="width=device-width, initial-scale=1.0"> 
        <meta name="description" content="Pronunciation Evaluation Data Collection" />
        <link rel="stylesheet" type="text/css" href="css/demo.css" />
        <link rel="stylesheet" type="text/css" href="css/style_mainpage.css" />
		<link rel="stylesheet" type="text/css" href="css/animate-custom.css" />
		<link href='http://fonts.googleapis.com/css?family=Chewy' rel='stylesheet' type='text/css'>
		<script src="js/main.js" type="text/javascript"></script>
		

    </head>
    <body>
        <div class="container">
            <header>
                <h1>Pronunciation Evaluation <span>Judge Page</span></h1>
                <nav class="codrops-demos">
					<span>Current User: 
						<strong>
							<?php
								echo $_SESSION['SESS_MEMBER_EMAIL'];
							?>
						</strong>
					</span>
					<a href="selection.php"  class="current-demo">Home</a>
					<a href="changepasswd">Change Password</a>
					<a href="bg_prepare_extrainfo.php">Update Info</a>
					<a href="logout.php">Log Out</a>
				</nav>
            </header>
            <section>			
                <div id="container_demo" >
                    <div id="wrapper">
                        <div id="main" class="animate form">
							<form  action="bg_judge_phrase.php" autocomplete="on" method="post"> 
								<h1>Assess the Pronunciation</h1>
								<p class="prompt">
									<label>
										<?php 
											if($errflag){
												echo "Whoops!<br/><br/>Error occurred!<br/><br/>";
											}else{
												echo "Great!<br/><br/>Scores are successfully updated!<br/><br/>";
											}
										?>
									</label>
								</p>
								
								<p class="change_link">   
									<a href="hm_s2.php" class="to_register"> Choose another phrase </a>
								</p>
							</form>
                        </div>
						
                    </div>
                </div>  
            </section>
            <footer class="page_info">
            	<br/>
            	gsoc@talknicer.net
            	<br/>
            </footer>
        </div>
    </body>
</html>


