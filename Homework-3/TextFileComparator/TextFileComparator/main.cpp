#include "interface.hpp"
#include<fstream>

using std::ifstream;
using std::cout;

int main(int argc, char* argv[])
{
	ifstream file1(argv[1]);
	ifstream file2(argv[2]);

	if (!file1.is_open() || !file2.is_open())
	{
		cout << "Couldnt open file for reading";
		return 1;
	}

	Comparator cmp;
	ComparisonReport report = cmp.compare(file1, file2);

	int file1Words = report.uniqueWords[0].size();
	int file2Words = report.uniqueWords[1].size();
	int commonWordsCount = report.commonWords.size();

	double file1WordsInFile2Percentage = ((double)commonWordsCount / (file1Words + commonWordsCount)) * 100;
	double file2WordsInFile1Percentage = ((double)commonWordsCount / (file2Words + commonWordsCount)) * 100;

	cout << "file1 contains " << file1Words + commonWordsCount
		<< " words and " << commonWordsCount << " are contained in file2." << "(" << file1WordsInFile2Percentage << "%)" << "\n";

	cout << "file2 contains " << file2Words + commonWordsCount
		<< " words and " << commonWordsCount << " are contained in file1." << "(" << file2WordsInFile1Percentage << "%)" << "\n";

	return 0;
}