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
	
	// word segmentation
	$phrase=trim($_POST['newphrase']);
	$str=strtolower($phrase);
	$words=str_word_count($str, 1);
	
	$_SESSION['SESS_ADDPHRASE_PHRASE']=$phrase;
	$_SESSION['SESS_ADDPHRASE_WORDS']=$words;

	// get the phoneme list, mapping from id to phone
	$qry = "select name from phonemes order by id";
	$result = mysql_query($qry);
	
	if($debug) print($qry);
	if ($result) {
		if(mysql_num_rows($result)>0){
			$itms=mysql_fetch_all($result);
			if($debug) print_r($itms);
			for($k = 0; $k < count($itms); $k ++) {
				$phones[$k+1]=$itms[$k]['name'];
			}
			$_SESSION['SESS_ADDPHRASE_PHONES']=$phones;
			if($debug) {
				print('<br/>');
				print_r($phones);
			}
		}
	}else{
		print('MySQL query error!');
		exit(1);
	}
	
	$_SESSION['SESS_ADDPHRASE_CURRENTWORD']=0;

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
		<script src="js/asjs.js" type="text/javascript"></script>
    </head>
    <body>
        <div class="container">
            <header>
                <h1>Pronunciation Evaluation <span>New Phrase Page</span></h1>
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
							<form  action="bg_add_newphrase.php" autocomplete="on" method="post"> 
								<h1>Choose the Pronunciation</h1>
								
								<p class="prompt">
									<label><?php echo $_SESSION['SESS_ADDPHRASE_PHRASE']; ?></label>
								</p>
								<p>
									<table border="">
										<?php
											
											echo "<tr><th><strong>Word</strong></th><th><strong>Pronunciation</strong></th><th><strong>PoS Tag</strong></th></tr>";
											for($k=0;$k<count($_SESSION['SESS_ADDPHRASE_WORDS']);$k++){
												echo "<tr>";
												echo "<th>".$_SESSION['SESS_ADDPHRASE_WORDS'][$k]."</th>";
												echo "<th>";

												// check the database for pronunciation for the word
												// get the phoneme list, mapping from id to phone
												$qry = "select * from words where name='\"".$_SESSION['SESS_ADDPHRASE_WORDS'][$k]."\"'";
												$result = mysql_query($qry);
												
												if($debug) print($qry);
												if ($result) {
													if(mysql_num_rows($result)>0){
														$itm = mysql_fetch_assoc($result);
														
														// process each pronunciation
														for($pi=0;$pi<$itm['numpro'];$pi ++){
															$pro_qry="select phnid from pronunciations where wordid=".$itm['id']." and proid=".$pi." order by phnpos";
															$pro_res=mysql_query($pro_qry);
															if($debug) print($pro_qry);
															if($pro_res){
																if(mysql_num_rows($pro_res)>0){
																	$phns=mysql_fetch_all($pro_res);
																	$phntext='';
																	for($pj=0;$pj<count($phns);$pj++){
																		$phntext=$phntext.$_SESSION['SESS_ADDPHRASE_PHONES'][$phns[$pj][phnid]]." ";
																	}
																	if($pi==0){
																		echo "<input class=phntext type=text name=prow".$k." value=\"".$phntext."\"/><br/>"; // displace the correct pronunciation
																		echo "<input type=radio name=w".$k." id=w".$k."p".$pi." value=".$pi." checked=checked onclick='this.form.prow".$k.".value=\"".$phntext."\";'/>";
																		
																	}else{
																		echo "<input type=radio name=w".$k." id=w".$k."p".$pi." value=".$pi." onclick='this.form.prow".$k.".value=\"".$phntext."\";'/>";
																	}
																	echo "<label for=w".$k."p".$pi." >".$phntext."</label><br/>";
																}
															}
														}
																	
													}else{
														// the word not in dict
														echo "<input class=phntext type=text name=prow".$k."/><br/>";
													}
												}else{
													// the word not in dict
													echo "<input class=phntext type=text name=prow".$k."/><br/>";
													
												}
												echo "</th>";
												
												echo "<th>";
												echo "<input type=radio name=posw".$k." id=posw".$k." value=q checked/>";
												echo "<label for=posw".$k." >Quantifier</label>&nbsp";
												echo "<input type=radio name=posw".$k." id=posw".$k." value=n/>";
												echo "<label for=posw".$k." >Noun</label>&nbsp";
												echo "<input type=radio name=posw".$k." id=posw".$k." value=v/>";
												echo "<label for=posw".$k." >Verb</label>&nbsp";
												echo "<input type=radio name=posw".$k." id=posw".$k." value=-/>";
												echo "<label for=posw".$k." >Negative</label>&nbsp";
												echo "<input type=radio name=posw".$k." id=posw".$k." value=w/>";
												echo "<label for=posw".$k." >Adverb</label>&nbsp";
												echo "<input type=radio name=posw".$k." id=posw".$k." value=m/>";
												echo "<label for=posw".$k." >Adjactive</label>&nbsp";
												echo "<input type=radio name=posw".$k." id=posw".$k." value=o/>";
												echo "<label for=posw".$k." >Pronoun</label>&nbsp";
												echo "<input type=radio name=posw".$k." id=posw".$k." value=s/>";
												echo "<label for=posw".$k." >Possessive</label>&nbsp";
												echo "<input type=radio name=posw".$k." id=posw".$k." value=p/>";
												echo "<label for=posw".$k." >Preposition</label>&nbsp";
												echo "<input type=radio name=posw".$k." id=posw".$k." value=c/>";
												echo "<label for=posw".$k." >Conjunction</label>&nbsp";
												echo "<input type=radio name=posw".$k." id=posw".$k." value=a/>";
												echo "<label for=posw".$k." >Article</label>&nbsp";
												echo "</th>";
																
												echo "</tr>";
											}
										?>
									</table>
								</p>

								<p class="login button"> 
                                    <input type="submit" value="Submit" /> 
								</p>
								
								<p class="change_link">   
									<a href="newphrase.php" class="to_register"> Add another phrase </a> or <a href="logout.php" class="to_register"> Log Out </a>
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

