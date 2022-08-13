#pragma once

enum class DocumentStatus {
    ACTUAL,
    IRRELEVANT,
    BANNED,
    REMOVED,
};

struct DocumentData {
    int rating;
    DocumentStatus status;
};

struct Document {
    Document() = default;

    Document(int i, double rel, int rat);

    int id = 0;
    double relevance = 0;
    int rating = 0;
};
