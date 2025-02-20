/********
 * Michael Pham-Tran
 * mqphamtr
 * 2023 Fall CSE101 PA7
 ********/


//--------------------------------------------------------
// Dictionary.cpp
// implementing the functions of Dictionary.h
//--------------------------------------------------------

#include<iostream>
#include<string>
#include<stdexcept>
#include"Dictionary.h"

//Private Constructor------------------------------------------------------

//Node constructor
Dictionary::Node::Node(keyType k, valType v){
	key=k;
	val=v;
	parent=nullptr;
	left=nullptr;
	right=nullptr;
}

//Helper functions----------------------------------------------------------

// inOrderString()
// Appends a string representation of the tree rooted at R to string s. The
// string appended consists of: "key : value \n" for each key-value pair in
//tree R, arranged in order by keys.
void Dictionary::inOrderString(std::string& s, Node* R) const{
	if (R!=nil){
		inOrderString(s, R->left);
		s+= R->key;
		s+=" : ";
		s+= std::to_string(R->val);
		s+="\n";
		inOrderString(s, R->right);		
	}	
}

// preOrderString()
// Appends a string representation of the tree rooted at R to s. The appended
// string consists of keys only, separated by "\n", with the order determined
// by a pre-order tree walk.
void Dictionary::preOrderString(std::string& s, Node* R) const{
	if (R!=nil){
                s+=R->key;
		s+="\n";
                preOrderString(s, R->left);
		preOrderString(s, R->right);
        }
}

// preOrderCopy()
// Recursively inserts a deep copy of the subtree rooted at R into this 
// Dictionary. Recursion terminates at N.
void Dictionary::preOrderCopy(Node* R, Node* N){
	if (R!= N){
		setValue(R->key, R->val);
		preOrderCopy(R->left, N);
		preOrderCopy(R->right, N);
	}	
}

// postOrderDelete()
// Deletes all Nodes in the subtree rooted at R.
void Dictionary::postOrderDelete(Node* R){
	if (R!=nil){
		postOrderDelete(R->left);
		postOrderDelete(R->right);
		delete(R);
	}
}

// search()
// Searches the subtree rooted at R for a Node with key==k. Returns
// the address of the Node if it exists, returns nil otherwise.
Dictionary::Node* Dictionary::search(Node* R, keyType k) const{
	//std::cout << "(search) node R key = " << R->key << std::endl;
	if (R->key==k || R==nil){
		return R;
	}
	
	else if (k<R->key){
		return search(R->left, k);
	}

	else{
                return search(R->right, k);
        }

}

// findMin()
// If the subtree rooted at R is not empty, returns a pointer to the 
// leftmost Node in that subtree, otherwise returns nil.
Dictionary::Node* Dictionary::findMin(Node* R){
	Node* cur=R;
	if (cur!=nil){
		while (cur->left!=nil){
			cur=cur->left;	
		}
	}
	return cur;
}

// findMax()
// If the subtree rooted at R is not empty, returns a pointer to the 
// rightmost Node in that subtree, otherwise returns nil.
Dictionary::Node* Dictionary::findMax(Node* R){
        Node* cur=R;
        if (cur!=nil){
		while (cur->right!=nil){
                	cur=cur->right;
        	}
	}
        return cur;
}

// findNext()
// If N does not point to the rightmost Node, returns a pointer to the
// Node after N in an in-order tree walk.  If N points to the rightmost 
// Node, or is nil, returns nil. 
Dictionary::Node* Dictionary::findNext(Node* N){
	Node* cur= N;
        Node* p = N->parent;
	if (N->right!=nil){
		//std::cout<<"(findNExt) return right" <<std::endl;
		return findMin(N->right);
	}

	while(p!=nil && p->right== cur){
		//std::cout<<"(findNExt) return parent" <<std::endl;
		//return N->parent;
		cur=p;
		p=p->parent;
	}
	
	return p;
}

// findPrev()
// If N does not point to the leftmost Node, returns a pointer to the
// Node before N in an in-order tree walk.  If N points to the leftmost 
// Node, or is nil, returns nil.
Dictionary::Node* Dictionary::findPrev(Node* N){
        Node* cur=N;
	Node* p = N->parent;
	if (N->left!=nil){
                return findMax(N->left);
        }

        while (p!=nil && p->left==cur){
                cur=p;
		p=p->parent;
        }

        return p;
}

//Class Constructor & Destructor-------------------------

// Creates new Dictionary in the empty state.
Dictionary::Dictionary(){
	nil= new Node("NIL", 42069);
	//nil->left=nil;
	//nil->right=nil;
	//nil->parent=nil;
	root= nil;
 	current= nil;
	num_pairs=0;
}

// Copy constructor.
Dictionary::Dictionary(const Dictionary& D){
	//nil= D.nil;
	nil= new Node("NIL", 42069);
	//nil->left=nil;
	//nil->right=nil;
	//nil->parent=nil;
	root=nil;
	current=D.current;
	preOrderCopy(D.root, D.nil);
	//printf("(copy constructor) num_pairs: %d", num_pairs);
	num_pairs=D.num_pairs;
}

// Destructor
Dictionary::~Dictionary(){
	postOrderDelete(root);
	delete (nil);		
}

// Access functions --------------------------------------------------------

// size()
// Returns the size of this Dictionary.
int Dictionary::size() const{
	return num_pairs;		
}

// contains()
// Returns true if there exists a pair such that key==k, and returns false
// otherwise.
bool Dictionary::contains(keyType k) const{
	if (search(root, k)!=nil){
		return true;
	}

	return false;
}

// getValue()
// Returns a reference to the value corresponding to key k.
// Pre: contains(k)

valType& Dictionary::getValue(keyType k) const{
	if (contains(k)==false){
		throw std::logic_error("Dictionary: getValue(): key does not exist");
	}
	Node* ret=search(root, k);
	return ret->val;
}

// hasCurrent()
// Returns true if the current iterator is defined, and returns false 
// otherwise.
bool Dictionary::hasCurrent() const{
	if (current!=nil){
		return true;
	}

	return false;
}

// currentKey()
// Returns the current key.
// Pre: hasCurrent() 
keyType Dictionary::currentKey() const{
	if (!hasCurrent()){
                throw std::logic_error("Dictionary: currentKey(): cursor does not exist");
        }
	return current->key;	
}

// currentVal()
// Returns a reference to the current value.
// Pre: hasCurrent()
valType& Dictionary::currentVal() const{
	if (!hasCurrent()){
		//printf("here\n\n");
		throw std::logic_error("Dictionary: currentVal(): cursor does not exist");
	}
	return current->val;
}

// Manipulation procedures -------------------------------------------------

// clear()
// Resets this Dictionary to the empty state, containing no pairs.
void Dictionary::clear(){
	postOrderDelete(root);
	current=nil;
	root=nil;
	num_pairs=0;	
}

// setValue()
// If a pair with key==k exists, overwrites the corresponding value with v, 
// otherwise inserts the new pair (k, v).
void Dictionary::setValue(keyType k, valType v){
	
	//std::cout << "(setValue) key: (" << k << ") val: (" << v << ")" << std::endl;	
	
	//newNode->left=nil;
	//newNode->right=nil;
	Node* temp= nil;
	Node* cur=root;
	while(cur!=nil){
		temp=cur;
		if (k < cur->key){
			cur=cur->left;
		}

		else if (k > cur->key){
			cur=cur->right;
		}

		else if (k== cur->key){
			cur->val=v;
			return;
		}
	}
	Node* newNode= new Node(k, v);
	newNode->parent=temp;
	if (temp==nil){
		root=newNode;
		num_pairs++;
	}
	else if (newNode->key < temp->key){
		temp->left=newNode;
		num_pairs++;
	}
	else if (newNode->key > temp->key){
		temp->right = newNode;
		num_pairs++;
	}
		
	newNode->left=nil;
        newNode->right=nil;			
}

// remove()
// Deletes the pair for which key==k. If that pair is current, then current
// becomes undefined.
// Pre: contains(k).
void Dictionary::remove(keyType k){
	
	if (!contains(k)){
		throw std::logic_error("Dictionary: remove(): key does not exist"); 	
	}
	
	Node* deleteNode= search(root, k);
	if (deleteNode==current){
		current=nil;
	}
	//Transplant basically
	if (deleteNode->left==nil){		//if only has right child
		if (deleteNode->parent ==nil){
			root=deleteNode->right;
		}

		else if (deleteNode == deleteNode->parent->left){
			deleteNode->parent->left=deleteNode->right;
		}
		
		else{
			deleteNode->parent->right=deleteNode->right;
		}
		if (deleteNode->right !=nil){
			deleteNode->right->parent = deleteNode->parent;
		}
		//delete(deleteNode);	
		//num_pairs--;
	}

	else if (deleteNode->right ==nil){	//if only has left child
		//transplant
		if (deleteNode->parent ==nil){
                        root=deleteNode->left;
                }

                else if (deleteNode == deleteNode->parent->left){
                        deleteNode->parent->left=deleteNode->left;
                }

                else{
                        deleteNode->parent->right=deleteNode->left;
                }
                if (deleteNode->left !=nil){
                        deleteNode->left->parent = deleteNode->parent;
                }
		//delete(deleteNode);
		//num_pairs--;
	}

	else{					//if no child/2children
		Node* min= findMin(deleteNode->right);
		if (min->parent != deleteNode){	//if deleteNode is not the right min's parent
			//transplant

			if (min->parent ==nil){
                        	root=min->right;
                	}

                	else if (min == min->parent->left){
                	        min->parent->left=min->right;
        	        }

	                else{
                        	min->parent->right=min->right;
                	}
                	if (min->right !=nil){
                        	min->right->parent = min->parent;
                	}

			min->right=deleteNode->right;
			min->right->parent = min;
		}
		//transplant
		if (deleteNode->parent ==nil){
                        root=min;
                }

                else if (deleteNode == deleteNode->parent->left){
                        deleteNode->parent->left=min;
                }

                else{
                        deleteNode->parent->right=min;
                }
                if (min !=nil){
                        min->parent = deleteNode->parent;
                }
		min->left= deleteNode->left;
		min->left->parent = min;

		//delete(deleteNode);
		//num_pairs--;
	}

	delete(deleteNode);
	num_pairs--;
}

// begin()
// If non-empty, places current iterator at the first (key, value) pair
// (as defined by the order operator < on keys), otherwise does nothing. 
void Dictionary::begin(){
	//if (num_pairs>0){
		current=findMin(root);	
	//}
}

// end()
// If non-empty, places current iterator at the last (key, value) pair
// (as defined by the order operator < on keys), otherwise does nothing. 
void Dictionary::end(){
	//if (num_pairs>0){
		current=findMax(root);
	//}
}

// next()
// If the current iterator is not at the last pair, advances current 
// to the next pair (as defined by the order operator < on keys). If 
// the current iterator is at the last pair, makes current undefined.
// Pre: hasCurrent()
void Dictionary::next(){
	if (!hasCurrent()){
		throw std::logic_error("Dictionary: next(): current is undefined");
	}
	if (current==findMax(root)){
		current=nil;
		return;
	}

	//std::cout<<"current key:" << current->key << " current val: " << current->val <<std::endl;
	current=findNext(current);
	
}

// prev()
// If the current iterator is not at the first pair, moves current to  
// the previous pair (as defined by the order operator < on keys). If 
// the current iterator is at the first pair, makes current undefined.
// Pre: hasCurrent()
void Dictionary::prev(){
	if (!hasCurrent()){
                throw std::logic_error("Dictionary: prev(): current is undefined");
        }
       	if (current==findMin(root)){
                current=nil;
                return;
        }
	current=findPrev(current);
}

// Other Functions ---------------------------------------------------------

// to_string()
// Returns a string representation of this Dictionary. Consecutive (key, value)
// pairs are separated by a newline "\n" character, and the items key and value 
// are separated by the sequence space-colon-space " : ". The pairs are arranged 
// in order, as defined by the order operator <.
std::string Dictionary::to_string() const{
	std::string s= "";
	inOrderString(s, root);
	return s;
}

// pre_string()
// Returns a string consisting of all keys in this Dictionary. Consecutive
// keys are separated by newline "\n" characters. The key order is given
// by a pre-order tree walk.
std::string Dictionary::pre_string() const{
	std::string s= "";
	preOrderString(s, root);
	return s;
}

// equals()
// Returns true if and only if this Dictionary contains the same (key, value)
// pairs as Dictionary D.
bool Dictionary::equals(const Dictionary& D) const{
	if (to_string() == D.to_string()){
		return true;
	}

	return false;
}

// Overloaded Operators ----------------------------------------------------

// operator<<()
// Inserts string representation of Dictionary D into stream, as defined by
// member function to_string().
std::ostream& operator<<( std::ostream& stream, Dictionary& D ){
	return stream<< D.to_string();
}

// operator==()
// Returns true if and only if Dictionary A equals Dictionary B, as defined
// by member function equals(). 
bool operator==( const Dictionary& A, const Dictionary& B ){
	return A.equals(B);
}

// operator=()
// Overwrites the state of this Dictionary with state of D, and returns a
// reference to this Dictionary.
Dictionary& Dictionary::operator=( const Dictionary& D ){
	//std::cout << "num_pairs = " << D.num_pairs << std::endl;
	if (this!= &D){
		Dictionary temp=D;
		//std::cout << "num_pairs = " << temp.num_pairs << std::endl;
		std::swap(nil, temp.nil);
		std::swap(root, temp.root);
		std::swap(current, temp.current);
		std::swap(num_pairs, temp.num_pairs);
	}

	return *this;

}

























