#include "ddnnf.h"
#include "args.h"

// timing operations
#include <chrono>

int main(int argc, char** argv) {
    // init objects
    DDNNFArgs args = DDNNFArgs(argc, argv);
    DDNNF ddnnf = DDNNF();

    // read input
    auto start_time = std::chrono::high_resolution_clock::now();
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
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    std::cout << "Read input in " << duration.count() << " ms" << std::endl;

    // perform conditioning if needed
    start_time = std::chrono::high_resolution_clock::now();
    std::set<int> conditions = args.get_conditions();
    if(conditions.size() > 0){
        ddnnf.condition_all(conditions);
        end_time = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        std::cout << "Performed conditioning in " << duration.count() << " ms" << std::endl;
    }

    // write output
    start_time = std::chrono::high_resolution_clock::now();
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
            return 0;
    }
    end_time = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    std::cout << "Saved output in " << duration.count() << " ms" << std::endl;
    
    // exit
    return 0;
}