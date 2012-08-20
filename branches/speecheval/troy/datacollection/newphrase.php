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
        <link rel="stylesheet" type="text/css" href="css/style.css" />
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
							<form  action="choosephonemes.php" method="post">
								<h1>Type in a New Phrase</h1> 
								<p> 
                                    <label for="newphrase" data-icon="e"> New Phrase </label>
                                    <input id="newphrase" name="newphrase" required="required" type="text" placeholder="eg. new phrase"/>
                                </p>
								<p class="login button"> 
                                    <input type="submit" value="Continue" /> 
								</p>
								<p class="change_link">
								</p>
							</form>
                        </div>
						
                    </div>
                </div>  
            </section>
        </div>
    </body>
</html>
