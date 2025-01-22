#include "ddnnf.h"

int main(){
    DDNNF ddnnf;
    ddnnf.read_c2d_file("test_input.nnf");

    long mc = ddnnf.model_count({1,2,3,4,11});

    std::cout << "Model count: " << mc << std::endl;

    ddnnf.enumerate();

    ddnnf.condition_all({-2,7,-4,1,-11});

    std::cout << "Conditioned model count: " << ddnnf.model_count({1,2,3,4,7,11}) << std::endl;

    ddnnf.enumerate();

    ddnnf.serialize("test_serialized.nnf");
    return 0;
}