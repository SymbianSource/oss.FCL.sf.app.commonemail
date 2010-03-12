var collapsed = true;
var displayImagesHidden = false;
var previousPageYOffset = -1;
var setScrollPositionInterval;

function collapseHeader(sendEvent) {
	if (sendEvent) {
    	location.href = "cmail://collapseHeader/";
    	}
    collapsed = true;
	handleHeaderDisplay( "header_table", "table_initial" );
	updateHeader();
	}

function expandHeader(sendEvent) {
	if (sendEvent) {
	    location.href = "cmail://expandHeader/";
    	}
	collapsed = false;
	handleHeaderDisplay( "table_initial", "header_table" )
	parent.document.getElementById('email_frameSet').rows = "40%, *";
	}

function updateHeader() {
    var rows = "40%, *";
	if (collapsed) {
    	if (displayImagesHidden || (parent.header_frame.g_autoLoadImages != 0) || (parent.hiddenCount == 0)) {
            rows = "10%, *";
        } else {
            rows = "17%, *";
	    }
	}
    parent.document.getElementById('email_frameSet').rows = rows;
    }

function handleHeaderDisplay( tableToHide, tableToShow ) {
	document.getElementById(tableToShow).style.display = "";
	document.getElementById(tableToHide).style.display = "none";
	}

function init(scrollPos) {
	if (document.getElementById("table_initial").style.display != "none") {
    	    collapseHeader(false);
	} else {
	    expandHeader(false);
	}
    window.scrollTo(0, scrollPos);
	setScrollPositionInterval = setInterval("updateScrollPosition()", 500);
    }

function displayImagesButtonPressed() {
    hideDisplayImagesButton();
    parent.restoreImages("body_frame");
    requestDisplayImages();
}

function hideDisplayImagesButton() {
	document.getElementById("displayImagesTable").style.display = "none";
    displayImagesHidden = true;
    updateHeader();
}

function showDisplayImagesButton() {
	document.getElementById("displayImagesTable").style.display = "";
    displayImagesHidden = false;
    updateHeader();
}

function requestDisplayImages() {
    location.href = "cmail://displayImages/";
}

function updateScrollPosition() {
    if (previousPageYOffset != parent.pageYOffset) {
        previousPageYOffset = parent.pageYOffset;
        location.href = "cmail://body/" + previousPageYOffset;
    }
}

