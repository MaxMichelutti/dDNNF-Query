#include "args.h"

void print_help(std::string command);

DDNNFArgs::~DDNNFArgs(){
    if(input_file != nullptr){
        delete input_file;
    }
    if(output_file != nullptr){
        delete output_file;
    }
}

DDNNFArgs::DDNNFArgs(int argc, char** argv) {
    input_file = nullptr;
    output_file = nullptr;
    input_format = NONE_TYPE;
    output_format = NONE_TYPE;
    conditions = std::set<int>();
    // check args by matching with all possible values:
    // not the most efficient solution, 
    // but no need to optimize since the amount of options is low
    for (int i = 1; i < argc; i++) {
        std::string current_arg(argv[i]);
        
        // parse -h (or --help)
        if((current_arg == "-h")||(current_arg == "--help")){
            print_help(std::string(argv[0]));
            exit(0);
        }
        // INPUT ARTGS
        // -i
        if(current_arg == "-i"){
            if(input_file != nullptr){
                std::cerr << "Error: Multiple input files specified" << std::endl;
                exit(1);
            }
            if(i+1 >= argc){
                std::cerr << "Error: Missing input file" << std::endl;
                exit(1);
            }
            input_file = new std::string(argv[i+1]);
            input_format = DDNNF_FILE_TYPE;
            i++;
            continue;
        }
        // -i_c2d
        if(current_arg == "-i_c2d"){
            if(input_file != nullptr){
                std::cerr << "Error: Multiple input files specified" << std::endl;
                exit(1);
            }
            if(i+1 >= argc){
                std::cerr << "Error: Missing input file" << std::endl;
                exit(1);
            }
            input_file = new std::string(argv[i+1]);
            input_format = C2D_FILE_TYPE;
            i++;
            continue;
        }
        // -i_d4
        if(current_arg == "-i_d4"){
            if(input_file != nullptr){
                std::cerr << "Error: Multiple input files specified" << std::endl;
                exit(1);
            }
            if(i+1 >= argc){
                std::cerr << "Error: Missing input file" << std::endl;
                exit(1);
            }
            input_file = new std::string(argv[i+1]);
            input_format = D4_FILE_TYPE;
            i++;
            continue;
        }
        // OUTPUT ARGS
        // -o
        if(current_arg == "-o"){
            if(output_file != nullptr){
                std::cerr << "Error: Multiple output files specified" << std::endl;
                exit(1);
            }
            if(i+1 >= argc){
                std::cerr << "Error: Missing output file" << std::endl;
                exit(1);
            }
            output_file = new std::string(argv[i+1]);
            output_format = DDNNF_FILE_TYPE;
            i++;
            continue;
        }
        // -o_c2d
        if(current_arg == "-o_c2d"){
            if(output_file != nullptr){
                std::cerr << "Error: Multiple output files specified" << std::endl;
                exit(1);
            }
            if(i+1 >= argc){
                std::cerr << "Error: Missing output file" << std::endl;
                exit(1);
            }
            output_file = new std::string(argv[i+1]);
            output_format = C2D_FILE_TYPE;
            i++;
            continue;
        }
        // -o_d4
        if(current_arg == "-o_d4"){
            if(output_file != nullptr){
                std::cerr << "Error: Multiple output files specified" << std::endl;
                exit(1);
            }
            if(i+1 >= argc){
                std::cerr << "Error: Missing output file" << std::endl;
                exit(1);
            }
            output_file = new std::string(argv[i+1]);
            output_format = D4_FILE_TYPE;
            i++;
            continue;
        }
        // CONDITIONING ARG
        // -c
        if(current_arg == "-c"){
            i++;
            if(i >= argc){
                std::cerr << "Error: Missing conditioning variables" << std::endl;
                exit(1);
            }
            // read next arg
            std::string next_arg = std::string(argv[i]);
            while(true){
                try{
                    // exception may be raised here
                    int var = std::stoi(next_arg);
                    
                    // check if var or -var is already in conditions
                    if(conditions.find(var) != conditions.end()){
                        std::cerr << "Error: Variable " << var << " is conditioned twice" << std::endl;
                        exit(1);
                    }
                    if(conditions.find(-var) != conditions.end()){
                        std::cerr << "Error: Variable " << -var << " is conditioned both positively and negatively" << std::endl;
                        exit(1);
                    }
                    conditions.insert(var);
                    i++;
                    if(i >= argc){
                        break;
                    }
                }catch(std::invalid_argument& e){
                    if(next_arg[0] == '-'){
                        break;
                    }else{
                        std::cerr << "Error: Invalid conditioning variable " << next_arg << std::endl;
                        exit(1);
                    }
                }
                next_arg = std::string(argv[i]);
            }
            // need to decrement i since the last arg I read was another option if I get here
            i--;
            continue;
        }
        // ERROR INVALID ARGUMENT
        std::cerr << "Error: Invalid option " << current_arg << std::endl;
        std::cerr << "Use -h to get a list of all options" << std::endl;
        exit(1);
    }

    if(input_file == nullptr){
        std::cerr << "Error: Please provide an input file" << std::endl;
        std::cerr << "Use -h to get a list of all options" << std::endl;
        exit(1);
    }
}

void print_help(std::string command){
    std::cout << "DDNNF Conditioning Tool" << std::endl;
    std::cout << "!!! If this message appears, no activity is performed during this call of the tool !!!" << std::endl;
    std::cout << "Usage: "<<command<<" [options]" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "-h\t\t\tShow this help message" << std::endl;
    std::cout << "INPUT OPTIONS:" << std::endl;
    std::cout << "-i <input_file>\tSpecify input file, input is expected in library nnf format" << std::endl;
    std::cout << "-i_d4 <input_file>\tSpecify input file, input is expected in c2d nnf format" << std::endl;
    std::cout << "-i_c2d <input_file>\tSpecify input file, input is expected in d4 nnf format" << std::endl;
    std::cout << "OUTPUT OPTIONS:" << std::endl;
    std::cout << "-o <output_file>\tSpecify output file, output will be saved in library nnf format" << std::endl;
    std::cout << "-o_c2d <output_file>\tSpecify output file, output will be saved in c2d nnf format" << std::endl;
    std::cout << "-o_d4 <output_file>\tSpecify output file, output will be saved in d4 nnf format" << std::endl;
    std::cout << "CONDITIONING OPTION:" << std::endl;
    std::cout << "-c <var1> <var2> ... <varN>\tSpecify variables to condition" << std::endl;
}

std::string DDNNFArgs::get_input_file()const{
    return *input_file;
}

bool DDNNFArgs::has_output_file()const{
    return output_file != nullptr;
}

std::string DDNNFArgs::get_output_file()const{
    if(has_output_file()){
        return *output_file;
    }
    // return empty string as default
    return std::string("");
}

ddnnf_file_format DDNNFArgs::get_input_format()const{
    return input_format;
}   

ddnnf_file_format DDNNFArgs::get_output_format()const{
    return output_format;
}

std::set<int> DDNNFArgs::get_conditions()const{
    return std::set<int>(conditions);
}
