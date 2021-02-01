#ifndef TEST_PARSER_FILE_H
#define TEST_PARSER_FILE_H

#include <cassert>
#include <memory>
#include <fstream>
#include <iostream>
#include <string_view>
#include <filesystem>

class parse_file {
public:
    //
    template <class T>
    static void null_deleter(T* t) {}
    // 准备解析文件
    parse_file(std::filesystem::path file) {
        auto* fs = new std::ifstream(file);
        assert(fs->is_open());
        spis_.reset(fs);
    }
    // 
    parse_file(std::istream& is) {
        assert(!is.bad());
        spis_.reset(&is, null_deleter<std::istream>);
    }
    //
    template <class State>
    void run(int (*executor)(State*, const char*, const char *), State* s) {
        std::string tmp;
        while(!spis_->eof()) {
            std::getline(*spis_, tmp);
            // 注意最后一行可能不需要进行 "\n" 的补充
            if(spis_->peek() != EOF) tmp.push_back('\n');
            std::cout << "> " << std::string_view(tmp.c_str(), tmp.size()) << std::flush;
            int e = executor(s, tmp.data(), tmp.data() + tmp.size());
            std::cout << "\n= " << e << std::endl;
        }
    }
private:
    // 
    std::shared_ptr<std::istream> spis_;
};

#endif // TEST_PARSER_FILE_SOURCE_H