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
        <link rel="stylesheet" type="text/css" href="css/style_findpasswd.css" />
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
					<label id="userid" hidden><?php echo $_SESSION['SESS_MEMBER_ID']; ?></label>
					<a href="exemplary_main.php">Home</a>
					<a href="changepasswd.php" class="current-demo">Change Password</a>
					<a href="bg_prepare_extrainfo.php">Update Info</a>
					<a href="logout.php">Log Out</a>
				</nav>
            </header>

            <section>				
                <div id="container_demo" >
                    <a class="hiddenanchor" id="tosuccesspagefromreset"></a>
                    <a class="hiddenanchor" id="toerrorpagefromreset"></a>
                    <a class="hiddenanchor" id="toresetfromerrorpage"></a>

                    <div id="wrapper">
                        <div id="reset" class="animate form">
                            <form  action="bg_change_passwd.php" autocomplete="on" method="post">
                                <h1>Change Password</h1> 
                                <p> 
                                    <label for="oldpassword" class="youpasswd" data-icon="p"> Old password </label>
                                    <input id="oldpassword" name="oldpassword" required="required" type="password" placeholder="eg. X8df90EO" /> 
                                </p>
                                <p> 
                                    <label for="newpassword" class="youpasswd" data-icon="p"> New password </label>
                                    <input id="newpassword" name="newpassword" required="required" type="password" placeholder="eg. X8df90EO" /> 
                                </p>
                                <p> 
                                    <label for="repassword" class="youpasswd" data-icon="p"> Please confirm your new password </label>
                                    <input id="repassword" name="repassword" required="required" type="password" placeholder="eg. X8df90EO" onfocus="validatePass(document.getElementById('newpassword'), this);" oninput="validatePass(document.getElementById('newpassword'), this);"/> 
                                </p>

                                <p class="reset button"> 
                                    <input type="submit" value="Update" /> 
								</p>

                                <p class="change_link">
								</p>
                            </form>

                        </div>



                        <div id="successpage" class="animate form">
                           	<h1> Password Updated! </h1>
                           	<p>
                           	<?php
                           		$email = $_SESSION['SESS_MEMBER_EMAIL'];
                           		
                           		echo "<label>Password for the account:<strong>".$email."</strong> has been successfully updated!</label>";
                           	?>
                           	</p>
                           	<p class="change_link">
						   	</p>
                        </div>

                        

                        <div id="errorpage" class="animate form">
							<h1 id="whoops"> Whoops! </h1>
							<p>
							<?php 
								$arr = $_SESSION['ERRMSG_ARR'];
								reset($arr);
								$msg = "";
								while (list(, $value) = each($arr)) {
									$msg = $msg . $value . "<br/>";
								}
								echo "<label>".$msg."</label>"; 
							?>	
							</p>

							<p class="change_link">
								Try to<a href="#toresetfromerrorpage" class="to_register">Change </a> again.
							</p>

                        </div>

						

                    </div>

                </div>  

            </section>

        </div>

    </body>

</html>


