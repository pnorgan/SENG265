#include <iostream>										//-
#include <string.h>										//-
#include "dynamic_array.h"									//-
												//-
using namespace std;										//-
												//-
// ********** public functions **********							//-
												//-
Dynamic_array::Dynamic_array() {								//-
	head_p = NULL;										//-
	size = 0;										//-
}												//-
												//-
Dynamic_array::Dynamic_array(Dynamic_array & d) {						//-
	head_p = copy_blocks(d.head_p);
	size = d.size;
}												//-
												//-
Dynamic_array &Dynamic_array::operator=(Dynamic_array & d) {					//-
	// 
	while (head_p != NULL) {
		delete head_p;
		head_p = head_p->next_p;
	}
	head_p = copy_blocks(d.head_p);
	size = d.size;
	return *this;										//-
}												//-
												//-
Dynamic_array::~Dynamic_array() {								//-
	//
	while (head_p != NULL) {
		Block * temp = head_p;
		delete head_p;
		head_p = temp->next_p;
	}
}												//-
												//-
void Dynamic_array::print_state(void) {								//-
	cout << "size:" << size << endl;							//-
	for (Block * p = head_p; p != NULL; p = p->next_p) {					//-
		cout << "\tsize:" << p->size << " elements:";					//-
		for (int i = 0; i < p->size; i++) {						//-
			cout << p->a[i] << ' ';							//-
		}										//-
		cout << endl;									//-
	}											//-
}												//-
												//-
int Dynamic_array::get_size(void) {								//-
	return size;
}												//-
												//-
int& Dynamic_array::operator[](int i) {								//-
	// check i for range error								//-
	if (i < 0 || i >= size) {								//-
		throw Subscript_range_exception();						//-
	}											//-
												//-
	// find target block and index								//-
	Block_position position = find_block(i);						//-
												//-
	// return element at index i								//-
	return position.block_p->a[position.i];							//-
}												//-
												//-
void Dynamic_array::insert(int x, int i) {							//-
	// case 1: range error									//-
	if (i < 0 || i > size) {								//-
		throw Subscript_range_exception();						//-
	}											//-
												//-
	// case 2: empty array									//-
	if (size == 0) {									//-
		// create linked list consisting of a single new block				//-
		Block * new_block_p = new Block;						//-
		new_block_p->size = 1;								//-
		new_block_p->a[0] = x;								//-
		new_block_p->next_p = NULL;							//-
												//-
		// insert new block								//-
		insert_blocks(NULL, new_block_p);						//-
												//-
		// update total size								//-
		size++;										//-
												//-
		return;										//-
	}											//-
												//-
	// find target block and index								//-
	Block_position position = find_block(i);						//-
												//-
	// case 3: non-empty array; new block not needed					//-
	if (position.block_p->size < BLOCK_SIZE) {						//-
		// shift block array right							//-
		for (int j = position.block_p->size; j > position.i; j--) {			//-
			position.block_p->a[j] = position.block_p->a[j-1];			//-
		}										//-
												//-
		// assign x									//-
		position.block_p->a[position.i] = x;						//-
												//-
		// update array and block size							//-
		size++;										//-
		position.block_p->size++;							//-
												//-
	// case 4: non-empty array; new block needed						//-
	} else {										//-
		// create new block								//-
		Block * new_block_p = new Block;						//-
		new_block_p->size = 1;								//-
		new_block_p->next_p = NULL;							//-
												//-
		// case 4.a: insert x into old block						//-
		if (position.i < BLOCK_SIZE) {							//-
			// copy last array element to new block					//-
			new_block_p->a[0] = position.block_p->a[BLOCK_SIZE-1];			//-
												//-
			// shift old block array right						//-
			for (int j = position.block_p->size-1; j > position.i; j--) {		//-
				position.block_p->a[j] = position.block_p->a[j-1];		//-
			}									//-
												//-
			// assign x into old block						//-
			position.block_p->a[position.i] = x;					//-
												//-
		// case 4.b: insert x into new block						//-
		} else {									//-
			new_block_p->a[0] = x;							//-
		}										//-
												//-
		// update total size								//-
		size++;										//-
												//-
		// insert new block into linked list						//-
		insert_blocks(position.block_p, new_block_p);					//-
	}											//-
}												//-
												//-
void Dynamic_array::insert(Dynamic_array &p, int i) {						//-
	// case 1: range error									//-
	if (i < 0 || i > size) {
		throw Subscript_range_exception();
	}

	// case 2: parameter array empty							//-
	if (p.size == 0) {
		return;
	}

	// case 3: array empty									//-
	if (size == 0) {
			// copy blocks from p
			Block * copy = copy_blocks(p.head_p);

			//insert new blocks
			insert_blocks(NULL, copy);

			// update total size
			size = copy->size;

			return;
	}

	// find target block and index								//-
	Block_position position = find_block(i);						//-
	// case 4: array non-empty; new blocks not needed					//-

	if (position.block_p->size + p.head_p->size < BLOCK_SIZE) {
		int n = 0;
		// shift elements to the right to make room for &p elements
		while (n < p.head_p->size) {
			// shift block array right
			for (int j = position.block_p->size; j > position.i; j--) {
				position.block_p->a[j] = position.block_p->a[j-1];
			}
			// try to update block at position size at end of if statement
			position.block_p->size++;
			n++;
		}

		n = 0;
		int k = position.i;
		//copy p into position
		while (n < p.head_p->size) {
			position.block_p->a[k] = p.head_p->a[n];
			n++;
			k++;
		}

		// update size
		size += p.head_p->size;

	// case 5: array non-empty; new blocks needed						//-
	} else {
		// copy p									//-
		Block * copy_p = copy_blocks(p.head_p);						//-
		// case 5.a: insert position at start of block					//-
		if (position.i == 0) {
			insert_blocks(position.pre_block_p, copy_p);
			// try to update size at end instead of here
			size += copy_p->size;

		// case 5.b: insert position at middle of block					//-

		} else if (position.i < BLOCK_SIZE - 1) {
			Block * new_block_p = new Block;
			int l;
			l = position.i;
			for (int r = 0; r < position.block_p->size - position.i; r++) {
				new_block_p->a[r] = position.block_p->a[l];
				l++;
				new_block_p->size++;
			}
			insert_blocks(position.block_p, new_block_p);
			insert_blocks(position.block_p, copy_p);
			position.block_p->size = copy_p->size;
			size = size + copy_p->size;
			


		// case 5.c: insert position after end of block					//-
		} else {
			insert_blocks(position.block_p, copy_p);
			size = size + copy_p->size;
		}
		// update total size								//-
	}
}												//-
												//-
void Dynamic_array::remove(int i) {								//-
	// case 1: range error									//-
	if (i < 0 || i >= size) {
		throw Subscript_range_exception();
	}

	// find target block and index								//-
	Block_position position = find_block(i);

	// case 2: block size == 1								//-
	if (position.block_p->size == 1) {
		if (size == 1)
			remove_blocks(NULL, position.block_p, position.block_p);
		else
			remove_blocks(position.pre_block_p, position.block_p, position.block_p);
		
	// case 3: block size > 1								//-
	} else {
		for (int k = position.i; k < position.block_p->size; k++) {
			position.block_p->a[k] = position.block_p->a[k+1];
		}
		// dec block size
		position.block_p->size = position.block_p->size - 1;
	}

	// update total size									//-
	size--;
}												//-
												//-
void Dynamic_array::remove(int start, int end) {						//-
	// case 1: range error
	if (start < 0 || start > size || end < 0 || end > size) {
		throw Subscript_range_exception();
	}

	// case 2: invalid start and end 
	if (start > end) {
		throw Subscript_range_exception();
	}

	// find start block position
	Block_position start_position = find_block(start);

	// find end block position
	Block_position end_position = find_block(end);


	// case 3: remove all blocks
	if (end - start >= size) {
		remove_blocks(NULL, start_position.block_p, end_position.block_p);
		size = 0;
		return;
	}

	int k = 0;
	int j = 0;
	int count;
	int n = end_position.i;
	Block * temp = new Block;


	// case 4: removal from 1 block
	if (start_position.block_p == end_position.block_p) {
		for (k = start_position.i; k < end_position.i; k++) {
			start_position.block_p->a[k] = start_position.block_p->a[n++];
			start_position.block_p->size--;
			if (start_position.block_p->size == 0) {
				if (start_position.pre_block_p != NULL) {
					//Block * copy_p = copy_blocks(start_position.block_p);
					remove_blocks(start_position.pre_block_p, start_position.block_p, start_position.block_p);
					//start_position.block_p = copy_p->next_p;
				} else {
					//Block * copy_p = start_position.block_p;
					remove_blocks(NULL, start_position.block_p, start_position.block_p);
					//start_position.block_p = copy_p->next_p;
				}
			}
		}
	// case 5: removal from mulitple blocks
	} else {
		// case 5.a first block
		count = 0;
		while (start_position.block_p->size != start_position.i) {
			//cout << "here while" << endl;
			start_position.block_p->size--;
			//cout << start_position.block_p->size <<endl;
			count++;
			if (start_position.block_p->size == 0) {
				//cout << "here0" << endl;
				if (start_position.block_p->next_p != NULL){
					temp = start_position.block_p;
					remove_blocks(start_position.pre_block_p, start_position.block_p, start_position.block_p);
					start_position.block_p = temp->next_p;
					break;
				} else {
					remove_blocks(start_position.pre_block_p, start_position.block_p, start_position.block_p);
					size = size - (end-start);
					return;
				}
			}
			if (start_position.block_p->size == start_position.i) {
				if (start_position.block_p->next_p != NULL) {
					
					start_position.pre_block_p = start_position.block_p;
					start_position.block_p = start_position.block_p->next_p;
					break;
				} else {
					size = size - (end-start);
					return;
				}
			}


		}


		for (k = count; k < end; k++) {
			count = 0;
			n = end_position.i;
			// case 5.b last block
			if (start_position.block_p == end_position.block_p) {
				//cout << "here last block" << endl;
				for (j = 0; j < start_position.block_p->size; j++) {
					if (n < end_position.block_p->size) {
						start_position.block_p->a[j] = start_position.block_p->a[n++];
					}
					count++;
				}
				start_position.block_p->size = end_position.block_p->size - end_position.i;
				k += count;

				if (start_position.block_p->size == 0) {
					if (start_position.block_p->next_p != NULL) {
						//cout << "here1" << endl;
						temp = start_position.block_p->next_p;
						remove_blocks(start_position.pre_block_p, start_position.block_p, start_position.block_p);
						start_position.block_p = temp;
						size = size - (end-start);
						return;
					} else {
						//cout << "here2" << endl;
						remove_blocks(start_position.pre_block_p, start_position.block_p, start_position.block_p);
						size = size - (end-start);
						return;
					}
				} else {
					//cout << "here3" << endl;
					size = size - (end - start);
					return;
				}
			}

			// case 5.c middle blocks 
			start_position.block_p->size--;
			if (start_position.block_p->size == 0) {
				if (start_position.block_p->next_p != NULL) {
					//cout << "here4" << endl;
					temp = start_position.block_p->next_p;
					remove_blocks(start_position.pre_block_p, start_position.block_p, start_position.block_p);
					start_position.block_p = temp;
				} else {
					//cout << "here5" << endl;
					remove_blocks(start_position.pre_block_p, start_position.block_p, start_position.block_p);
					size = size - (end-start);
					return;
				}
			}
		}

	}
	size = size - (end-start);

}												//-
												//-
// ********** private functions **********							//-
												//-
// purpose											//-
//	return b where										//-
//		if i < size									//-
//			b.block_p->a[b.i] is the ith element overall				//-
//		else										//-
//			b.block_p->a[b.i-1] is the i-1st element overall			//-
//												//-
//		if b.block_p == head_p								//-
//			b.pre_block_p is NULL							//-
//		else										//-
//			b.pre_block_p points to block preceding b.block_p			//-
// preconditions										//-
//	i in [1..size]										//-
Dynamic_array::Block_position Dynamic_array::find_block(int i) {				//-
	Block_position position;								//-
												//-
	// scan Block list									//-
	position.i = i;										//-
	position.pre_block_p = NULL;								//-
	for (position.block_p = head_p;								//-
	position.block_p != NULL;								//-
	position.block_p = position.block_p->next_p) {						//-
		// found in current block							//-
		if (position.i < position.block_p->size) {					//-
			break;									//-
		}										//-
												//-
		// special case: insert just after last element					//-
		if (i == size && position.block_p->next_p == NULL) {				//-
			break;									//-
		}										//-
												//-
		// not found yet: advance							//-
		position.pre_block_p = position.block_p;					//-
		position.i -= position.block_p->size;						//-
	}											//-
												//-
	return position;									//-
}												//-
												//-
// purpose											//-
//	create a new linked list which is a copy of the list pointed to p			//-
//	return a pointer to the head of the new linked list					//-
// preconditions										//-
//	p is the head of a possibly empty linked list of blocks					//-
Dynamic_array::Block * Dynamic_array::copy_blocks(Block * p) {					//-
	Block * new_head_p = NULL;								//-
	Block * new_p;										//-
	while (p != NULL) {									//-
		// allocate and link in new block						//-
		if (new_head_p == NULL) {							//-
			new_p = new Block;							//-
			new_head_p = new_p;							//-
		} else {									//-
			new_p->next_p = new Block;						//-
			new_p = new_p->next_p;							//-
		}										//-
												//-
		// copy the elements								//-
		new_p->size = p->size;								//-
		for (int i = 0; i < p->size; i++) {						//-
			new_p->a[i] = p->a[i];							//-
		}										//-
												//-
		// advance									//-
		p = p->next_p;									//-
	}											//-
												//-
	// terminate new list									//-
	if (new_head_p != NULL) {								//-
		new_p->next_p = NULL;								//-
	}											//-
												//-
	return new_head_p;									//-
}												//-
												//-
// purpose											//-
//	insert the list headed by src_p into the list headed by head_p				//-
//	if dst_p is NULL									//-
//		insert the list at the start of the list headed by head_p			//-
//	else											//-
//		insert the list just after block dst_p						//-
// preconditions										//-
//	list headed by src_p is non-empty							//-
//	list headed by src_p has no blocks in common with the list headed by head_p		//-
void Dynamic_array::insert_blocks(Block * dst_p, Block * src_p) {				//-
	// find the last block in the list headed by src_p					//-
	Block * p = src_p;									//-
	while (p->next_p != NULL) {								//-
		p = p->next_p;									//-
	}											//-
												//-
	// insert at head									//-
	if (dst_p == NULL) { // insert at head							//-
		p->next_p = head_p;								//-
		head_p = src_p;									//-
												//-
	// insert after dst_p									//-
	} else { // insert after dst_p								//-
		p->next_p = dst_p->next_p;							//-
		dst_p->next_p = src_p;								//-
	}											//-
}												//-
												//-
// purpose											//-
//	remove the blocks pointed to by start_p and end_p, and all the blocks between		//-
// preconditions										//-
//	start_p and end_p point to blocks in the list headed by head_p				//-
//	end_p points to either the same block as start_p or a block to its right		//-
//	if start_p == head_p									//-
//		pre_start_p == NULL								//-
//	else											//-
//		pre_start_p points to the block immediately preceding start_p			//-
//												//-
void Dynamic_array::remove_blocks(Block * pre_start_p, Block * start_p, Block * end_p) {	//-
	// release the blocks									//-
	while (1) {										//-
		// release start_p								//-
		Block * p = start_p->next_p;							//-
		delete start_p;									//-
												//-
		// advance									//-
		if (start_p == end_p) {								//-
			break;									//-
		} else {									//-
			start_p = p;								//-
		}										//-
	}											//-
												//-
	// link left and right sublists								//-
	if (pre_start_p == NULL) {								//-
		head_p = end_p->next_p;								//-
	} else {										//-
		pre_start_p->next_p = start_p->next_p;						//-
	}											//-
}												//-
