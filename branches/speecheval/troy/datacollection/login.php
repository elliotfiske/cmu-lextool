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
        <link rel="stylesheet" type="text/css" href="css/style.css" />
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
                    <a class="hiddenanchor" id="toregisterfromlogin"></a>
                    <a class="hiddenanchor" id="tologinfromregister"></a>
                    <a class="hiddenanchor" id="toerrorpagefromlogin"></a>
                    <a class="hiddenanchor" id="toerrorpagefromregister"></a>
                    <a class="hiddenanchor" id="toregisterfromerrorpage"></a>
                    <a class="hiddenanchor" id="tologinfromerrorpage"></a>

                    <div id="wrapper">
                        <div id="login" class="animate form">
                            <form  action="bg_userlogin.php" autocomplete="on" method="post">
                                <h1>Log in</h1> 
                                <p> 
                                    <label for="useremail" class="uname" data-icon="e" > Your email </label>
                                    <input id="useremail" name="useremail" required="required" type="email" placeholder="eg. mysupermail@mail.com"/>
                                </p>
                                <p> 
                                    <label for="password" class="youpasswd" data-icon="p"> Your password </label>
                                    <input id="password" name="password" required="required" type="password" placeholder="eg. X8df90EO" /> 
                                </p>

                                <p class="login button"> 
                                    <input type="submit" value="Login" /> 
								</p>

                                <p class="change_link">
									Not a member yet ?
									<a href="#toregisterfromlogin" class="to_register">Join us</a>
								</p>
                            </form>

                        </div>



                        <div id="register" class="animate form">
                            <!--<form  action="userregister.php" autocomplete="on" method="post"> -->
                            <form autocomplete="on" method="post">
                                <h1> Sign up </h1> 

                                <p> 
                                    <label for="emailsignup" class="youmail" data-icon="e" > Your email</label>
                                    <input id="emailsignup" name="emailsignup" required="required" type="email" placeholder="eg. mysupermail@mail.com"/> 
                                </p>

                                <p> 
                                    <label for="passwordsignup" class="youpasswd" data-icon="p">Your password </label>
                                    <input id="passwordsignup" name="passwordsignup" required="required" type="password" placeholder="At lease 8 alphanumeric characters" pattern="[a-zA-Z0-9]{8,}"/>
                                </p>

                                <p> 
                                    <label for="passwordsignup_confirm" class="youpasswd" data-icon="p">Please confirm your password </label>
                                    <input id="passwordsignup_confirm" name="passwordsignup_confirm" required="required" type="password" placeholder="At lease 8 alphanumeric characters" onfocus="validatePass(document.getElementById('passwordsignup'), this);" oninput="validatePass(document.getElementById('passwordsignup'), this);"/>
                                </p>

                                <p class="signin button"> 
									<input type="submit" formaction="bg_student_register.php" value="Student"/> 
									<input type="submit" formaction="bg_exemplary_register.php" value="Exemplary"/>
								</p>

                                <p class="change_link">  
									Already a member ?
									<a href="#tologinfromregister" class="to_register"> Go and log in </a>
								</p>

                            </form>

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
							<?php
								$type = $_SESSION['ERRMSG_TYPE'];
								if(strcmp($type, 'user_exists')==0) {
									echo 'Try to <a href="#tologinfromerrorpage" class="to_register"> Log In </a> &nbsp again ; or <a href="findpasswd.php" class="to_register"> Find Password</a>';
								} else {
									echo 'Try to <a href="#tologinfromerrorpage" class="to_register"> Log In </a> &nbsp; or <a href="#toregisterfromerrorpage" class="to_register"> Register now</a>';
								}
							?>
							</p>

                        </div>

						

                    </div>

                </div>  

            </section>

        </div>

    </body>

</html>


