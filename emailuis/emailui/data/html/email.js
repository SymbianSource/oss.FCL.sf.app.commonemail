// Original image sources are stored in this array
var originalImageSources = new Array();

// Cid identifier string, if the image name begins with cid: it's shown
var cid = new String("cid:");

// Number of images hidden (or replaced with our placeholder image)
var hiddenCount = 0;

// Replace image sources with our placeholder (if the autoloading is disabled)
function hideImages(frameId) {
    if (parent.header_frame.g_autoLoadImages == 0) {
        var doc = document.getElementById(frameId).contentDocument;
        if (!doc) {
            doc = document.frames[frameId].document;
        }
        for (i = 0; i < doc.images.length; i++) {
        	var image = doc.images[i];
            originalImageSources.push(image.src);
            if (image.src.length > 0 && image.src.indexOf(cid) == -1) {
                doc.images[i].src = "../hidden.png";
                hiddenCount++;
            }
        }
        if (hiddenCount == 0) {
            parent.header_frame.hideDisplayImagesButton();
        } else {
            parent.header_frame.showDisplayImagesButton();
        }
    } else {
        parent.header_frame.updateHeader();
    }
    requestLoadImages();
}

// Restore original images sources
function restoreImages(frameId) {
    if (parent.header_frame.g_autoLoadImages == 0) {
        var doc = document.getElementById(frameId).contentDocument;
        if (!doc) {
            doc = document.frames[frameId].document;
        }
        for (i = 0; i < originalImageSources.length; i++) {
            doc.images[i].src = originalImageSources[i];
        }
    }
}

// Causes application to reload images
function requestLoadImages() {
    location.href = "cmail://loadImages/";
}
