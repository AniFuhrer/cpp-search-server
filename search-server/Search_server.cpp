#include "Search_server.h"

using namespace std;

    SearchServer::SearchServer(string stop_words) {
        for (const string& word : SplitIntoWords(stop_words)) {
            if (!IsValidWord(word)) {
                throw invalid_argument("Mistake in stop words");
            }
            stop_words_.insert(word);
        }
    }

    void SearchServer::SetStopWords(const string& text) {
        for (const string& word : SplitIntoWords(text)) {
            stop_words_.insert(word);
        }
    }

    void SearchServer::ThrowIfWrongDocumentId(int document_id) {
        if (document_id < 0) {
            throw invalid_argument("Invalid document ID");
        }
        if (documents_.count(document_id)) {
            throw invalid_argument("Documents already have this ID");
        }
    }

    void SearchServer::AddDocument(int document_id, const string& document, DocumentStatus status, const vector<int>& ratings) {
        ThrowIfWrongDocumentId(document_id);
        const vector<string> words = SplitIntoWordsNoStop(document);
        const double inv_word_count = 1.0 / words.size();
        for (const string& word : words) {
            word_to_document_freqs_[word][document_id] += inv_word_count;
            document_to_word_freqs_[document_id][word] += inv_word_count;
        }
        documents_.emplace(document_id,
            DocumentData{
                ComputeAverageRating(ratings),
                status
                });
        document_id_.insert(document_id);
    }

    vector<Document> SearchServer::FindTopDocuments(const string& raw_query, DocumentStatus status) const {
        return FindTopDocuments(raw_query, [status](int document_id, DocumentStatus document_status, int rating)
            { return document_status == status; });
    }

    vector<Document> SearchServer::FindTopDocuments(const string& raw_query) const {
        return FindTopDocuments(raw_query, DocumentStatus::ACTUAL);
    }

    size_t SearchServer::GetDocumentCount() const {
        return documents_.size();
    }

    tuple<vector<string>, DocumentStatus> SearchServer::MatchDocument(const string& raw_query, int document_id) const {
        const Query query = ParseQuery(raw_query);
        vector<string> matched_words;
        for (const string& word : query.plus_words) {
            if (word_to_document_freqs_.count(word) == 0) {
                continue;
            }
            if (word_to_document_freqs_.at(word).count(document_id)) {
                matched_words.push_back(word);
            }
        }
        for (const string& word : query.minus_words) {
            if (word_to_document_freqs_.count(word) == 0) {
                continue;
            }
            if (word_to_document_freqs_.at(word).count(document_id)) {
                matched_words.clear();
                break;
            }
        }
        tuple<vector<string>, DocumentStatus> res = { matched_words, documents_.at(document_id).status };
        return res;
    }

    const map<string, double>& SearchServer::GetWordFrequencies(int document_id) const{
        static map<string, double> empty_return_freqs;
        empty_return_freqs = document_to_word_freqs_.at(document_id);
        return empty_return_freqs;
    }

    void SearchServer::RemoveDocument(int document_id) {
        for (const auto& [s, inf] : word_to_document_freqs_) {
            word_to_document_freqs_[s].erase(s.find(document_id));
        }
        document_to_word_freqs_.erase(document_id);
        document_id_.erase(document_id);
        documents_.erase(document_id);
    }


    bool SearchServer::IsStopWord(const string& word) const {
        return stop_words_.count(word) > 0;
    }

    vector<string> SearchServer::SplitIntoWordsNoStop(const string& text) const {
        if (!IsValidWord(text)) {
            throw invalid_argument("Special symbols in string");
        }
        vector<string> words;
        for (const string& word : SplitIntoWords(text)) {
            if (IsJustMinus(text)) {
                throw invalid_argument("Minus in stdstring");
            }
            if (!IsStopWord(word)) {
                words.push_back(word);
            }
        }
        return words;
    }

    int SearchServer::ComputeAverageRating(const vector<int>& ratings) {
        if (ratings.empty()) {
            return 0;
        }
        int rating_sum = accumulate(ratings.begin(), ratings.end(), 0);
        
        return rating_sum / static_cast<int>(ratings.size());
    }

    SearchServer::QueryWord SearchServer::ParseQueryWord(string text) const {
        if (IsJustMinus(text)) {
            throw invalid_argument("Minus in string");
        }
        bool is_minus = false;
        // Word shouldn't be empty
        if (text[0] == '-') {
            is_minus = true;
            text = text.substr(1);
        }
        if (IsDoubleMinus(text)) {
            throw invalid_argument("Double minus in query");
        }
        return {
            text,
            is_minus,
            IsStopWord(text)
        };
    }

    SearchServer::Query SearchServer::ParseQuery(const string& text) const {
        Query query;
        if (!IsValidWord(text)) {
            throw invalid_argument("Special symbols in string");
        }
        for (const string& word : SplitIntoWords(text)) {
            const QueryWord query_word = ParseQueryWord(word);
            if (!query_word.is_stop) {
                if (query_word.is_minus) {
                    query.minus_words.insert(query_word.data);
                }
                else {
                    query.plus_words.insert(query_word.data);
                }
            }
        }
        return query;
    }

    bool SearchServer::IsValidWord(const string& word) {
        return none_of(word.begin(), word.end(), [](char c) {
            return (c >= '\0' && c < ' ');
            });
    }

    bool SearchServer::IsDoubleMinus(const string& word) {
        if (word[0] == '-') {
            return true;
        }
        return false;
    }

    bool SearchServer::IsJustMinus(const string& word) {
        if (word == "-") {
            return true;
        }

        return false;
    }

    double SearchServer::ComputeWordInverseDocumentFreq(const string& word) const {
        return log(GetDocumentCount() * 1.0 / word_to_document_freqs_.at(word).size());
    }
