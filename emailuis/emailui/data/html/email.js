// Original image sources are stored in this array
var originalImageSources = new Array();

// Number of images hidden (or replaced with our placeholder image)
var hiddenCount = 0;

function modifyEmail( frameId ) {
	hideImages(frameId);
	modifyFontSizes( frameId );
}

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
            if (image.src.length > 0) {
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

// removes all fixed font size definitions
function modifyFontSizes( frameId ) {
    var doc = document.getElementById(frameId).contentDocument;
    if ( doc ) {
        var nodes = doc.body.children;
        domodify( nodes );
    }
}

function domodify( nodes ) {
	for(var i=0;i < nodes.length;i++)
	{
	  var nodestyle = nodes[i].style;
	  for(var ii=0;ii<nodestyle.length;ii++)
	  {
	  	if(nodestyle[ii] == "font-size")
	  	{
	  		var fontsize = nodestyle.fontSize;
	  		if((fontsize == "8pt") || (fontsize == "9pt") || (fontsize == "10pt"))
	  		{
	  			nodestyle.fontSize = "xx-small";
	  	  	break;
	  	  }
	  		else if((fontsize == "11pt") || (fontsize == "12pt") || (fontsize == "13pt"))
	  		{
	  			nodestyle.fontSize = "x-small";
	  	  	break;
	  	  }
	  		else if((fontsize == "14pt") || (fontsize == "15pt") || (fontsize == "16pt") ||
	  		(fontsize == "17pt") || (fontsize == "18pt") || (fontsize == "19pt"))
	  		{
	  			nodestyle.fontSize = "medium";
	  	  	break;
	  	  }
	  	  else if(fontsize.match("pt"))
	  	  	{
	  	  		var cutpoint = fontsize.indexOf("p")
	  				var sizenumber = fontsize.slice(0,cutpoint);
	  				var fs = parseInt(sizenumber);
	  				if(fs < 8)
	  				{
	  					nodestyle.fontSize = "xx-small";
	  	  			break;
	  	  		}
	  	  		else if(fs > 20)
	  	  		{
	  	  			break;	  	  			
	  	  		}
	  	    }
	  	}
	  }
	  if(nodes[i].children.length) domodify(nodes[i].children);
	}
}

// Sets the body direction to ltr in mirrored layout if not already set to rtl
function setEmailBodyDirection() {
    var d = parent.frames['body_frame'].document;
    var a = d.getElementsByTagName('html');
    if (a.length > 0) {
        if (a[0].getAttribute('dir') != 'rtl') {
            a = document.getElementsByTagName('html');
            if (a.length > 0) {
                a[0].setAttribute('dir', 'ltr');
            }
        }
    }
}

// Appends body and its childs to left-aligned <p> element in mirrored layout
// if the document direction is not rtl
function setHeaderDirection() {
    var a = document.getElementsByTagName('html');
    if (a.length > 0) {
        if (a[0].getAttribute('dir') != 'rtl') {
            var hd = parent.frames['header_frame'].document;
            var p = hd.createElement('p');
            p.setAttribute('align', 'left');
            var body = hd.getElementsByTagName('body');
            if (body.length > 0) {
                while (body[0].childNodes.length > 0) {
                    p.appendChild(body[0].childNodes[0]);
                }
            body[0].appendChild(p);
            }
        }
    }
}

// Used onLoad when opening mail body frame in mirrored layout
function setDirections() {
    setEmailBodyDirection();
    setHeaderDirection();
}
