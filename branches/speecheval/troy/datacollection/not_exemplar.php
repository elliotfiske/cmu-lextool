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
                <h1>Pronunciation Evaluation <span>Exemplar Analysis Page</span></h1>
            </header>
            <section>			
                <div id="container_demo" >
                    <div id="wrapper">
                        <div id="main" class="animate form">
							<form  action="" autocomplete="on" method="post"> 
								<h1>Exemplar Outlier Analysis</h1>
							
								<p class="prompt">
									<label>Whoops!</label>
									<br>
									<br>
									<label>The recording is not an exemplar! </label>
								</p>

								<p class="change_link">  
									The page will automatically go to <a href="exemplary_main.php" class="to_register"> Redo </a> the current phrase  
									in 5 seconds or you can ignore and <a href="bg_continue.php" class="to_register"> Continue </a> to next phrase.
								</p>
								<?php
									header("refresh: 5; exemplary_main.php");
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

