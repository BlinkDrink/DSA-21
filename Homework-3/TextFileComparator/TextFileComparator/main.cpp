#include "interface.hpp"
#include<fstream>
#include <time.h>

using std::ifstream;
using std::filebuf;
using std::cout;

int main(int argc, char* argv[])
{
	clock_t tStart = clock();
	ifstream file1(argv[1]);
	ifstream file2(argv[2]);

	if (!file1.is_open() || !file2.is_open())
	{
		cout << "Couldnt open file for reading";
		return 1;
	}

	file1.seekg(0, file1.end);
	size_t file1Size = file1.tellg();
	file1.seekg(0, file1.beg);

	file2.seekg(0, file2.end);
	size_t file2Size = file2.tellg();
	file2.seekg(0, file2.beg);

	if (file1Size < file2Size)
		std::swap(file1, file2);

	Comparator cmp;
	ComparisonReport report = cmp.compare(file1, file2);

	int file1Words = report.uniqueWords[0].countOfAllWords();
	int file2Words = report.uniqueWords[1].countOfAllWords();
	int commonWordsCount = report.commonWords.countOfAllWords();

	double file1WordsInFile2Percentage = ((double)commonWordsCount / (file1Words + commonWordsCount)) * 100;
	double file2WordsInFile1Percentage = ((double)commonWordsCount / (file2Words + commonWordsCount)) * 100;

	cout << argv[1] << " contains " << file1Words + commonWordsCount
		<< " words and " << commonWordsCount << " are contained in file2" << "(" << file1WordsInFile2Percentage << "%)" << "\n";

	cout << argv[2] << " contains " << file2Words + commonWordsCount
		<< " words and " << commonWordsCount << " are contained in file1" << "(" << file2WordsInFile1Percentage << "%)" << "\n";

	cout << "Time taken " << (double)(clock() - tStart) / CLOCKS_PER_SEC;

	return 0;
}