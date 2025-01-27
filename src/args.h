#ifndef __ARGS_DDNNF_H__
#define __ARGS_DDNNF_H__
#include <string>
#include <set>
#include <iostream>

enum ddnnf_file_format {
    C2D_FILE_TYPE,
    D4_FILE_TYPE,
    DDNNF_FILE_TYPE,
    NONE_TYPE
};

class DDNNFArgs{
    private:
    std::string* input_file;
    std::string* output_file;
    ddnnf_file_format input_format;
    ddnnf_file_format output_format;
    std::set<int> conditions;
    public:
    DDNNFArgs(int argc, char** argv);
    ~DDNNFArgs();
    std::string get_input_file()const;
    std::string get_output_file()const;
    bool has_output_file()const;
    ddnnf_file_format get_input_format()const;
    ddnnf_file_format get_output_format()const;
    std::set<int> get_conditions()const;
};


#endif