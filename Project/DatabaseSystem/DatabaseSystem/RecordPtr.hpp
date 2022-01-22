#pragma once

/**
 * @brief Descriptor of BPTree pointers to the records.
*/
class RecordPtr
{
public:
	RecordPtr() : pageNumber(-1), indexInPage(-1) {}
	RecordPtr(int pageNumber, int indexInPage) : pageNumber(pageNumber), indexInPage(indexInPage) {}

	/**
	 * @return the number of the page
	*/
	int getPage()
	{
		return pageNumber;
	}

	/**
	 * @return the index of the record in the page
	*/
	int getIndexInPage()
	{
		return indexInPage;
	}

private:
	int pageNumber, indexInPage;
};