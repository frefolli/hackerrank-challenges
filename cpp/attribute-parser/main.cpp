#include <bits/stdc++.h>

class Tag {
    private:
        std::map<std::wstring, Tag*>* childs = NULL;
        std::map<std::wstring, std::wstring>* values = NULL;
        std::wstring name = L"Unnamed";
        
        void init() {
            this->childs = new std::map<std::wstring, Tag*>();
            this->values = new std::map<std::wstring, std::wstring>();
        }
        
        void init(std::wstring name) {
            this->init();
            this->name = name;
        }
    public:
        Tag() {
            init();
        }
        
        Tag(std::wstring name) {
            init(name);
        }
        
        ~Tag() {
            delete this->values;
            for(auto it = this->childs->begin();
                it != this->childs->end(); it++) {
                if (it->second != NULL)
                    delete it->second;
            }
            delete this->childs;
        }
        
        void putTag(std::wstring name, Tag* tag) {
            (*(this->childs)) [name] = tag;
        }
        
        void putValue(std::wstring name, std::wstring value) {
            (*(this->values)) [name] = value;
        }
        
        Tag* getTag(std::wstring name) {
            auto it = this->childs->find(name);
            if (it != this->childs->end()) {
                return it->second;
            } else {
                return NULL;
            }
        }
        
        std::wstring getValue(std::wstring name) {
            auto it = this->values->find(name);
            if (it != this->values->end()) {
                return it->second.substr(1, it->second.size() - 2);
            } else {
                return L"Not Found!";
            }
        }
        
        std::wstring getName() {
            return this->name;
        }
        
        bool isClosingTag() {
            return (this->name.size() > 0) && (this->name[0] == '/');
        }
        
        std::wstring toString() {
            std::wstring s_attributes = L"";
            std::wstring s_childs = L"";
            
            for(auto it = this->childs->begin();
                it != this->childs->end(); it++) {
                s_attributes += L" " + it->second->toString();
            }
            
            for(auto it = this->values->begin();
                it != this->values->end(); it++) {
                s_attributes += L" (" + it->first + L" " + it->second + L")";
            }
            
            return L"(" + this->name + s_attributes + L" " + s_childs + L")";
        }
};

class Query {
    std::vector<std::wstring>* path;
    std::wstring attribute;
    public:
        Query() {
            this->path = new std::vector<std::wstring>();
        }
        
        ~Query() {
            delete this->path;
        }
        
        void setAttribute(std::wstring name) {
            this->attribute = name;
        }
        
        void pushPath(std::wstring path) {
            this->path->push_back(path);
        }
        
        std::wstring apply(Tag* document) {
            for(auto it = this->path->begin();
                it != this->path->end(); it++) {
                document = document->getTag(*it);
                if (document == NULL) {
                    return L"Not Found!";
                }
            }
            return document->getValue(this->attribute);
        }
        
        std::wstring toString() {
            std::wstring s_path = L"";
            
            for(auto it = this->path->begin();
                it != this->path->end(); it++) {
                s_path += *it + L" ";
            }
            
            return L"(" + s_path + this->attribute + L")";
        }
};

class TagParser {
    public:
    TagParser() {
        
    }
    
    Tag* parse(std::wstring text) {
        auto tokens = this->tokenize(&text);
        Tag* tag = new Tag(tokens->front());
        
        for(auto it = tokens->begin() + 1; it != tokens->end();) {
            std::wstring name = *it;
            it++;
            it++;
            std::wstring value = *it;
            it++;
            tag->putValue(name, value);
        }
        
        delete tokens;
        return tag;
    }
    
    std::vector<std::wstring>* tokenize(std::wstring* text) {
        // i assume that first and last are tag enclosers
        std::vector<std::wstring>* tokens = new std::vector<std::wstring>();
        std::wstring buffer;
        
        for(auto it = text->begin() + 1; it != text->end() - 1; it++) {
            if (*it == ' ') {
                if (buffer.size() > 0) {
                    if (buffer[0] == '"') {
                        buffer += *it;
                    } else {
                        tokens->push_back(buffer);
                        buffer = L"";
                    }
                }
            } else if (*it == '"') {
                if (buffer.size() > 0) {
                    if (buffer[0] == '"') {
                        buffer += *it;
                        tokens->push_back(buffer);
                        buffer = L"";
                    } else {
                        tokens->push_back(buffer);
                        buffer = L"";
                        buffer += *it;
                    }
                } else {
                    buffer += *it;
                }
            } else {
                buffer += *it;
            }
        }
        if (buffer.size() > 0) {
            tokens->push_back(buffer);
        }
        
        return tokens;
    }
};

class QueryParser {
    public:
    QueryParser() {
        
    }
    
    Query* parse(std::wstring text) {
        auto tokens = this->tokenize(&text);
        Query* query = new Query();
        
        for(auto it = tokens->begin(); it != tokens->end() - 1; it++) {
            query->pushPath(*it);
        }
        query->setAttribute(tokens->back());
        
        return query;
    }
    
    std::vector<std::wstring>* tokenize(std::wstring* text) {
        std::vector<std::wstring>* tokens = new std::vector<std::wstring>();
        std::wstring buffer;
        
        for(auto it = text->begin(); it != text->end(); it++) {
            if (*it == '~' || *it == '.') {
                tokens->push_back(buffer);
                buffer = L"";
            } else {
                buffer += *it;
            }
        }
        if (buffer.size() > 0) {
            tokens->push_back(buffer);
        }
        
        return tokens;
    }
};

int readDocument(int missing, Tag* document, TagParser* parser) {
    std::wstring line = L"";
    
    if (missing != 0) { 
        std::getline(std::wcin, line);
        //std::wcout << line << std::endl;
        Tag* tag = NULL;
        tag = parser->parse(line);
        
        if (tag->getName() == L"/" + document->getName()) {
            return missing - 1;
        } else {
            missing = readDocument(missing - 1, tag, parser);
            document->putTag(tag->getName(), tag);
            return readDocument(missing, document, parser);
        }
    } else {
        return 0;
    }
}

void readQuery(Tag* world, QueryParser* parser) {
    std::wstring line = L"";
    std::getline(std::wcin, line);
    Query* query = parser->parse(line);
    //std::wcout << query->toString() << " => " << query->apply(world) << std::endl;
    std::wcout << query->apply(world) << std::endl;
    delete query;
}

void readQueries(int missing, Tag* world, QueryParser* parser) {
    for(int i = 0; i < missing; i++)
        readQuery(world, parser);
}

int main() {
    int tags, queries;
    std::wcin >> tags >> queries;
    std::wcin.getline(NULL, NULL);
    
    TagParser* tagParser = new TagParser();
    QueryParser* queryParser = new QueryParser();
    Tag* world = new Tag();
    
    readDocument(tags, world, tagParser);
    //std::wcout << world->toString() << std::endl;
    readQueries(queries, world, queryParser);
    
    delete tagParser;
    delete queryParser;
    delete world;
    /* Enter your code here. Read input from STDIN. Print output to STDOUT */   
    return 0;
}
