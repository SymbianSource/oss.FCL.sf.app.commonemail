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

var intervalHandler;
function fetchMoreIfAtBottom()
   {
   var element = parent.body_frame.document.body;

   if ( element == null )
      {
      return;
      }
    
    var viewport = parent.pageYOffset;
    var elementHeight = element.offsetHeight;
    var elementOffsetTop = element.offsetTop;
    var screenHeight = screen.availHeight;
    if ( elementOffsetTop + elementHeight - viewport < screenHeight )
       {            
       location.href = "cmail://body/" + viewport
       clearInterval(intervalHandler);
       }
    }
	   
function scrollHandler()
	{	
	  intervalHandler = setInterval( "fetchMoreIfAtBottom()", 500);
	} 
				
function init(scrollPos)
    {
    collapseHeader();
    window.scrollTo(0, scrollPos);
    //start a scrolling event which will trigger the check of bottom reached...
    //this will take care of short document not does not need scrolling
    scrollHandler();
    } 

//onload=init;