function collapseHeader()
	{
	handleHeaderDisplay( "header_table", "table_initial" )
	parent.document.getElementById('headerBody').rows = '10%,*';
	}
	
function expandHeader()
	{
	handleHeaderDisplay( "table_initial", "header_table" )
	parent.document.getElementById('headerBody').rows = '40%,*';
	}
	
function handleHeaderDisplay( tableToHide, tableToShow )
	{
	document.getElementById(tableToShow).style.display = "";
	document.getElementById(tableToHide).style.display = "none";
	}
	
onload=collapseHeader;