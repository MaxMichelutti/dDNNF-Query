#include "ddnnf.h"
#include "args.h"

int main(int argc, char** argv) {
    // init objects
    DDNNFArgs args = DDNNFArgs(argc, argv);
    DDNNF ddnnf = DDNNF();

    // read input
    std::string input_file = args.get_input_file();
    ddnnf_file_format input_format = args.get_input_format();
    switch(input_format){
        case ddnnf_file_format::DDNNF_FILE_TYPE:
            ddnnf.read_ddnnf_file(input_file.c_str());
            break;
        case ddnnf_file_format::C2D_FILE_TYPE:
            ddnnf.read_c2d_file(input_file.c_str());
            break;
        case ddnnf_file_format::D4_FILE_TYPE:
            ddnnf.read_d4_file(input_file.c_str());
            break;
        default:
            std::cerr << "Error: Invalid input format" << std::endl;
            exit(1);
    }

    // perform conditioning if needed
    std::set<int> conditions = args.get_conditions();
    if(conditions.size() > 0){
        ddnnf.condition_all(conditions);
    }

    // write output
    std::string output_file = args.get_output_file();
    ddnnf_file_format output_format = args.get_output_format();
    switch(output_format){
        case ddnnf_file_format::DDNNF_FILE_TYPE:
            ddnnf.serialize(output_file.c_str());
            break;
        case ddnnf_file_format::C2D_FILE_TYPE:
            ddnnf.serialize_c2d(output_file.c_str());
            break;
        case ddnnf_file_format::D4_FILE_TYPE:
            ddnnf.serialize_d4(output_file.c_str());
            break;
        default:
            // do nothing, no output file specified
            break;
    }
    
    // exit
    return 0;
}