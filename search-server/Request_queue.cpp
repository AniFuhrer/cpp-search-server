#include "Request_queue.h"

using namespace std;

    RequestQueue::RequestQueue(const SearchServer& search_server)
        :search_server_(search_server) {
    }

    vector<Document> RequestQueue::AddFindRequest(const string& raw_query, DocumentStatus status) {
        vector<Document> docs = search_server_.FindTopDocuments(raw_query, status);
        QueryResult find(raw_query, docs.size());
        if (requests_.size() >= min_in_day_) {
            requests_.pop_front();
        }
        requests_.push_back(find);
        return docs;
    }

    vector<Document> RequestQueue::AddFindRequest(const string& raw_query) {
        vector<Document> docs = search_server_.FindTopDocuments(raw_query);
        QueryResult find(raw_query, docs.size());
        if (requests_.size() >= min_in_day_) {
            requests_.pop_front();
        }
        requests_.push_back(find);
        return docs;
    }

    int RequestQueue::GetNoResultRequests() const {
        int empty_rq_count = 0;
        for (auto req : requests_) {
            if (req.found_docs == 0) {
                empty_rq_count += 1;
            }
        }
        return empty_rq_count;
    }

    RequestQueue::QueryResult::QueryResult(string request_, size_t found_docs_) {
        request = request_;
        found_docs = found_docs_;
        }
