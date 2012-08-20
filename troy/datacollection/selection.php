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
							<h1>Select a Mode</h1> 
							<form method="post">
								<p class="login button">
									For student, please go with <input type="submit" formaction="student_main.php" value="Practicing"/>
								</p>
								<p class="login button">
									For examplar recodring, please go with <input type="submit" formaction="exemplary_main.php" value="Recording"/>
								</p>
								<p class="login button">
									For phrase building, please go with <input type="submit" formaction="newphrase.php" value="Phrase Building"/>
								</p>
								<p class="login button">
									For evaluating students, please go with <input type="submit" formaction="hm_s1.php" value="Judge"/>
								</p>
								<p class="change_link">
									<a href="logout.php" class="to_register">Log Out</a>
								</p>
							</form>
                        </div>
						
                    </div>
                </div>  
            </section>
        </div>
    </body>
</html>
