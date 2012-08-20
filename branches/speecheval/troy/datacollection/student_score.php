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
        <link rel="stylesheet" type="text/css" href="css/style_mainpage.css" />
		<link rel="stylesheet" type="text/css" href="css/animate-custom.css" />
		<link href='http://fonts.googleapis.com/css?family=Chewy' rel='stylesheet' type='text/css'>
		<script src="js/main.js" type="text/javascript"></script>
		<script src="js/asjs.js" type="text/javascript"></script>
    </head>
    <body>
        <div class="container">
            <header>
                <h1>Pronunciation Evaluation <span>Student Score Page</span></h1>
            </header>
            <section>			
                <div id="container_demo" >
                    <div id="wrapper">
                        <div id="main" class="animate form">
							<form  action="" autocomplete="on" method="post"> 
								<h1>Evaluation Score</h1>
							
								<p class="prompt">
									<?php
										
										if(isset($_SESSION['ERRMSG_ARR']) && !empty($_SESSION['ERRMSG_ARR'])){
											echo "<label> Improper recording! </label>";
											print_r($_SESSION['ERRMSG_ARR']);
										}else{
											$ac=$_SESSION['SESS_STUDENT_AC_SCORE'];
											$dur=$_SESSION['SESS_STUDENT_DUR_SCORE'];
											
											if($ac>4.0 && $dur>4.0){
												echo "<label>Excellent!</label>";
											}else if($ac>3.5 || $dur>3.5){
												echo "<label>Good!</label>";
											}else{
												echo "<label>Practice more!</label>";
											}
											echo "<br><br>";
											echo "<label>You have achieved ".$ac." for acoustics and ".$dur." for duration.</label>";
										}
									?>
								</p>

								<p class="change_link"> 
									The page will automatically navigate to <a href="student_main.php" class="to_register"> Next </a> phrase in 5 seconds. Or you can also <a href="bg_goback.php" class="to_register">Redo</a> this one.
								</p>
								<?php
									header("refresh: 5; student_main.php");
								?>
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




