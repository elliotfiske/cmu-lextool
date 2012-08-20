var jsReady = false;
var swfReady = false;
var recorderReady = false;

var debug = false;


// Gets a reference to the specified SWF file by checking which browser is
// being used and using the appropriate JavaScript.
// Unfortunately, newer approaches such as using getElementByID() don't
// work well with Flash Player/ExternalInterface.
function getSWF(movieName) {
	if (navigator.appName.indexOf("Microsoft") != -1) {
		return window[movieName];
	} else {
		return document[movieName];
	}
}

window.onload = function() 
{
	if(debug) alert("pageInit...");
	// record that JS is ready to go.
	jsReady = true;
	selectchange();
	
}

function selectchange() 
{
	if(debug) alert("selection changed");
	var lst = document.getElementById("datalist");
	
	if(lst.selectedIndex<0){
		lst.selectedIndex=0;
	}
	
	if(lst.selectedIndex==0) {
		document.getElementById("btnPrevious").disabled=true;
	}else {
		document.getElementById("btnPrevious").disabled=false;
	}
	
	if(lst.selectedIndex > lst.options.length-1) {
		lst.selectedIndex=lst.options.length-1;
	}
	
	if(lst.selectedIndex == lst.options.length-1){
		document.getElementById("btnNext").disabled=true;
	}else {
		document.getElementById("btnNext").disabled=false;
	}
		
	document.getElementById("lblprompt").innerHTML = lst.options[lst.selectedIndex].innerHTML;
	document.getElementById("lblprompt_record").innerHTML = lst.options[lst.selectedIndex].innerHTML;
	document.getElementById("phraseid").value = lst.options[lst.selectedIndex].value;
	if(swfReady) {
		var userid = document.getElementById("userid").innerHTML;
		//var d = new Date();
		var fname = "audioRecorder/user"+userid+"/phrase"+lst.options[lst.selectedIndex].value; //+"_"+d.getTime();
		if(debug) alert(fname);
		getSWF("audioRecorder").setFileName(fname);
		getSWF("audioRecorder").setRecordingExistance(false); // the recorder will check against the server to make necessary changes
	}
	
	// change the exemplars section contents
	document.getElementById("exemplars_section").innerHTML = "<strong> Maximum 5 exemplars for the phrase "+lst.selectedIndex+" from the database are listed below for your reference.</strong>"
	
	document.getElementById("exemplars_phrase"+lst.selectedIndex).hidden=false;
	if(lst.selectedIndex-1>=0){
		document.getElementById("exemplars_phrase"+(lst.selectedIndex-1)).hidden=true;
	}
	if(lst.selectedIndex+1<=lst.options.length){
		document.getElementById("exemplars_phrase"+(lst.selectedIndex+1)).hidden=true;
	}
	//getSWF("audioRecorder").setEnabled(true);

}

function setSWFisReady()
{
	swfReady = true;
	getSWF("audioRecorder").showDebug(debug);
	if(debug) alert("swf ready!");
	selectchange();
}

// indicate the recorder RMTP connection is successfully set up
function setRecorderReady(value)
{
	recorderReady = value;
}

function startRecording()
{
	document.getElementById('datalist').disabled=true;
	document.getElementById("btnPrevious").disabled=true;
	document.getElementById("btnNext").disabled=true;
	// create new file name for the new recording
	var userid = document.getElementById("userid").innerHTML;
	//var d = new Date();
	var lst = document.getElementById("datalist");
	var fname = "audioRecorder/user"+userid+"/phrase"+lst.options[lst.selectedIndex].value; //+"_"+d.getTime();
	getSWF("audioRecorder").setFileName(fname);
}

function finishRecording()
{
	document.getElementById('datalist').disabled=true;
	document.getElementById("btnPrevious").disabled=true;
	document.getElementById("btnNext").disabled=true;
	getSWF("audioRecorder").setEnabled(false);
	
	//selectchange();
	//getSWF("audioRecorder").setRecordingExistance(true);
	var lst = document.getElementById("datalist");
	window.location.href=('bg_student_scoring.php?phraseid='+lst.options[lst.selectedIndex].value);
}

function startPlaying()
{
	document.getElementById('datalist').disabled=true;
	document.getElementById("btnPrevious").disabled=true;
	document.getElementById("btnNext").disabled=true;
}

function finishPlaying()
{
	document.getElementById('datalist').disabled=false;
	//document.getElementById("datalist").selectedIndex += 1;
	selectchange();
}

// functioned called by Actionscript
// called to check if the page has intialized and JS is available
function isReady()
{
	return jsReady;
}

function nextSentence()
{
	document.getElementById("datalist").selectedIndex += 1;
	if(debug) alert(document.getElementById("datalist").selectedIndex);
	selectchange();
}

function preSentence()
{
	document.getElementById("datalist").selectedIndex -= 1;
	if(debug) alert(document.getElementById("datalist").selectedIndex);
	selectchange();
}


