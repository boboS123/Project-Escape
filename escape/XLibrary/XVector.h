/*
Xvector class
Balan Bogdan

todo:
- Iterator
- find



*/


#include "stdafx.h"
#ifndef XVECTOR_H
#define XVECTOR_H

#define XMax(a,b)            (((a) > (b)) ? (a) : (b))

enum SORT_ALGORITHM { QuickSort, InsertSort, MergeSort, RadixSort };
enum SORT_ORDER { Ascending, Descending };

template<typename ElemType>
class __declspec(dllexport)  XVector {

	//A pointer to the block of elements allocated
	ElemType* elements;

	//Capacity is the physical allocation of elements
	//Count is the number of valid and virtual allocated elements



	//Extend the physical capacity when the virtual elements size reach the physical elements size
	size_t ExtendCapacity(size_t newSize);
	size_t ShrinkCapacity(size_t newSize);


public:



	XVector() :GrowthOffset(0), ShrinkOffset(0) {}

	~XVector() {
		Destroy();
	}


	size_t count;
	size_t capacity;
	bool Sorted;


	size_t GrowthOffset;
	size_t ShrinkOffset;

	ElemType* getAdress() {
		return elements;
	}


	//Reserve a specific physical size of a vector but virtually 0 elements so we can use Insert to add them
	void Reserve(size_t capacity);

	//Reserve a specific physical size of a vector and virtually the same amount of elements to be accessed via [] operator
	void ReserveAndFill(size_t count, size_t capacity);


	//Insert an element in the Vector by physically copying the data
	int Insert(const ElemType& value);
	//void InsertMore(const ElemType& value, ...);
	void Remove(size_t index);

	//Searches for an element, returns the index
	int Find(ElemType& value, int start, int end);

	//Replaces the whole buffer adress with a new adress from a specified source
	//The new data must be correctly formated.

	void ReplaceArrayPointer(char*newAdress);

	void Destroy();

	size_t(*grw)(size_t actuallSize);
	size_t(*shr)(size_t actuallSize);
	//This function is provided by the user and so it can change what part of the element and how are compared
	int(XVector::*ComparisonFunction)(ElemType &elem1, ElemType &elem2);
	
	//int (Sample::*ptr)();

	bool operator==(XVector* v2) {

		return 1;


	}

	void SetCount(int newCount) {
		capacity = count = newCount;
	}



	void operator=(XVector &v2) {

		int i = 0;

		if (count == 0) {
			ReserveAndFill(v2.count, v2.count);
		}

		else if (count <= v2.count) {
			//Extend the initial Vector 
			ExtendCapacity(v2.count);
			count = v2.count;

		}

		else if (count > v2.count) {
			ShrinkCapacity(v2.count);
			count = v2.count;
		}


		while (i < this->count && i<v2.count) {
			elements[i] = v2[i];
			i++;

		}



	}

	/*
	XVector operator+(XVector &v2) {
	XVector result;
	result.grw = grw;

	int i = 0;
	while (i < count && i < v2.count) {
	// result[i] = elements[i] + v2[i];

	result.Insert(elements[i] + v2[i]);
	i++;

	}
	return result;
	}
	*/









	ElemType& operator [](int k) {
		return elements[k];
	}

	size_t GetCapacity() {
		return capacity;
	}


	//Insert an element in sorted order without sorting all the vector
	void InsertSorted(ElemType element) {

		Insert(element);



		for (int i = 0; i < count-1; i++) {
			if (elements[i] > element) {
				Swap(elements[i], elements[count - 1]);
			}

			else if (elements[i] == element) {
				count--;
				break;
			}



		}






	}

	void SortVector(SORT_ALGORITHM alg = QuickSort, SORT_ORDER Order = Ascending);

	ElemType pivot;
	int pivotIndex = 0;


	void Swap(ElemType &elem1, ElemType &elem2) {
		ElemType temp = elem1;
		elem1 = elem2;
		elem2 = temp;
	}


	int Partition(int start, int end) {
		pivot = elements[end];
		pivotIndex = start;
		for (int i = start; i < end; i++) {
			if (elements[i] < pivot) {
				Swap(elements[i], elements[pivotIndex]);
				pivotIndex++;
			}


		}
		Swap(elements[pivotIndex], elements[end]);

		return pivotIndex;
	}


	void QuickSortInplace(int start, int end) {

		if (start < end) {
			int partitionIndex = Partition(start, end);
			QuickSortInplace(start, partitionIndex - 1);
			QuickSortInplace(partitionIndex + 1, end);
		}


	}

	void InsertionSort() {
		int sortedIndex = 0;

		//while the sorted index is not the last element (when the array is completely sorted)
		while (sortedIndex != count - 1) {


			//Loop in to the sorted area to compare with all the elements
			for (int j = sortedIndex; j >= 0; j--) {
				if (elements[j]> elements[j + 1]) {

					Swap(elements[j], elements[j + 1]);

				}

			}

			sortedIndex++;

		}




	}





	/*


	GrowOffset = when to reallocate; When is set to 0 the reallocation will take place when the physical

	*/
	size_t SetGrowOffset(size_t count) {
		GrowthOffset = count;
	}

	size_t SetHrinkOffset(size_t count) {
		ShrinkOffset = count;
	}





};


template<typename ElemType>
void XVector<ElemType>::Reserve(size_t cpc) {

	elements = new ElemType[cpc];
	capacity = cpc;

}

template<typename ElemType>
void XVector<ElemType>::ReserveAndFill(size_t count, size_t capacity) {

	elements = new ElemType[capacity];
	capacity = capacity;
	this->count = count;

}


template<typename ElemType>
void XVector<ElemType>::ReplaceArrayPointer(char*newAdress) {
	ElemType* temp = elements;

	elements = (ElemType*)newAdress;
	
	//Delete the old buffer
	delete temp;
}



template<typename ElemType>
int XVector<ElemType>::Insert(const ElemType& value) {


	if (count >= capacity) ExtendCapacity(XMax(1, size_t(capacity * 2)));

	elements[count] = value;
	count++;
	return count - 1;
}


// 
// template<typename ElemType>
// void XVector<ElemType>::InsertMore(const ElemType& value, ...) {
// 
// 
// 	va_list arguments;
// 
// 	va_start(arguments, value);
// 	for (;;) {
// 		ElemType temp = va_arg(arguments, ElemType);
// 		if (temp == 0) break;
// 		Insert(temp);
// 	}
// 
// 	va_end(arguments);
// 
// 
// }




template<typename ElemType>
void XVector<ElemType>::Remove(size_t index) {

	for (; index < count; index++) {
		elements[index] = elements[index + 1];
	}
	count--;

	if (count + ShrinkOffset < capacity) ShrinkCapacity(shr(capacity));

}

template<typename ElemType>
int XVector<ElemType>::Find(ElemType& value, int start, int end) {

	if (count == 0) return -1;

	for (int i = start; i <= end; i++) {
		if (elements[i] == value) return i;

	}

	return -1;


}




template<typename ElemType>
size_t XVector<ElemType>::ExtendCapacity(size_t newSize) {

	ElemType* newLocation = new ElemType[newSize];

	//Move the older array in the new array place
	for (UINT i = 0; i < count; i++) {
		newLocation[i] = elements[i];

	}

	
		ElemType *oldLocation = elements;


		elements = newLocation;
		capacity = newSize;

		
		delete [] oldLocation;
		

		return newSize;
	

}




template<typename ElemType>
size_t XVector<ElemType>::ShrinkCapacity(size_t newSize) {

	ElemType* newLocation = new ElemType[newSize];

	//Move the older array in the new array place
	for (UINT i = 0; i < count; i++) {
		newLocation[i] = elements[i];

	}
	ElemType *oldLocation = elements;
	elements = newLocation;
	capacity = newSize;
	delete oldLocation;
	return newSize;

}


template<typename ElemType>
void XVector<ElemType>::SortVector(SORT_ALGORITHM alg = QuickSort, SORT_ORDER Order = Ascending) {

	int start = 0;
	switch (alg) {

	case QuickSort:
		QuickSortInplace(0, count - 1);
		break;
	case InsertSort:
		InsertionSort();
		break;

	}



}



template<typename ElemType>
void XVector<ElemType>::Destroy() {

	delete[] elements;
	count = 0;
	capacity = 0;
}


#endif