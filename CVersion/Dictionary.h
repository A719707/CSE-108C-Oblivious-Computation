/********
 * Michael Pham-Tran
 * mqphamtr
 ********/

//--------------------------------------------------------------------------
// Dictionary.h
// Header file for Dictionary ADT based on a Binary Search Tree. Includes a
// built in iterator called current that can perform a smallest-to-largest
// (forward) or a largest-to-smallest (reverse) iteration over all keys.
//--------------------------------------------------------------------------

#include<iostream>
#include<string>

#ifndef DICTIONARY_H_INCLUDE_
#define DICTIONARY_H_INCLUDE_

#define RED 2
#define BLACK 3

// Exported types ----------------------------------------------------------

typedef std::string keyType;  // must be comparable using <, <=, >, >=, ==, !=
typedef int         valType;

class Dictionary{

private:
	// private Node struct
	struct Node{
		// Node fields
		keyType key;
      		valType val;
      		Node* parent;
      		Node* left;
      		Node* right;

		// Node constructor
		Node(keyType k, valType v);
   	};

	// Dictionary fields
	Node* nil;
   	Node* root;
   	Node* current;
   	int   num_pairs;

	// Helper Functions (Optional) ---------------------------------------------
	
	void inOrderString(std::string& s, Node* R) const;

	void preOrderString(std::string& s, Node* R) const;

	void BST_insert(Node* M);

	void preOrderCopy(Node* R, Node* N);

	void postOrderDelete(Node* R);

	Node* search(Node* R, keyType k) const;

	Node* findMin(Node* R);

	Node* findMax(Node* R);

	Node* findNext(Node* N);

	Node* findPrev(Node* N);

	// RBT Helper Functions (Optional) -----------------------------------------
	
	void LeftRotate(Node* N);

	void RightRotate(Node* N);

	void RB_InsertFixUp(Node* N);

	void RB_Transplant(Node* u, Node* v);

	void RB_DeleteFixUp(Node* N);

	void RB_Delete(Node* N);

public:

	// Class Constructors & Destructors ----------------------------------------
	
	Dictionary();

	Dictionary(const Dictionary& D);

	~Dictionary();

	// Access functions --------------------------------------------------------
	
	int size() const;

	bool contains(keyType k) const;

	valType& getValue(keyType k) const;

	bool hasCurrent() const;

	keyType currentKey() const;

	valType& currentVal() const;

	// Manipulation procedures -------------------------------------------------
	
	void clear();

	void setValue(keyType k, valType v);

	void remove(keyType k);

	void begin();

	void end();

	void next();

	void prev();

	// Other Functions ---------------------------------------------------------
	
	std::string to_string() const;

	std::string pre_string() const;

	bool equals(const Dictionary& D) const;

	// Overloaded Operators ----------------------------------------------------
	
	friend std::ostream& operator<<( std::ostream& stream, Dictionary& D );

	friend bool operator==( const Dictionary& A, const Dictionary& B );

	Dictionary& operator=( const Dictionary& D );

};

#endif

