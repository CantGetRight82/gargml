#pragma once
#include <iostream>
#include <vector>
#include <map>
#include <math.h>

using namespace std;

class Node {
	public:
		static Node* zero;
		Node* parent;
		Node* prev;

		Node() : parent(NULL), prev(NULL) {
		}
		float left() {
			float left = attf("left");
			if(!isnan(left)) {
				if(!isFirstChild() && isFlowSide()) {
					return prev->right() + left;
				}
				return parentz()->left() + left;
			}
			return 0;
		}
		float top() {
			float top = attf("top");
			if(!isnan(top)) {
				if(isFirstChild()) {
					return parentz()->top() + top;
				}
				if(isFlowSide()) {
					return parentz()->top() + top;
				}
				return prev->down() + top;
				
			}
			return 0;
		}



		bool isFirstChild() {
			return prev == NULL;
		}

		bool isFlowSide() {
			return atts["flow"].compare("side") == 0;
		}


		float right() {
			float specr = attf("right");
			if(!isnan(specr)) {
				return parentz()->right() - specr;
			}

			float specw = attf("width");
			if(!isnan(specw)) { return left() + specw; }
			return left();
		}
		float down(bool allowParent = true) {

			float spech = attf("height");
			float specd = attf("down");
			if(!isnan(specd)) {
				if(allowParent) {
					return parentz()->down() - specd;
				} else {
					return top() + spech + specd;
				}
			}

			if(!isnan(spech)) { return top() + spech; }

			float max = top();
			for(Node* n : subs) {
				max = fmaxf( max, n->down(false) );

			}

			return max;
		}

		map<string,string> atts;
		vector<Node*> subs;

		void renderJSON(std::ostream& to);
		int atti(string key);
		float attf(string key);

		Node* parentz() {
			if(parent) { return parent; }
			if(!zero) {
				zero = new Node();
				zero->atts["top"] = "0";
				zero->atts["left"] = "0";
				zero->atts["down"] = "0";
				zero->atts["right"] = "0";
			}
			return zero;
		}

		Node* clone() {
			Node* result = new Node();
			result->atts = this->atts;

			vector<Node*>::iterator p;
			for(p = subs.begin(); p != subs.end(); p++) {
				result->subs.push_back( *p );
			}
			return result;
		}

		void pass1() {

		}


};
