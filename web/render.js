
function measure(node) {

}


function render(on, node) {
	var div = document.createElement("div");

	var frame = node.frame();
	div.style.left = frame.x + 'px'
	div.style.top = frame.y + 'px';
	div.style.width = frame.width + 'px';
	div.style.height = frame.height+ 'px';
	on.appendChild( div );


	if("fill" in node.atts) {
		div.style.background = node.atts["fill"];
	}

	if("text" in node.atts) {
		div.innerText = node.atts["text"];
	}

	node.subs.forEach( function(n) {
		render(div, n);
	});
}

