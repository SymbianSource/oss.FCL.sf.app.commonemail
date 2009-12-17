function collapseHeader()
	{
	handleHeaderDisplay( "header_table", "table_initial" )
	}
	
function expandHeader()
	{
	handleHeaderDisplay( "table_initial", "header_table" )
	}
	
function handleHeaderDisplay( tableToHide, tableToShow )
	{
	document.getElementById(tableToShow).style.display = "";
	document.getElementById(tableToHide).style.display = "none";
	}
	
onload=collapseHeader;