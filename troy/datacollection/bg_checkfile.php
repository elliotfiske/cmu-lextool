<?

   	$debug = false;	
	
	$dataroot='/home/li-bo/web/data/';
	//$dataroot='/Applications/MAMP/htdocs/rtmplite/';
	
	$flvname=$dataroot . $_POST["flvname"] . '.flv';
	
	if($debug) {
		print($flvname."<br>");
	}
	
	if(file_exists($flvname)) {
		print("yes");
	}else {
		print("no");
	}
 
?>

