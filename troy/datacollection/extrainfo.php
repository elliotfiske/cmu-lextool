<?php 
	session_start();
	
	if(!isset($_SESSION['SESS_MEMBER_EMAIL'])) {
		header("location:login.php");
	}
?>

<!DOCTYPE html>
 <html lang="en" class="no-js">
    <head>
        <meta charset="UTF-8" />
        <title>Pronunciation Evaluation Data Collection</title>
        <meta name="viewport" content="width=device-width, initial-scale=1.0"> 
        <meta name="description" content="Pronunciation Evaluation Data Collection" />
        <link rel="stylesheet" type="text/css" href="css/demo.css" />
        <link rel="stylesheet" type="text/css" href="css/style_extrainfo.css" />
		<link rel="stylesheet" type="text/css" href="css/animate-custom.css" />
		<script src="js/main.js" type="text/javascript"></script>
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
					<?php 
						if (!isset($_SESSION['SESS_NEW_MEMBER']) || strcmp($_SESSION['SESS_NEW_MEMBER'], 'y')!=0)
						{
							if(strcmp($_SESSION['SESS_MEMBER_TYPE'], '10000000')==0)
							{
								echo '<a href="student_main.php">Home</a>';
							}else if (strcmp($_SESSION['SESS_MEMBER_TYPE'], '01000000')==0)
							{
								echo '<a href="exemplary_main.php">Home</a>';
							}else {
								echo '<a href="selection.php">Home</a>';
							}
							echo '<a href="changepasswd.php">Change Password</a>';
							echo '<a href="bg_prepare_extrainfo.php"  class="current-demo">Update Info</a>';
							echo '<a href="logout.php">Log Out</a>';
						}
					?>
				</nav>
            </header>

            <section>				
                <div id="container_demo" >
					<a class="hiddenanchor" id="toerrorpagefromregister"></a>
                    <a class="hiddenanchor" id="toerrorpagefromreset"></a>
                    
                    <div id="wrapper">
                        <div id="register" class="animate form">
                            <form  action="userregister.php" autocomplete="on" method="post">
                                <h1> Additional Information </h1> 

                                <p> 
                                    <label for="namesignup" class="youname"> Name: </label> &nbsp;
                                    <input id="namesignup" name="namesignup" required="required" placeholder="eg. Tom"
                                    <?php 
                                    	if(isset($_SESSION['SESS_MEMBER_NAME']) && strcmp($_SESSION['SESS_MEMBER_NAME'], '')!=0) {
                                    		echo 'value="'.$_SESSION['SESS_MEMBER_NAME'].'"';
                                    	}
                                    ?>
                                    /> 
                                </p>
                                
                                <p>
                                	<label for="sexsignup" class="yousex"> Sex: </label> &nbsp;
                                	<?php
                                		if(!isset($_SESSION['SESS_MEMBER_SEX']) || strcmp($_SESSION['SESS_MEMBER_SEX'], 'm')==0) {
                                			echo '<input type="radio" name="sexsignup" id="male" value="m" checked="checked"/>&nbsp;<label for="male">Male</label>&nbsp;';
                                			echo '<input type="radio" name="sexsignup" id="female" value="f"/>&nbsp;<label for="female">Female</label>&nbsp;';
                                		} else {
                                			echo '<input type="radio" name="sexsignup" id="male" value="m"/>&nbsp;<label for="male">Male</label>&nbsp;';
                                			echo '<input type="radio" name="sexsignup" id="female" value="f" checked="checked"/>&nbsp;<label for="female">Female</label>&nbsp;';
                                		}
                                	?>
                                </p>
                                <p> 
                                    <label for="birthdatesignup" class="youbirthdate">Your birthdate: </label> &nbsp;
                                    <input id="birthdatesignup" name="birthdatesignup" required="required" type="date" placeholder="Formate: YYYY-MM-DD, eg. 1980-01-01" min="1860-01-01" max="2012-12-12" 
                                    <?php
                                    	if(isset($_SESSION['SESS_MEMBER_BIRTHDATE']) && strcmp($_SESSION['SESS_MEMBER_BIRTHDATE'], '')!=0) {
                                    		echo 'value="'.$_SESSION['SESS_MEMBER_BIRTHDATE'].'"';
                                    	}
                                    ?>
                                    />
                                </p>

								<p>
									<label for="langlist">Native Language:</label> &nbsp;
									<select id="langlist" name="langlist">
										<?php
											if(!isset($_SESSION['SESS_LANGLIST'])) {
												// unexpected error!
											}else{
												$langs = $_SESSION['SESS_LANGLIST'];
												for($i = 0; $i < count($langs); $i ++) {
													echo '<option value="'.$langs[$i]['id'].'"';
													if(strcmp($_SESSION['SESS_MEMBER_LANG'],$langs[$i]['id'])==0) {
														echo " selected";
													}
													echo '>'.$langs[$i]['description'].'</option>';
												}
											}
										?>
									</select>
								</p>
								
								<p class="itemlist">
									<label for="regionlist">Place you lived when you were between 6 and 8:</label> &nbsp;
									<select id="regionlist" name="regionlist" onchange="">
										<?php
											if(!isset($_SESSION['SESS_REGIONLIST'])) {
												// unexpected error!
											}else{
												$regions = $_SESSION['SESS_REGIONLIST'];
												for($i = 0; $i < count($regions); $i ++) {
													echo '<option value="'.$regions[$i]['id'].'"';
													if(strcmp($_SESSION['SESS_MEMBER_REGION'],$regions[$i]['id'])==0) {
														echo " selected";
													}
													echo '>'.$regions[$i]['description'].'</option>';
												}
											}
										?>
									</select>
								</p>

                                <p class="signin button"> 
									<input type="submit" formaction="bg_update_extrainfo.php" value="Update"/> 
								</p>

                                <p class="change_link">  
                                	<?php 
										$arr = $_SESSION['ERRMSG_ARR'];
										reset($arr);
										$msg = "";
										while (list(, $value) = each($arr)) {
											$msg = $msg . $value . "<br/>";
										}
										echo $msg; 
									?>	
								</p>

                            </form>

                        </div>

						

                    </div>

                </div>  

            </section>

        </div>

    </body>

</html>



