<?php
	session_start();
	
	if(!isset($_SESSION['SESS_MEMBER_EMAIL'])|| $_SESSION['SESS_MEMBER_TYPE'][1]!='1') {
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
		<script type="text/javascript" src="js/swfobject.js"></script>
		<script type="text/javascript">
            // For version detection, set to min. required Flash Player version, or 0 (or 0.0.0), for no version detection. 
            var swfVersionStr = "11.1.0";
            // To use express install, set to playerProductInstall.swf, otherwise the empty string. 
            var xiSwfUrlStr = "playerProductInstall.swf";
            var flashvars = {};
            var params = {};
            params.quality = "high";
            params.bgcolor = "#F7F7F7";
            params.wmode = "window";
            params.allowscriptaccess = "always";
            params.allowfullscreen = "true";
            var attributes = {};
            attributes.id = "audioRecorder";
            attributes.name = "audioRecorder";
            attributes.align = "middle";
            swfobject.embedSWF(
                "audioRecorder.swf", "flashContent", 
                "250", "180", 
                swfVersionStr, xiSwfUrlStr, 
                flashvars, params, attributes);
            // JavaScript enabled so display the flashContent div in case it is not replaced with a swf object.
            swfobject.createCSS("#flashContent", "display:block;text-align:left;");
        </script>
    </head>
    <body>
        <div class="container">
            <header>
                <h1>Pronunciation Evaluation <span>Exemplary Page</span></h1>
                <nav class="codrops-demos">
					<span>Current User: 
						<strong>
							<?php
								echo $_SESSION['SESS_MEMBER_EMAIL'];
							?>
						</strong>
					</span>
					<label id="userid" hidden><?php echo $_SESSION['SESS_MEMBER_ID']; ?></label>
					<a href="exemplary_main.php"  class="current-demo">Home</a>
					<a href="changepasswd.php">Change Password</a>
					<a href="bg_prepare_extrainfo.php">Update Info</a>
					<a href="logout.php">Log Out</a>
				</nav>
            </header>
            <section>			
                <div id="container_demo" >
					<a class="hiddenanchor" id="touploadfrommain"></a>
                    <a class="hiddenanchor" id="tomainfromupload"></a>
                    <div id="wrapper">
                        <div id="main" class="animate form">
							<form  action="" autocomplete="on" method="post"> 
								<h1>Recording</h1>
								<p class="itemlist">
									<label for="datalist">Select an item to start:</label>
									<select id="datalist" onchange="selectchange();">
										<?php
											include "bg_prepare_phrases.php";
										?>
									</select>
								</p>
								<p class="prompt">
									<label id="lblprompt"></label>
								</p>
								<p>
									<div id="flashContent">
										<p>
											To view this page ensure that Adobe Flash Player version 
											11.1.0 or greater is installed. 
										</p>
										<script type="text/javascript"> 
											var pageHost = ((document.location.protocol == "https:") ? "https://" : "http://"); 
											document.write("<a href='http://www.adobe.com/go/getflashplayer'><img src='" 
															+ pageHost + "www.adobe.com/images/shared/download_buttons/get_flash_player.gif' alt='Get Adobe Flash player' /></a>" ); 
										</script> 
									</div>
									<noscript>
										<object classid="clsid:D27CDB6E-AE6D-11cf-96B8-444553540000" width="160" height="164" id="audioRecorder">
											<param name="movie" value="audioRecorder.swf" />
											<param name="quality" value="high" />
											<param name="bgcolor" value="#F7F7F7" />
											<param name="wmode" value="window" />
											<param name="allowScriptAccess" value="always" />
											<param name="allowFullScreen" value="true" />
											<!--[if !IE]>-->
											<object type="application/x-shockwave-flash" data="audioRecorder.swf" width="160" height="164">
												<param name="quality" value="high" />
												<param name="bgcolor" value="#F7F7F7" />
												<param name="wmode" value="window" />
												<param name="allowScriptAccess" value="always" />
												<param name="allowFullScreen" value="true" />
											<!--<![endif]-->
											<!--[if gte IE 6]>-->
												<p> 
													Either scripts and active content are not permitted to run or Adobe Flash Player version
													11.1.0 or greater is not installed.
												</p>
											<!--<![endif]-->
												<a href="http://www.adobe.com/go/getflashplayer">
													<img src="http://www.adobe.com/images/shared/download_buttons/get_flash_player.gif" alt="Get Adobe Flash Player" />
												</a>
											<!--[if !IE]>-->
											</object>
											<!--<![endif]-->
										</object>
									</noscript>     
								</p>
								<p class="upload button">
									<input type="button" id="btnPrevious" value="Previous" onclick="preSentence()">
									<input type="button" id="btnNext" value="Next" onclick="nextSentence()">
								</p>
								<p class="change_link">
									Alternatively, your can <a href="#touploadfrommain" class="to_upload"> Upload </a> a recording file.
								</p>
							</form>
                        </div>
						
						<div id="upload" class="animate form">
                            <form  action="bg_uploadfile_exemplary.php" method="post" enctype="multipart/form-data">
                                <h1>Upload the Recording</h1> 
                                <p>
									<input id="phraseid" name="phraseid" type="text" hidden></input>
                                </p>
                                <p class="prompt">
									<label id="lblprompt_record"></label>
								</p>
                                <p> 
                                    <label for="userfile" > File Name: </label>
                                    <input id="userfile" name="userfile" required="required" type="file" placeholder="eg. recording.wav"/>
                                </p>

                                <p class="login button"> 
                                    <input type="submit" value="Upload" /> 
								</p>

                                <p class="change_link">
									<a href="#tomainfromupload" class="to_record">Go Back</a>
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

