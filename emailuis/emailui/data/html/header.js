var collapsed = true;
var displayImagesHidden = false;
var previousPageYOffset = -1;
var setScrollPositionInterval;

function collapseHeader(sendEvent) {
    if (sendEvent) {
        location.href = "cmail://collapseHeader/";
        }
    collapsed = true;
    handleHeaderDisplay('expanded_header', 'collapsed_header');
    updateHeader();
    }

function expandHeader(sendEvent) {
    if (sendEvent) {
        location.href = "cmail://expandHeader/";
        }
    collapsed = false;
    handleHeaderDisplay('collapsed_header', 'expanded_header');
    updateHeader();
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
    document.getElementById(tableToHide).style.display = 'none';
    document.getElementById(tableToShow).style.display = '';
    }

function init(scrollPos) {
    if (document.getElementById('collapsed_header').style.display != 'none') {
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

