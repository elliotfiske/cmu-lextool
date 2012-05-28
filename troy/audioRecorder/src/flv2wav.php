<?
   define('FFMPEG_LIBRARY', '/usr/bin/ffmpeg ');
   
   $flvroot='../data/';
   $wavroot='/tmp/data/';
   
   //$fname='../data/audioRecorder/user2/Sentence_1_q8';
   $flvname=$flvroot . $_POST["flvname"] . '.flv';
   $tmpname=str_replace('/', '-', $_POST["flvname"]);
   $wavname=$wavroot . $tmpname . '.wav';
   
   $exec_string = FFMPEG_LIBRARY.' -y -i '.$flvname.' -vn -f wav -ar 16000 -ac 1 '.$wavname.' 2>&1';
   print($exec_string.'\n'); 
   exec($exec_string, $output); //where exec is the command used to execute shell command in php
   print_r($output);
?>

