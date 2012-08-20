<?php
	$phraseid=1;
	$audiofile="../recordings/audioRecorder/user2/phrase1.flv";
	$alnfile=str_replace(".flv", ".phonesegdir/phrase".$phraseid.".lab", $audiofile);
	print($alnfile."<br/>");
	$fp=fopen($alnfile, "r") or exit("Unable to open file!");
	$i=0;
	while(!feof($fp)){
		$line=fgets($fp);
		echo $line."<br/>";
		$n=sscanf($line, "%f %d %s", $t, $x, $ph);
		//print($n." ".$t." ".$x." ".$ph."<br/>");
		if($n==3){
			$tim[$i]=$t;
			$phns[$i]=$ph;
			$i=$i+1;
		}
	}
	fclose($fp);
	for($i=0;$i<count($tim);$i++){
		if(strcmp($phns[$i], "SIL")!=0){
			$s=substr($phns[$i],0,1);
			if(strcmp($s, '+')!=0) {
				echo $phns[$i]." ".$tim[$i]."~".$tim[$i+1]."<br/>";
			}
		}
	}
?> 
			
		
