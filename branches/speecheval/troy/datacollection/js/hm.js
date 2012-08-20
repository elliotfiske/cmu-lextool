
var debug = false;

window.onload = function() 
{
	if(debug) alert("pageInit...");
	selectchange();
	
}

function selectchange() 
{
	if(debug) alert("selection changed");
	var lst = document.getElementById("datalist");
	
	document.getElementById("lbldata").innerHTML = lst.options[lst.selectedIndex].innerHTML;
}




