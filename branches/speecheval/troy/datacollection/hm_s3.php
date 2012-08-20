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
	$phraseid=$_POST['phraseid'];
	$_SESSION['SESS_HM_SELECTED_PHRASEID']=$phraseid;
	$stuid=$_SESSION['SESS_HM_SELECTED_STUID'];
	
	// get user email
	$qry = "select email from users where id=".$stuid;
	$result = mysql_query($qry);
	if($debug) print($qry);
	if ($result) {
		if(mysql_num_rows($result)>0){
			$itms=mysql_fetch_assoc($result);
			if($debug) print_r($itms);
			$stuemail=$itms['email'];
		}
	}else{
		print('MySQL query error!');
		exit(1);
	}
	
	// get the phrase content
	$qry = "select name from phrases where id=".$phraseid;
	$result = mysql_query($qry);
	if($debug) print($qry);
	if ($result) {
		if(mysql_num_rows($result)>0){
			$itms=mysql_fetch_assoc($result);
			if($debug) print_r($itms);
			$phrase=$itms['name'];
		}
	}else{
		print('MySQL query error!');
		exit(1);
	}
	
	// get the recording id and audio file path
	$qry = "select id, audio from recordings where userid=".$stuid." and phraseid=".$phraseid;
	$result = mysql_query($qry);
	if($debug) print($qry);
	if ($result) {
		if(mysql_num_rows($result)>0){
			$itms=mysql_fetch_assoc($result);
			if($debug) print_r($itms);
			$recordingid=$itms['id'];
			$_SESSION['SESS_HM_SELECTED_RECID']=$recordingid;
			$audiofile=$itms['audio'];
		}
	}else{
		print('MySQL query error!');
		exit(1);
	}
	
	// get the phoneme list, mapping from id to phone
	$qry = "select name from phonemes order by id";
	$result = mysql_query($qry);
	if($debug) print($qry."<br/>");
	if ($result) {
		if(mysql_num_rows($result)>0){
			$itms=mysql_fetch_all($result);
			if($debug) print_r($itms);
			for($k = 0; $k < count($itms); $k ++) {
				$phn2id[$itms[$k]['name']]=$k+1;
			}
			if($debug) {
				print('<br/>');
				print_r($phn2id);
			}
		}
	}else{
		print('MySQL query error!');
		exit(1);
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
							<form  action="hm_s4.php" autocomplete="on" method="post"> 
								<h1>Assess the Pronunciation</h1>
								<p>
									<label><strong><?php echo "[".$stuemail."]"; ?></strong></label>
								</p>
								<p class="prompt">
									<label><?php echo $phrase; ?></label>
								</p>
								<p>
									<audio id="sentplayer" controls >
										<?php
											//$wavfile=str_replace("/home/troy/Documents/GSoC/launchpad/", "http://localhost/", $audiofile);
											$wavfile=str_replace("/home/li-bo/web/", "http://talknicer.net/~li-bo/", $audiofile);
											$wavfile=str_replace(".flv", ".wav", $wavfile);
											echo "<source src=\"".$wavfile."\" type=\"audio/wav\" />";
										?>
									</audio>
									<script>
										var audio = document.getElementById('sentplayer');
										var segmentEnd;

										audio.addEventListener('timeupdate', function (){
											if (segmentEnd && audio.currentTime >= segmentEnd) {
												audio.pause();
											}   
											console.log(audio.currentTime);
										}, false);

										function playSegment(startTime, endTime){
											segmentEnd = endTime;
											audio.currentTime = startTime;
											audio.play();
										}

										function printValue(sliderID, textbox) {
											var x = document.getElementById(textbox);
											var y = document.getElementById(sliderID);
											x.value = y.value;
										}
									</script>
								</p>
								<p>
									
									<table border="">
										<?php
										
											if($debug) print("audiofile:".$audiofile."<br/>");
											echo "<tr><th><strong>Phoneme</strong></th><th><strong>Acoustic Score</strong></th><th><strong>Duration Score</strong></th></tr>";
											$alnfile=str_replace(".flv", ".phonesegdir/phrase".$phraseid.".lab", $audiofile);
											if($debug) print($alnfile."<br/>".$audiofile."<br/>");
											$fp=fopen($alnfile, "r") or exit("Unable to open file!");
											$i=0;
											while(!feof($fp)){
												$line=fgets($fp);
												if($debug) echo $line."<br/>";
												$n=sscanf($line, "%f %d %s", $t, $x, $ph);
												//print($n." ".$t." ".$x." ".$ph."<br/>");
												if($n==3){
													$tim[$i]=$t;
													$phns[$i]=$ph;
													$i=$i+1;
												}
											}
											fclose($fp);
											$totphns=0;
											$pid=0;
											for($i=0;$i<count($tim);$i++){
												if(strcmp($phns[$i], "SIL")!=0){
													$s=substr($phns[$i],0,1);
													if(strcmp($s, '+')!=0) {
														echo '<tr>';
														echo '<th><a href="javascript:playSegment('.$tim[$i].', '.$tim[$i+1].');">'.$phns[$i].'</a><input type=hidden name=pid_p'.$pid.' value='.$phn2id[$phns[$i]].' /></th>';
														echo '<th><input type=text id=ac_p'.$pid.' name=ac_p'.$pid.' size=3 value=2.5 />&nbsp<input type=range step=0.01 min=0 max=5 value=2.5 id=slidac_p'.$pid.' onchange="printValue(\'slidac_p'.$pid.'\', \'ac_p'.$pid.'\')" /></th>';
														echo '<th><input type=text id=dur_p'.$pid.' name=dur_p'.$pid.' size=3 value=2.5 />&nbsp<input type=range step=0.01 min=0 max=5 value=2.5 id=sliddur_p'.$pid.' onchange="printValue(\'sliddur_p'.$pid.'\', \'dur_p'.$pid.'\')" /></th>';
														echo "</tr>";
														$totphns=$totphns+1;
														$pid=$pid+1;
													}
												}
											}
											echo '<input type=hidden name=totphns value='.$totphns.' />';

										?>
									</table>
								</p>

								<p class="login button"> 
                                    <input type="submit" value="Submit" /> 
								</p>
								
								<p class="change_link">   
									<a href="hm_s2.php" class="to_register"> Change phrase </a>
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

