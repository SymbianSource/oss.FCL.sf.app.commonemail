var collapsed = is_header_expanded_visible;
var displayImagesHidden = false;
var previousPageYOffset = -1;
var setScrollPositionInterval;

function collapseHeader(sendEvent) {
	if (sendEvent) {
    	location.href = "cmail://collapseHeader/";
    	}
    collapsed = true;
	handleHeaderDisplay( "header_expanded", "header_collapsed" );
	showTable( "header_collapsed_2" );
	hideTable( "to_table" );
	hideTable( "cc_table" );
	hideTable( "bcc_table" );
	hideTable( "header_expanded_2" );
	handleHeaderDisplay( "to_expanded", "to_collapsed" );
	handleHeaderDisplay( "cc_expanded", "cc_collapsed" );
	handleHeaderDisplay( "bcc_expanded", "bcc_collapsed" );
	if (sendEvent) {
	    handleHeaderDisplay( "attachments_expanded", "attachments_collapsed" );
        }
	updateHeader();
    }

function initHeader() {

    if (typeof(is_header_expanded_visible) !== "undefined" && !is_header_expanded_visible) {
    	handleHeaderDisplay( "header_expanded", "header_collapsed" );
	}

    if (typeof(is_header_collapsed_visible) !== "undefined" && !is_header_collapsed_visible) {
	    hideTable( "header_collapsed" );
	}

	if (typeof(is_header_collapsed_2_visible) !== "undefined" && !is_header_collapsed_2_visible) {
	    hideTable( "header_collapsed_2" );
	}

	if (typeof(is_header_expanded_2_visible) !== "undefined" && !is_header_expanded_2_visible) {
	    hideTable( "header_expanded_2" );
	}

    if (typeof(is_to_table_visible) !== "undefined") {
        if (!is_to_table_visible) {
    	    hideTable( "to_table" );
        	handleHeaderDisplay( "to_expanded", "to_collapsed" );
    	} else {
    	    if (typeof(is_to_expanded_visible) !== "undefined" ) {
    	        if (is_to_expanded_visible) {
                	handleHeaderDisplay( "to_collapsed", "to_expanded" );
    	        } else {
                	handleHeaderDisplay( "to_expanded", "to_collapsed" );
    	        }
    	    } else {
                handleHeaderDisplay( "to_expanded", "to_collapsed" );
    	    }
    	}
    }

    if (typeof(is_cc_table_visible) !== "undefined") {
    	if (!is_cc_table_visible) {
    	    hideTable( "cc_table" );
    	    handleHeaderDisplay( "cc_expanded", "cc_collapsed" );
    	} else {
    	    if (typeof(is_cc_expanded_visible) !== "undefined" ) {
    	        if (is_cc_expanded_visible) {
                	handleHeaderDisplay( "cc_collapsed", "cc_expanded" );
    	        } else {
                	handleHeaderDisplay( "cc_expanded", "cc_collapsed" );
    	        }
    	    } else {
                handleHeaderDisplay( "cc_expanded", "cc_collapsed" );
    	    }
    	}
    }


    if (typeof(is_bcc_table_visible) !== "undefined") {
    	if (!is_bcc_table_visible) {
	        hideTable( "bcc_table" );
    	    handleHeaderDisplay( "bcc_expanded", "bcc_collapsed" );
	    } else {
    	    if (typeof(is_bcc_expanded_visible) !== "undefined" ) {
    	        if (is_bcc_expanded_visible) {
                	handleHeaderDisplay( "bcc_collapsed", "bcc_expanded" );
    	        } else {
                	handleHeaderDisplay( "bcc_expanded", "bcc_collapsed" );
    	        }
    	    } else {
                handleHeaderDisplay( "bcc_expanded", "bcc_collapsed" );
    	    }
    	}
	}

    if (typeof(is_header_expanded_2_visible) !== "undefined" && !is_header_expanded_2_visible) {
        hideTable( "attachments_expanded" );
    }

    if (typeof(is_attachments_expanded_visible) !== "undefined" && is_attachments_expanded_visible) {
        handleHeaderDisplay( "attachments_collapsed" , "attachments_expanded" );
    } else {
        handleHeaderDisplay( "attachments_expanded", "attachments_collapsed" );
    }
}

function expandHeader(sendEvent) {
	if (sendEvent) {
	    location.href = "cmail://expandHeader/";
    	}
	collapsed = false;
	handleHeaderDisplay( "header_collapsed", "header_expanded" )
	hideTable( "header_collapsed_2" );
	showTable( "to_table" );
	showTable( "cc_table" );
	showTable( "bcc_table" );
	showTable( "header_expanded_2" );
	parent.document.getElementById('email_frameSet').rows = "30%, *";
	}

function updateHeader() {
    var rows = "30%, *";
	if (collapsed) {
    	if (displayImagesHidden || (parent.header_frame.g_autoLoadImages != 0) || (parent.hiddenCount == 0)) {
            rows = "10%, *";
        } else {
            rows = "19%, *";
	    }
	}
    parent.document.getElementById('email_frameSet').rows = rows;
    }

function showTable( tableToShow ) {
    if (document.getElementById(tableToShow) != null) {
	    document.getElementById(tableToShow).style.display = "";
	}
}

function hideTable( tableToHide ) {
    if (document.getElementById(tableToHide) != null) {
	    document.getElementById(tableToHide).style.display = "none";
    }
}

function handleHeaderDisplay( tableToHide, tableToShow ) {
    showTable(tableToShow);
    if (document.getElementById(tableToShow) != null) {
        hideTable(tableToHide);
    } else {
        showTable(tableToHide);
    }
}

function init(scrollPos) {
	/*if (document.getElementById("header_collapsed").style.display != "none") {
        collapseHeader(false);
	} else {
	    expandHeader(false);
	}*/

	initHeader();
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

