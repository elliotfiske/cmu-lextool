<?php 
	session_start();
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
            </header>

            <section>				
                <div id="container_demo" >
                    <a class="hiddenanchor" id="tosuccesspagefromreset"></a>
                    <a class="hiddenanchor" id="toerrorpagefromreset"></a>
                    <a class="hiddenanchor" id="toresetfromerrorpage"></a>

                    <div id="wrapper">
                        <div id="reset" class="animate form">
                            <form  action="bg_reset_passwd.php" autocomplete="on" method="post">
                                <h1>Reset Password</h1> 
                                <p> 
                                    <label for="useremail" class="uname" data-icon="u" > Your email </label>
                                    <input id="useremail" name="useremail" required="required" type="email" placeholder="eg. mysupermail@mail.com"/>
                                </p>

                                <p class="reset button"> 
                                    <input type="submit" value="Reset" /> 
								</p>

                                <p class="change_link">
								</p>
                            </form>

                        </div>



                        <div id="successpage" class="animate form">
                           	<h1> Password Reset! </h1>
                           	<p>
                           	<?php
                           		$email = $_SESSION['SESS_MEMBER_EMAIL'];
                           		
                           		echo "<label>New password has been sent to the Email:<strong>".$email."</strong>.<br>Please change the password as soon as possible!</label>";
                           	?>
                           	</p>
                           	<p class="change_link">
                           		Go to <a href="login.php" class="to_register"> Log In </a> page.
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
								Try to<a href="#toresetfromerrorpage" class="to_register">Reset </a> again or<a href="login.php#toregisterfromerrorpage" class="to_register">Register</a> with us.
							</p>

                        </div>

						

                    </div>

                </div>  

            </section>

        </div>

    </body>

</html>


