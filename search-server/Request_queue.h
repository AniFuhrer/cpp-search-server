#pragma once
#include "Search_server.h"
#include "Document.h"

#include <string>
#include <vector>
#include <deque>
#include <iostream>

class RequestQueue {
public:
    explicit RequestQueue(const SearchServer& search_server);
        
    template <typename DocumentPredicate>
    std::vector<Document> AddFindRequest(const std::string& raw_query, DocumentPredicate document_predicate);

    std::vector<Document> AddFindRequest(const std::string& raw_query, DocumentStatus status);

    std::vector<Document> AddFindRequest(const std::string& raw_query);

    int GetNoResultRequests() const;

private:

    struct QueryResult {
        QueryResult() = default;

        QueryResult(std::string request_, size_t found_docs_);
             
        std::string request = "";
        size_t found_docs = 0;
    };
    std::deque<QueryResult> requests_;
    const static int min_in_day_ = 1440;
    const SearchServer& search_server_;
};

template <typename DocumentPredicate>
std::vector<Document> RequestQueue::AddFindRequest(const std::string& raw_query, DocumentPredicate document_predicate) {
    std::vector<Document> docs = search_server_.FindTopDocuments(raw_query, document_predicate);
    QueryResult find(raw_query, docs.size());
    if (requests_.size() >= min_in_day_) {
        requests_.pop_front();
    }
    requests_.push_back(find);
    return docs;
}