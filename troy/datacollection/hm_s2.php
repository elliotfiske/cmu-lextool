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
	
	session_start();
	
	if(!isset($_SESSION['SESS_MEMBER_EMAIL'])) {
		header("location:login.php");
	}
	
	if($debug) print_r($_POST);
	
	if(isset($_POST['stuid'])){
		$_SESSION['SESS_HM_SELECTED_STUID']=$_POST['stuid'];
	}

	if($debug) print_r($_SESSION);	
?>
<!DOCTYPE html>
 <html lang="en" class="no-js">
    <head>
        <meta charset="UTF-8" />
        <title>Pronunciation Evaluation Data Collection</title>
        <meta name="viewport" content="width=device-width, initial-scale=1.0"> 
        <meta name="description" content="Pronunciation Evaluation Data Collection" />
        <link rel="stylesheet" type="text/css" href="css/demo.css" />
        <link rel="stylesheet" type="text/css" href="css/style.css" />
		<link rel="stylesheet" type="text/css" href="css/animate-custom.css" />
		<script src="js/main.js" type="text/javascript"></script>
		<script src="js/hm.js" type="text/javascript"></script>
    </head>
    <body>
        <div class="container">
            <header>
                <h1>Pronunciation Evaluation <span>Data Collection</span></h1>
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
                        <div id="login" class="animate form">
							<form  action="hm_s3.php" method="post">
								<h1>Choose A Phrase</h1> 
								<p> 
                                    <label for="datalist"> Phrases: </label>
                                    <select id="datalist" name="phraseid" onchange="selectchange();">
										<?php
											session_start();
	
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
											$qry = "select recordings.phraseid, phrases.name from recordings, phrases where recordings.phraseid=phrases.id and recordings.userid=".$_SESSION['SESS_HM_SELECTED_STUID']." order by recordings.phraseid";
											$result = mysql_query($qry);
											if($debug) print($qry);
											if($result) {
												if(mysql_num_rows($result)>0){
													$recorded = mysql_fetch_all($result);
													for($i = 0; $i < count($recorded); $i ++) {
														if($debug) {
															print_r($recorded[$i]);
															print('<br>');
														}
														if($i==0){
															echo '<option selected value="'.$recorded[$i]['phraseid'].'" label="Phrase '.$i.'">'.$recorded[$i]['name'].'</option>';
														}else {
															echo '<option value="'.$recorded[$i]['phraseid'].'" label="Phrase '.$i.'">'.$recorded[$i]['name'].'</option>';
														}
													}
												}
											}
										?>
									</select>
                                </p>
                                <p class="prompt">
									<label id="lbldata"></label>
                                </p>
								<p class="login button"> 
                                    <input type="submit" value="Select" /> 
								</p>
								<p class="change_link">
									<a href="hm_s1.php">Change Student</a>
								</p>
							</form>
                        </div>
						
                    </div>
                </div>  
            </section>
        </div>
    </body>
</html>
