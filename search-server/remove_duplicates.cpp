#include"remove_duplicates.h"

using namespace std;

void RemoveDuplicates(SearchServer& search_server) {
	set<int> to_remove;
	map<set<string>, vector<int>> not_duplicate;
	for (const int document_id : search_server) {
		set<string> words;
		map<string, double> word_with_frequencie = search_server.GetWordFrequencies(document_id);
		for (auto [word, freq] : word_with_frequencie) {
			words.insert(word);
		}
		if (not_duplicate.count(words) == 0) {
			not_duplicate[words].push_back(document_id);
		} else {
			to_remove.insert(document_id);
		}
	}
	for (const int id : to_remove)
	{
		search_server.RemoveDocument(id);
	}
}