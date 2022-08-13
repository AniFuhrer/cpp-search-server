#pragma once

#include <iostream>
#include <vector>

using namespace std::string_literals;

std::ostream& operator<<(std::ostream& out, Document document) {
    out << "{ "s
        << "document_id = "s << document.id << ", "s
        << "relevance = "s << document.relevance << ", "s
        << "rating = "s << document.rating << " }"s;
    return out;
}

template<typename Iterator>
class IteratorRange
{
public:
    IteratorRange(Iterator first, Iterator last)
        :start(first), finish(last) {}

    auto begin() const { return start; }

    auto end() const { return finish; }
private:
    Iterator start, finish;

};

template<typename Iterator>
std::ostream& operator<<(std::ostream& out, IteratorRange<Iterator> pages) {
    for (auto& page : pages) {
        out << page;
    }
    return out;
}

template <typename Iterator>
class Paginator {
    std::vector<IteratorRange<Iterator>> pg;
public:
    Paginator(Iterator first, Iterator last, size_t page_size) {
        for (size_t left = distance(first, last); left > 0;) {
            const size_t current_page_size = std::min(page_size, left);
            const auto& current_page_end = next(first, current_page_size);
            pg.push_back({ first, current_page_end });
            left -= current_page_size;
            first = current_page_end;
        }
    }

    auto begin() const {
        return pg.begin();
    }

    auto end() const {
        return pg.end();
    }

    auto size() const {
        return pg.size();
    }
};