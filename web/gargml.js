
class ZeroNode {
	top() { return 0; }
	left() { return 0; }
	down() { return 0; }
	right() { return 0; }
}

class Node {
	constructor() {
		this.atts = {};
		this.subs = [];
		this.prev = null;
		this.next = null;
		this.parent = null;
	}


	lastSub() {
		return this.subs[ this.subs.length-1 ];
	}

	clone() {
		var result = new Node();
		var atts = this.atts;
		for(var key in atts) {
			result.atts[key] = atts[key];
		}

		var subs = this.subs;
		for(var i=0; i<subs.length; i++) {
			result.subs.push( subs[i].clone() );
		}
		return result;
	}

	frame() {
		return {
			x:this.left() - this.parent.left(),
			y:this.top() - this.parent.top(),
			width:this.right() - this.left(),
			height:this.down() - this.top()
		};
	}

	left() {
		var left = this.attf("left");
		if(!isNaN(left)) {
			if(!this.isFirstChild() && this.isFlowSide()) {
				return this.prev.right() + left;
			}
			return this.parent.left() + left;
		}
		return this.parent.left();
	}

	top() {
		var top = this.attf("top");
		var max = 0;
		if(!isNaN(top)) {
			if(this.isFirstChild() || this.isFlowSide()) {
				return this.parent.top() + top;
			}
			max = top;
		}
		else if(this.isFirstChild()) {
			return this.parent.top();
		}
		if("down" in this.prev.atts) {
			max = Math.max( max, this.prev.attf("down") );
		}
		return this.prev.down() + max;
	}

	isFirstChild() {
		return this.prev === null;
	}
	isLastChild() {
		return this.next === null;
	}

	isFlowSide() {
		return this.atts["flow"] === "side";
	}


	right() {
		var specr = this.attf("right");
		if(!isNaN(specr)) {
			return this.parent.right() - specr;
		}

		var specw = this.attf("width");
		if(!isNaN(specw)) { return this.left() + specw; }
		return this.parent.right();
	}

	down(preventParent) {
		var spech = this.attf("height");
		if(!isNaN(spech)) { return this.top() + spech; }
		var specd = this.attf("down");
		if(!isNaN(specd)) {
			if(!preventParent) {
				if(this.isFlowSide() || this.isLastChild()) {
					return this.parent.down() - specd;
				} else {
					return this.top() + spech;
				}
			} else {
				return this.top() + spech + specd;
			}
		}


		var max = this.top();
		this.subs.forEach( function(n) {
			max = Math.max( max, n.down(true) );
		});
		return max;
	}

	attf(key) {
		return parseFloat( this.atts[key] );
	}
};


class SymbolPos {
	constructor(symbol, pos) {
		this.symbol = symbol;
		this.pos = pos;
	}
}

function isspace(code) {
	return code === 0x20 || (code > 0x08 && code < 0x0e);
}

function trimBefore( buffer,  positions, i) {
	var end = positions[i].pos-1;
	var start = i>0 ? positions[i-1].pos+1 : 0;
	while( start < end && isspace(buffer.charCodeAt(start)) ) { start++; }
	while( end > start && isspace(buffer.charCodeAt(end)) ) { end--; }
	if(end>=start) {
		var size = end-start+1;
		return buffer.substr( start, size );
	}
	return "";

}

function tldrProp( code ) {
	switch(code) {
		case 't': return "top";
		case 'l': return "left";
		case 'd': return "down";
		case 'r': return "right";
	}
	return "notldr";
}


function tldr(node, key, val) {
	var size = key.length;
	if(size>4) { return false; }
	for(var i=0; i<size; i++) {
		if("tldr".indexOf(key.charAt(i)) === -1) {
			return false;
		}
	}

	for(var i=0; i<size; i++) {
		var prop = tldrProp( key[i] );
		node.atts[ prop ] = val;
	}
	return true;
}

function gargml(buffer, measure) {
	var root = parse(buffer);
	measureTexts( root, measure );
	return root;
}

function measureTexts(node, measure) {
	if("text" in node.atts) {
		var size = measure( node );
		node.atts["height"] = size.height;

	}

	node.subs.forEach( function(n) {
		measureTexts(n,measure);;
	});
}

function parse(buffer) {
	var size = buffer.length;

	var positions = [];
	var delimeters = "{}:;`";
	var inString = false;
	var level = 0;
	for(var i=0; i<size; i++) {
		if(delimeters.indexOf(buffer.charAt(i)) !== -1) {
			var c = buffer.charAt(i);
			positions.push( new SymbolPos( c, i ) );
			if(c == '`') { inString = !inString; }
			else if(c == '{' && !inString) { level++; }
			else if(c == '}' && !inString) { level--; }
		}
	}

	if(level != 0) {
		throw "{ and } characters do not match up";
	}



	var stack = [];
	var root = null;
	var top = null;
	var key;
	for(var i=0; i<positions.length; i++) {
		var symbol = positions[i].symbol;
		var str = trimBefore(buffer, positions, i);
		if(symbol == '{') {
			var node;
			var clone = null;
			if(str.indexOf('<') !== -1) {
				clone = top.lastSub();
			}

			if(clone) {
				node = clone.clone();
			} else {
				node = new Node();
			}

			node.parent = top;
			if(top) {
				if(top.subs.length) {
					top.lastSub().next = node;
					node.prev = top.lastSub();
				}
				top.subs.push( node );
			}
			stack.push( node );
			top = node;
			if(i==0) {
				root = node;
				root.parent = new ZeroNode();
			}
		} else if(symbol == '}') {
			if(stack.length) {
				stack.pop();
				top = stack[ stack.length-1 ];
			}
		} else if(symbol == ':') {
			key = str;
		} else if(symbol == ';') {
			if(key.length && str.length) {
				if(tldr(top, key, str)) {
				} else if(key == "square") {
					top.atts[ "width" ] = str;
					top.atts[ "height" ] = str;
				} else {
					top.atts[ key ] = str;
				}
			}
		}
	}
	return root;
}
