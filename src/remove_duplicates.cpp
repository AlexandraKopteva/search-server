#include "remove_duplicates.h"

#include <iostream>
#include <iterator> 
/*
��� ����������� ������������� ���������� ������� ������
������� �������� � ������� id �� ���������� �������,
� ��� ���� �������� id ��������� ��������� � ������������
� �������� �������� ������, ���������� ����.
*/
void RemoveDuplicates(SearchServer& search_server) {
	//O(wN(log N+log W))O(wN(logN+logW)) 
	using namespace std;

	vector<int> delete_id_documents;
	set<set<string>> documents;
	//��������� �� ���� id � ����������
	for (const int document_id : search_server) {
		const map<std::string_view, double> word_freqs = search_server.GetWordFrequencies(document_id);
		set<string> words;
		transform(word_freqs.begin(), word_freqs.end(), inserter(words, words.begin()),
			[](const pair<string_view, double> word) { return static_cast<string>(word.first); });

		if (documents.count(words) == 0) {
			documents.insert(words);
		} else {
			delete_id_documents.push_back(document_id);
		}
	}
	//
	for (const int id : delete_id_documents) {
		cout << "Found duplicate document id "s << id << endl;
		search_server.RemoveDocument(id);
	}
}