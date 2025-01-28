#include "ddnnf.h"

DDNNFNode::DDNNFNode(int id, DDNNF* manager, ddnnf_node_type type, int var) {
    this->id = id;
    this->manager = manager;
    this->type = type;
    children = std::set<int>();
    parents = std::set<int>();
    if (type == DDNNF_LITERAL) {
        this->var = var;
    }else{
        this->var = 0;
    }
}

DDNNFNode::~DDNNFNode() {
    // do nothing:
    // deleting parents/children may
    // result in dangling references
    // so it is better to delegate
    // this task to the manager
}

ddnnf_node_type DDNNFNode::get_type()const{return type;}

int DDNNFNode::get_var()const{return var;}

int DDNNFNode::get_id()const{return id;}
void DDNNFNode::set_id(int id){this->id = id;}

bool DDNNFNode::is_literal()const{return type==DDNNF_LITERAL;}
bool DDNNFNode::is_true()const{return type==DDNNF_TRUE;}
bool DDNNFNode::is_false()const{return type==DDNNF_FALSE;}

void DDNNFNode::add_child(int child_id) {
    if(get_type() == DDNNF_LITERAL){
        std::cerr << "Error: Cannot add children to literal node" << std::endl;
        exit(1);
    }
    if(get_type() == DDNNF_TRUE){
        std::cerr << "Error: Cannot add children to true node" << std::endl;
        exit(1);
    }
    if(get_type() == DDNNF_FALSE){
        std::cerr << "Error: Cannot add children to false node" << std::endl;
        exit(1);
    }

    children.insert(child_id);
}

void DDNNFNode::add_parent(int parent_id) {
    parents.insert(parent_id);
}

bool DDNNFNode::is_root()const{
    return manager->is_root(this->id);
}

const std::set<int>& DDNNFNode::get_children()const{
    return children;
}
const std::set<int>& DDNNFNode::get_parents()const{
    return parents;
}
void DDNNFNode::change_parents_sign(){
    std::set<int> new_parents = std::set<int>();
    for(auto parent: parents){
        new_parents.insert(-parent);
    }
    parents.clear();
    for(auto parent: new_parents){
        parents.insert(parent);
    }
}
void DDNNFNode::remove_child(int child_id){
    auto child_iter = std::find(children.begin(),children.end(),child_id);
    if(child_iter != children.end()){children.erase(child_id);}
}
void DDNNFNode::remove_all_children(){
    children.clear();
}
void DDNNFNode::remove_parent(int parent_id){
    auto parent_iter = std::find(parents.begin(),parents.end(),parent_id);
    if(parent_iter != parents.end()){parents.erase(parent_id);}
}

DDNNF::DDNNF() {
    nodes = std::vector<DDNNFNode*>();
    literals = std::map<int, int>();
    mentioned_vars = std::set<int>();

    reset();
}

DDNNF::~DDNNF() {
    // delete all non-null nodes
    for(auto node: nodes){
        if(node != nullptr){
            delete node;
        }
    }
}

bool DDNNF::is_root(int node_id) {
    return node_id == root_id;
}

long DDNNF::node_count()const{
    return nodes.size();
}

long DDNNF::edge_count()const{
    int total_edges = 0;
    for(auto item: nodes){
        total_edges += item->get_children().size();
    }
    return total_edges;

}

int DDNNF::add_node(ddnnf_node_type type, int var) {
    int id = nodes.size();
    if (type == DDNNF_TRUE) {
        // check there is not a true node yet
        if (true_node_id != -1) {
            std::cerr << "Error: Multiple true nodes" << std::endl;
            exit(1);
        }
        // update true node id
        true_node_id = id;
    } else if (type == DDNNF_FALSE) {
        // check there is not a false node yet
        if (false_node_id != -1) {
            std::cerr << "Error: Multiple false nodes" << std::endl;
            exit(1);
        }
        // update false node id
        false_node_id = id;
    } else if(type == DDNNF_LITERAL){
        // check var index is valid
        if (literals.find(var) == literals.end()) {
            std::cerr << "Error: Invalid literal" << std::endl;
            exit(1);
        }
        // check there is not a literal for the same variable index
        if (literals[var] != -1) {
            std::cerr << "Error: Multiple literals for the same variable" << std::endl;
            exit(1);
        }
        // update literals map
        literals[var] = id;
    }
    // create actual node and add it to the nodes map
    DDNNFNode* node = new DDNNFNode(id, this, type, var);
    nodes.push_back(node);
    return id;
}

void DDNNF::add_edge(int parent_id, int child_id) {
    if ((child_id < 0) || (child_id >= nodes.size()) || (parent_id < 0) || (parent_id >= nodes.size())) {
        std::cerr << "ADD EDGE Error: Invalid node id" << std::endl;
        exit(1);
    }
    nodes[parent_id]->add_child(child_id);
    nodes[child_id]->add_parent(parent_id);
}

DDNNFNode* DDNNF::get_node(int id) {
    if ((id < 0) || (id >= nodes.size())) {
        return nullptr;
    }
    return nodes[id];
}

int DDNNF::get_literal_id(int var) {
    if (literals.find(var) == literals.end()) {
        return -1;
    }
    return literals[var];
}

void DDNNF::reset(){
    // delete all non-null nodes
    for(auto node: nodes){
        if(node != nullptr){
            delete node;
        }
    }
    nodes.clear();
    literals.clear();
    mentioned_vars.clear();
    
    root_id = -1;
    true_node_id = -1;
    false_node_id = -1;
    total_variables = 0;
}

void DDNNF::prepare_literals(int num_vars) {
    // associates to all variables a pointer to null
    for (int i = 1; i <= num_vars; i++) {
        // check if key i exists
        if (literals.find(i) == literals.end()) {
            literals[i] = -1;
        }
        if (literals.find(-i) == literals.end()) {
            literals[-i] = -1;
        }
    }
}

void print_c2d_error(){
    std::cerr << "Error: File is not in c2d nnf format" << std::endl;
    exit(1);
}

void print_d4_error(){
    std::cerr << "Error: File is not in d4 nnf format" << std::endl;
    exit(1);
}

bool is_digit(char c){
    return (c >= '0') && (c <= '9');
}

void DDNNF::read_c2d_file(const char* filename){
    read_file(filename,C2D_FILE);
}

void DDNNF::read_ddnnf_file(const char* filename){
    read_file(filename,DDNNF_FILE);
}

void DDNNF::read_d4_file(const char* filename){
    reset();
    std::ifstream infile;

    // check if file exists
    infile.open(filename);
    if (!infile) {
        std::cerr << "Error: Unable to open file " << filename << std::endl;
        exit(1);
    }

    std::string line;
    std::string token;
    bool found_nodes = false;
    int max_literal = 0;
    int max_defined_node = 0;
    bool defining_edges = false;
    // add buffer 0-th AND node
    // since D4 format starts at node index 1
    add_node(DDNNF_AND,0);
    while (std::getline(infile,line)){
        std::istringstream iss(line);
        // skip empty lines
        if(!(iss >> token)){continue;}

        // check if current line describes a node
        char node_type = token[0];
        switch (node_type)
        {
            case 'a':{
                if(defining_edges){print_d4_error();}
                found_nodes = true;
                add_node(DDNNF_AND,0);
                max_defined_node++;
                continue;
            }break;
            case 'o':{
                if(defining_edges){print_d4_error();}
                found_nodes = true;
                add_node(DDNNF_OR,0);
                max_defined_node++;
                continue;
            }break;
            case 't':{
                if(defining_edges){print_d4_error();}
                found_nodes = true;
                add_node(DDNNF_TRUE,0);
                max_defined_node++;
                continue;
            }break;
            case 'f':{
                if(defining_edges){print_d4_error();}
                found_nodes = true;
                add_node(DDNNF_FALSE,0);
                max_defined_node++;
                continue;
            }break;
            default:{
                // line is edge, should start with a number
                if(!is_digit(node_type)){
                    print_d4_error();
                }
                defining_edges = true;
            }
        }

        // check if current line describes an edge
        int source_id;
        int destination_id;
        try{
            source_id = std::stoi(token);
        }catch(std::invalid_argument& e){print_d4_error();}
        // check source bounds
        if(source_id < 1 || source_id > max_defined_node){print_d4_error();}
        if(!(iss >> token)){print_d4_error();}
        try{
            destination_id = std::stoi(token);
        }catch(std::invalid_argument& e){print_d4_error();}
        // check dst bounds
        if(destination_id < 1 || destination_id > max_defined_node){print_d4_error();}
        // check source and dst are distinct
        if(source_id == destination_id){print_d4_error();}

        // read literals
        std::vector<int> edge_literal_node_ids = std::vector<int>();
        while(iss >> token){
            int literal;
            try{
                literal = std::stoi(token);
            }catch(std::invalid_argument& e){print_d4_error();}
            // if literal is 0, line ends
            if(literal == 0){break;}
            int abs_literal = abs(literal);
            mentioned_vars.insert(abs_literal);
            if(abs_literal > max_literal){
                max_literal = abs_literal;
                prepare_literals(max_literal);
            }
            
            if(literals[literal] == -1){
                // create node for literal if not present
                add_node(DDNNF_LITERAL,literal);
            }
            edge_literal_node_ids.push_back(literals[literal]);
        }

        // complete edge creation
        if(edge_literal_node_ids.size() == 0){
            // add edge
            add_edge(source_id,destination_id);
            continue;
        }else{
            // add edge with intermetiate AND node
            int and_node_id = add_node(DDNNF_AND,0);
            add_edge(source_id,and_node_id);
            add_edge(and_node_id,destination_id);
            // add edges from and node to literals
            for(int literal_node_id: edge_literal_node_ids){
                add_edge(and_node_id,literal_node_id);
            }
        }
    }
    total_variables = max_literal;
    // close stream
    infile.close();

    // check that at least one node was found
    if(!found_nodes){
        std::cerr << "Error: No nodes defined" << std::endl;
        exit(1);
    }

    // find root node
    for(int i = 1; i <= max_defined_node; i++){
        if(nodes[i]->get_parents().size() == 0){
            root_id = i;
            break;
        }
    }
    if(root_id == -1){
        std::cerr << "Error: No root node found" << std::endl;
        exit(1);
    }

    // complete reading by simplifying the formula
    simplify();

}

void DDNNF::read_file(const char* filename, file_format format) {
    if(format == D4_FILE){
        std::cerr << "Error: D4 format not supported on this function" << std::endl;
        exit(1);
    }
    reset();
    
    // init stream
    std::ifstream(infile);

    // check if file exists
    infile.open(filename);
    if (!infile) {
        std::cerr << "Error: Unable to open file " << filename << std::endl;
        exit(1);
    }

    // read line by line
    std::string line;
    bool is_first_line = true;
    // last node parsed is always the root node
    int last_node_id = -1;
    while (std::getline(infile, line)) {
        std::istringstream iss(line);
        std::string token;
        // skip empty lines
        if(!(iss >> token)){continue;}
        if(is_first_line){
            // first line should be "nnf <num_nodes> <num_edges> <num_vars>"
            if(token != "nnf") {print_c2d_error();}
            // check if there are 3 more tokens
            for(int i = 0; i < 3; i++){
                if(!(iss >> token)){print_c2d_error();}
                // last token should have the number of variables
                if (i == 2) {
                    try{
                        int token_value = std::stoi(token);
                        this->prepare_literals(token_value);
                        total_variables = token_value;
                    }catch(std::invalid_argument& e){print_c2d_error();}
                }
            }
            is_first_line = false;
        }else{
            // node lines should be "<node_type> node_data..."
            if(token.length() != 1){print_c2d_error();}
            char node_type = token[0];
            switch(node_type){
                case 'L':{
                    if(!(iss >> token)){print_c2d_error();}
                    try{
                        int var = std::stoi(token);
                        this->mentioned_vars.insert(abs(var));
                        last_node_id = this->add_node(DDNNF_LITERAL, var);
                    }catch(std::invalid_argument& e){print_c2d_error();}
                } break;
                case 'A':{
                    if(!(iss >> token)){print_c2d_error();}
                    try{
                        int count = std::stoi(token);
                        if(count == 0){
                            // TRUE node
                            last_node_id = this->add_node(DDNNF_TRUE, 0);
                        }else{
                            // AND node
                            int node_id = this->add_node(DDNNF_AND, 0);
                            while(count > 0){
                                if(!(iss >> token)){print_c2d_error();}
                                try{
                                    int child_id = std::stoi(token);
                                    this->get_node(node_id)->add_child(child_id);
                                    this->get_node(child_id)->add_parent(node_id);
                                    count--;
                                }catch(std::invalid_argument& e){print_c2d_error();}
                            }
                            last_node_id = node_id;
                        }
                    }catch(std::invalid_argument& e){print_c2d_error();}
                } break;
                case 'O':{
                    //read j (and ignore it)
                    if(!(iss >> token)){print_c2d_error();}
                    if(!(iss >> token)){print_c2d_error();}
                    try{
                        int count = std::stoi(token);
                        if(count == 0){
                            // FALSE node
                            last_node_id = this->add_node(DDNNF_FALSE, 0);
                        }else if(format == C2D_FILE){
                            // OR node
                            if(count != 2){print_c2d_error();}
                        }
                        int node_id = this->add_node(DDNNF_OR, 0);
                        while(count > 0){
                            if(!(iss >> token)){print_c2d_error();}
                            try{
                                int child_id = std::stoi(token);
                                this->get_node(node_id)->add_child(child_id);
                                this->get_node(child_id)->add_parent(node_id);
                                count--;
                            }catch(std::invalid_argument& e){print_c2d_error();}
                        }
                        last_node_id = node_id;
                    }catch(std::invalid_argument& e){print_c2d_error();}
                } break;
                default: print_c2d_error();
            }
        }
    }
    infile.close();

    if (last_node_id == -1) {
        std::cerr << "Error: No nodes found" << std::endl;
        exit(1);
    }
    this->root_id = last_node_id;

    simplify();
}

void DDNNF::serialize(const char * filename)const{
    // uses c2d format, extending OR nodes to allow for more than 2 children
    int total_nodes = node_count();
    int total_vars = this->total_variables;
    int total_edges = edge_count();
    

    std::ofstream out(filename);
    out<<"nnf "<<total_nodes<<" "<<total_edges<<" "<<total_vars<<"\n";
    for(DDNNFNode* node: nodes){
        ddnnf_node_type node_type = node->get_type();
        switch(node_type){
            case DDNNF_AND:{
                const std::set<int>& child_ids = node->get_children();
                out<<"A "<<child_ids.size()<<" ";
                for(int child: child_ids){
                    out<<child<<" ";
                }
                out<<"\n";
            } break;
            case DDNNF_OR:{
                const std::set<int>& child_ids = node->get_children();
                // always print j=0 since I dont store this information
                out<<"O 0 "<<child_ids.size()<<" ";
                for(int child: child_ids){
                    out<<child<<" ";
                }
                out<<"\n";
            } break;
            case DDNNF_FALSE: {
                out<<"O 0 0\n";
            }break;
            case DDNNF_TRUE:{
                out<<"A 0\n";
            } break;
            case DDNNF_LITERAL: {
                out<<"L "<<node->get_var()<<"\n";
            }break;
            default: print_c2d_error();
        }
    }
    out.close();
}

// long DDNNF::model_count(const std::set<int>& vars)const{
//     //check that all vars are positive and inside the range of total_variables
//     for(int var: vars){
//         if(var <= 0){
//             std::cerr << "Error: Non positive variable provided for MC" << std::endl;
//             exit(1);
//         }
//         if(var > total_variables){
//             std::cerr << "Error: Variable index out of range" << std::endl;
//             exit(1);
//         }
//     }

//     // compute vars that have to be projected
//     // these vars are all vars that are mentioned in dDNNF
//     // but are not in the interesting vars for model counting
//     std::map<int,bool> projected_vars;
//     for(int var: mentioned_vars){
//         // if a mentioned var does not appear in vars,
//         // add it to projevcted_vars vector,
//         // paired with a false constant
//         if(vars.find(var) == vars.end()){
//             projected_vars.insert(std::make_pair(var,false));
//         }   
//     }

//     if(projected_vars.empty()){
//         std::vector<MCMemoItem> memo = std::vector<MCMemoItem>(nodes.size());
//         mc_dfs(root_id, memo, projected_vars);

//         // if root result is mc false, then there are no models
//         long result = memo[root_id].model_count;
//         if(result == MC_FALSE){
//             return 0;
//         }
//         // if root result is mc true, then there are 2^|vars| models
//         if(result == MC_TRUE){
//             // 2^|vars|
//             return (1 << vars.size());
//         }
//         // otherwise
//         // count the number of variables that are in vars
//         // and do not appear in memo[root_id].vars
//         int vars_size = vars.size();
//         int root_vars_size = memo[root_id].vars.size();
//         int var_diff = vars_size - root_vars_size;

//         // result is the model count of the root node times 2^var_diff
//         return result * (1 << var_diff);
//     }else{
//         std::cout<<"Projected Model counting cannot be computed in polytime on dDNNF"<<std::endl;
//         exit(1);
//     }
// }

// MCMemoItem::MCMemoItem(){
//     vars = std::set<int>();
//     model_count = MC_UNKNOWN;
// }

// void DDNNF::mc_dfs(int node_id, std::vector<MCMemoItem>& memo, const std::map<int,bool>& proj_vars)const{
//     std::set<int> childs = nodes[node_id]->get_children();
//     for(int child: childs){
//         // if not visited
//         if(memo[child].model_count == MC_UNKNOWN){
//             mc_dfs(child,memo,proj_vars);
//         }
//     }
//     switch(nodes[node_id]->get_type()){
//         case DDNNF_TRUE:{
//             memo[node_id].model_count = MC_TRUE;
//         }break;
//         case DDNNF_FALSE:{
//             memo[node_id].model_count = MC_FALSE;
//         }break;
//         case DDNNF_LITERAL:{
//             int var = nodes[node_id] -> get_var();
//             // check if var is in proj_vars
//             if (proj_vars.find(abs(var)) != proj_vars.end()) { // proj_vars has key for var
//                 bool truth = proj_vars.at(abs(var));
//                 // if var is negated, change truth of node
//                 if(var<0){truth = ! truth;}
//                 if(truth){ // proj_var is assigned true
//                     memo[node_id].model_count = MC_TRUE;
//                 }else{// proj_var is assigned false
//                     memo[node_id].model_count = MC_FALSE;
//                 }
//             }else{ // var is not projected
//                 memo[node_id].vars.insert(abs(nodes[node_id]->get_var()));
//                 memo[node_id].model_count = 1;
//             }
//         }break;
//         case DDNNF_OR:{
//             for(int child: childs){
//                 // insert all vars from each child
//                 memo[node_id].vars.insert(memo[child].vars.begin(),memo[child].vars.end());

//                 // handle edge case where OR contains a 
//                 // true child or an ignored literal
//                 if (memo[child].model_count == MC_TRUE) {
//                     memo[node_id].model_count = MC_TRUE;
//                     memo[node_id].vars.clear();
//                     return;
//                 }
//             }
//             // amount of models is the sum of models from each child
//             int total_models = 0;
//             int mentioned_vars = memo[node_id].vars.size();
//             for(int child: childs){
//                 // skip children that are false since they dont affect mc of or node
//                 if(memo[child].model_count == MC_FALSE){continue;}

//                 // models of a child are its total_models times 2^x
//                 // where x is the number of variables that the child does not mention
//                 int child_mentioned_vars = memo[child].vars.size();
//                 int var_diff = mentioned_vars - child_mentioned_vars;
//                 // (1 << var_diff) = 2^var_diff due to binary representation of numbers
//                 // notice that var_diff is always non negative
//                 int child_models = memo[child].model_count * (1 << var_diff);
//                 total_models += child_models;
//             }
//             // if only false children mc is mc false
//             if (total_models == 0) {
//                 memo[node_id].model_count = MC_FALSE;
//                 memo[node_id].vars.clear();
//                 return;
//             }
//             memo[node_id].model_count = total_models;
//         }break;
//         case DDNNF_AND:{
//             for(int child: childs){
//                 // insert all vars from child in vars of node
//                 memo[node_id].vars.insert(memo[child].vars.begin(),memo[child].vars.end());

//                 // handle edge case where AND contains a false child
//                 if (memo[child].model_count == MC_FALSE) {
//                     memo[node_id].model_count = MC_FALSE;
//                     memo[node_id].vars.clear();
//                     return;
//                 }
//             }
//             // amount of models is the product of models from each child
//             int total_models = 1;
//             bool found_non_true_child = false;
//             for(int child: childs){
//                 // skip children that are true since they dont affect mc of and node
//                 if(memo[child].model_count == MC_TRUE){continue;}
//                 found_non_true_child = true;
//                 total_models *= memo[child].model_count;
//             }
//             // if only true children, mc is mc true
//             if (!found_non_true_child) {
//                 memo[node_id].model_count = MC_TRUE;
//                 memo[node_id].vars.clear();
//                 return;
//             }
//             memo[node_id].model_count = total_models;
//         }break;        
//     }
// }

void DDNNF::condition_all(const std::set<int>& vars){
    // check input
    for(int var: vars){
        if(vars.find(-var) != vars.end()){
            std::cerr << "Error: Cannot condition on both a variable and its negation" << std::endl;
            exit(1);
        }
    }
    // condition on all vars one at a time
    for(int var: vars){
        condition(var);
    }
}

void DDNNF::condition(int var){
    if(var == 0){
        std::cerr << "Error: Cannot condition on 0" << std::endl;
        exit(1);
    }
    if(literals.find(var) == literals.end()){
        std::cerr << "Error: Invalid literal to condition" << std::endl;
        exit(1);
    }
    if(true_node_id == -1){
        add_node(DDNNF_TRUE,0);
    }
    if(false_node_id == -1){
        add_node(DDNNF_FALSE,0);
    }
    for(auto node: nodes){
        if(! node->is_literal()){continue;}
        if(node->get_var() == var){
            literals[var] = -1;
            for(auto parent: node->get_parents()){
                nodes[parent]->remove_child(node->get_id());
                add_edge(parent,true_node_id);
            }
            DDNNFNode* old_node = nodes[node->get_id()];
            nodes[old_node->get_id()] = nullptr;
            delete old_node;
        }else if(node->get_var() == -var){
            literals[-var] = -1;
            for(auto parent: node->get_parents()){
                nodes[parent]->remove_child(node->get_id());
                add_edge(parent,false_node_id);
            }
            DDNNFNode* old_node = nodes[node->get_id()];
            nodes[old_node->get_id()] = nullptr;
            delete old_node;
        }
    }
    // create new node for literal var
    int new_node_id = add_node(DDNNF_LITERAL,var);
    // create AND node between root and new node
    int and_node_id = add_node(DDNNF_AND,0);
    add_edge(and_node_id,new_node_id);
    add_edge(and_node_id,root_id);
    // update root
    root_id = and_node_id;
    // simplify conditioned formula
    simplify();
}

void DDNNF::simplify(){
    // temporarely add a true and false node if they are not present yet
    if(true_node_id == -1){
        add_node(DDNNF_TRUE,0);
    }
    if(false_node_id == -1){
        add_node(DDNNF_FALSE,0);
    }

    // simplify boolean constants in the DDNNF
    std::vector<bool> visited = std::vector<bool>(nodes.size(),false);
    simplify_truth_rec(root_id,visited);
    
    // remove all nodes that 
    // don't have parents
    // and are not root
    remove_unreferenced_nodes();

    // indexes of nodes have to change
    // to delete all gaps from removed
    // nodes
    recompute_indexes();

    // some vars may disappear after simplification
    recompute_mentioned_vars();
}

void DDNNF::recompute_mentioned_vars(){
    mentioned_vars = std::set<int>();
    for(auto node: nodes){
        if(node->get_type() == DDNNF_LITERAL){
            mentioned_vars.insert(abs(node->get_var()));
        }
    }
}

void DDNNF::recompute_indexes(){
    std::vector<bool> visited = std::vector<bool>(nodes.size(),false);
    std::vector<DDNNFNode*> new_nodes_vector = std::vector<DDNNFNode*>(); 
    std::map<int,int> old_to_new_indexes = std::map<int,int>();
    recompute_indexes_rec(root_id,visited,new_nodes_vector,old_to_new_indexes);
    // parents are all negative indexes after the recursive function call
    for(auto node: nodes){
        if(node != nullptr){
            node->change_parents_sign();
        }
    }
    nodes.clear();
    for(DDNNFNode* node: new_nodes_vector){
        nodes.push_back(node);
    }
    // root is always last node visited in DFS
    root_id = nodes.size() - 1;
}

void DDNNF::recompute_indexes_rec(int node_id, std::vector<bool>& visited, std::vector<DDNNFNode*>& new_nodes_vector, std::map<int,int>& old_to_new_indexes){
    // skip visited nodes
    if(visited[node_id]){return;}
    // mark current node as visited
    visited[node_id] = true;
    // get children of current node
    std::set<int> children = nodes[node_id]->get_children();
    for(auto child: children){
        // recompute indexes of children
        recompute_indexes_rec(child,visited,new_nodes_vector, old_to_new_indexes);
    }
    // recompute node indes
    int old_index = node_id;
    new_nodes_vector.push_back(nodes[node_id]);
    int new_index = new_nodes_vector.size() - 1;

    // update node id
    nodes[node_id]->set_id(new_index);

    // update literal map, false_node_id and true_node_id 
    // if the current node is literal, true or false respectively
    if(nodes[node_id]->is_literal()){
        literals[nodes[node_id]->get_var()] = new_index;
    }
    if(nodes[node_id]->is_true()){
        true_node_id = new_index;
    }
    if(nodes[node_id]->is_false()){
        false_node_id = new_index;
    }

    // update old to new indexes map
    old_to_new_indexes.insert(std::make_pair(old_index,new_index));
    
    // recompute children indexes
    std::set<int> children_to_add = std::set<int>();
    for(auto child_id: children){
        // add children to children to add
        children_to_add.insert(old_to_new_indexes[child_id]);

        // remove old node is from children's parents
        nodes[child_id]->remove_parent(old_index);

        // add parent as -new_index
        // because new_index may already point to 
        // a different node
        nodes[child_id]->add_parent(-new_index);
    }

    // update children of node
    // IMPORTANT!!!: first clear children then add all changes
    nodes[node_id]->remove_all_children();
    for(auto child: children_to_add){
        nodes[node_id]->add_child(child);
    }
}

void DDNNF::remove_unreferenced_nodes(){
    // find all non-root nodes that do not have parents
    // these nodes are the roots of unreferenced sub-DAGs
    // remove in BFS (from root to leaves) these sub-DAGs

    // FIND roots
    std::queue<int> unreferenced_node_ids = std::queue<int>();
    for(auto node: nodes){
        // if a node is nullptr, all regferences to it
        // must already have been removed
        if(node == nullptr){continue;}
        // root node is the only unreferenced node
        // that we have to keep
        if(node->is_root()){continue;}
        // node is referenced by some parent
        if(node->get_parents().size() != 0){continue;}
        // node is not referenced by any parent, 
        // add it to removal queue
        unreferenced_node_ids.push(node->get_id());
    }

    // BFS
    while(!unreferenced_node_ids.empty()){
        int node_to_delete_id = unreferenced_node_ids.front();
        unreferenced_node_ids.pop();
        DDNNFNode* node = nodes[node_to_delete_id];
        // update literals map
        if(node->is_literal()){
            literals[node->get_var()] = -1;
        }
        // update true and false node ids
        if(node->is_true()){true_node_id = -1;}
        if(node->is_false()){false_node_id = -1;}

        // remove node from all children parents
        for(auto child: node->get_children()){
            nodes[child]->remove_parent(node_to_delete_id);
            // if child has no parents, it is a new
            // unreferenced root, so add it to
            // unreferenced_node_ids
            if(nodes[child]->get_parents().size() == 0){
                unreferenced_node_ids.push(child);
            }
        }
        // now its safe to delete node
        nodes[node_to_delete_id] = nullptr;
        delete node;
    }
}

void DDNNF::simplify_truth_rec(int node_id, std::vector<bool>& visited){
    // skip visited nodes
    if(visited[node_id]){return;}
    // mark current node as visited
    visited[node_id] = true;
    // get children of current node
    std::set<int> children = nodes[node_id]->get_children();
    for(auto child: children){
        // simplify children
        simplify_truth_rec(child,visited);
    }

    children = nodes[node_id]->get_children();
    // simplify node
    switch(nodes[node_id]->get_type()){
        case DDNNF_TRUE: 
        case DDNNF_FALSE: 
        case DDNNF_LITERAL: return;
        case DDNNF_AND:{
            std::set<int> true_children_ids = std::set<int>();
            bool node_is_true = true;
            bool node_is_false = false;
            for(auto child: children){
                // if any child is true, remove child
                if(nodes[child]->is_true()){
                    nodes[child]->remove_parent(node_id);
                    true_children_ids.insert(child);
                }else if(nodes[child]->is_false()){
                    // if any child is false, node is false
                    node_is_false = true;
                    node_is_true = false;
                    nodes[child]->remove_parent(node_id);
                }else{
                    // if any child is not true, node is not true
                    node_is_true = false;
                }
            }
            if(node_is_false){
                // remove node from all parents children
                // and add edge from parent to false node
                for(auto parent: nodes[node_id]->get_parents()){
                    nodes[parent]->remove_child(node_id);
                    add_edge(parent,false_node_id);
                }
                if(nodes[node_id]->is_root()){
                    root_id = false_node_id;
                }
                // have all nodes children forget their parent
                for(auto child: nodes[node_id]->get_children()){
                    nodes[child]->remove_parent(node_id);
                }
                // delete node
                DDNNFNode* old_node = nodes[node_id];
                nodes[node_id] = nullptr;
                delete old_node;
                return;
            }
            if(node_is_true){
                // remove node from all parents children
                // and add edge from parent to true node
                for(auto parent: nodes[node_id]->get_parents()){
                    nodes[parent]->remove_child(node_id);
                    add_edge(parent,true_node_id);
                }
                if(nodes[node_id]->is_root()){
                    root_id = true_node_id;
                }
                // have all nodes children forget their parent
                for(auto child: nodes[node_id]->get_children()){
                    nodes[child]->remove_parent(node_id);
                }
                // delete node
                DDNNFNode* old_node = nodes[node_id];
                nodes[node_id] = nullptr;
                delete old_node;
                return;
            }
            // remove true children
            for(auto true_child: true_children_ids){
                nodes[node_id]->remove_child(true_child);
            }
            if(nodes[node_id]->get_children().size() == 1){
                // node is not necessary
                int non_true_child = *nodes[node_id]->get_children().begin();
                nodes[non_true_child]->remove_parent(node_id); 
                for(auto parent: nodes[node_id]->get_parents()){
                    nodes[parent]->remove_child(node_id);
                    add_edge(parent,non_true_child);
                }
                if(nodes[node_id]->is_root()){
                    root_id = non_true_child;
                }
                DDNNFNode* old_node = nodes[node_id];
                nodes[node_id] = nullptr;
                delete old_node;
                return;
            }
            // if any child is AND, merge child with node
            children = nodes[node_id]->get_children();
            for(auto child: children){
                if(nodes[child]->get_type() == DDNNF_AND){
                    // remove edge from node to child
                    nodes[node_id]->remove_child(child);
                    // add children of child to node
                    for(auto grandchild: nodes[child]->get_children()){
                        add_edge(node_id,grandchild);
                        nodes[grandchild]->remove_parent(child);
                    }
                    // remove child
                    DDNNFNode* old_child = nodes[child];
                    nodes[child] = nullptr;
                    delete old_child;
                }
            }
        }break;
        case DDNNF_OR:{
            std::set<int> false_children_ids = std::set<int>();
            bool node_is_true = false;
            bool node_is_false = true;
            for(auto child: children){
                // if any child is false, remove child
                if(nodes[child]->is_false()){
                    nodes[child]->remove_parent(node_id);
                    false_children_ids.insert(child);
                }else if(nodes[child]->is_true()){
                    // if any child is true, node is true
                    nodes[child]->remove_parent(node_id);
                    node_is_true = true;
                    node_is_false = false;
                }else{
                    // if any child is not false, node is not false
                    node_is_false = false;
                }
            }
            if(node_is_true){
                // remove node from all parents children
                // and add edge from parent to true node
                for(auto parent: nodes[node_id]->get_parents()){
                    nodes[parent]->remove_child(node_id);
                    add_edge(parent,true_node_id);
                }
                if(nodes[node_id]->is_root()){
                    root_id = true_node_id;
                }
                // have all nodes children forget their parent
                for(auto child: nodes[node_id]->get_children()){
                    nodes[child]->remove_parent(node_id);
                }
                // delete node
                DDNNFNode* old_node = nodes[node_id];
                nodes[node_id] = nullptr;
                delete old_node;
                return;
            }
            if(node_is_false){
                // remove node from all parents children
                // and add edge from parent to false node
                for(auto parent: nodes[node_id]->get_parents()){
                    nodes[parent]->remove_child(node_id);
                    add_edge(parent,false_node_id);
                }
                if(nodes[node_id]->is_root()){
                    root_id = false_node_id;
                }
                // have all nodes children forget their parent
                for(auto child: nodes[node_id]->get_children()){
                    nodes[child]->remove_parent(node_id);
                }
                // delete node
                DDNNFNode* old_node = nodes[node_id];
                nodes[node_id] = nullptr;
                delete old_node;
                return;
            }
            // remove false children
            for(auto false_child: false_children_ids){
                nodes[node_id]->remove_child(false_child);
            }
            if(nodes[node_id]->get_children().size() == 1){
                // node is not necessary
                int non_false_child = *nodes[node_id]->get_children().begin();
                nodes[non_false_child]->remove_parent(node_id); 
                for(auto parent: nodes[node_id]->get_parents()){
                    nodes[parent]->remove_child(node_id);
                    add_edge(parent,non_false_child);
                }
                if(nodes[node_id]->is_root()){
                    root_id = non_false_child;
                }
                DDNNFNode* old_node = nodes[node_id];
                nodes[node_id] = nullptr;
                delete old_node;
                return;
            }
            // if any child is OR, merge child with node
            children = nodes[node_id]->get_children();
            for(auto child: children){
                if(nodes[child]->get_type() == DDNNF_OR){
                    // remove edge from node to child
                    nodes[node_id]->remove_child(child);
                    // add children of child to node
                    for(auto grandchild: nodes[child]->get_children()){
                        add_edge(node_id,grandchild);
                        nodes[grandchild]->remove_parent(child);
                    }
                    // remove child
                    DDNNFNode* old_child = nodes[child];
                    nodes[child] = nullptr;
                    delete old_child;
                }
            }
        }break;
    }
}

DDNNF DDNNF::clone()const{
    DDNNF new_ddnnf = DDNNF();
    new_ddnnf.nodes = std::vector<DDNNFNode*>();
    new_ddnnf.literals = std::map<int,int>();
    new_ddnnf.mentioned_vars = std::set<int>();
    new_ddnnf.true_node_id = true_node_id;
    new_ddnnf.false_node_id = false_node_id;
    new_ddnnf.root_id = root_id;
    new_ddnnf.total_variables = total_variables;
    new_ddnnf.prepare_literals(total_variables);
    // clone nodes
    for(auto node: nodes){
        new_ddnnf.nodes.push_back(new DDNNFNode(node->get_id(),&new_ddnnf,node->get_type(),node->get_var()));
        for(auto child: node->get_children()){
            new_ddnnf.nodes[node->get_id()]->add_child(child);
        }
        for(auto parent: node->get_parents()){
            new_ddnnf.nodes[node->get_id()]->add_parent(parent);
        }
        if(node->is_literal()){
            new_ddnnf.literals[node->get_var()] = node->get_id();
            new_ddnnf.mentioned_vars.insert(abs(node->get_var()));
        }
    }
    // clone mentioned vars
    for(auto var: mentioned_vars){
        new_ddnnf.mentioned_vars.insert(var);
    }
    return new_ddnnf;
}


DDNNF* DDNNF::clone_ptr()const{
    DDNNF* new_ddnnf = new DDNNF();
    new_ddnnf->nodes = std::vector<DDNNFNode*>();
    new_ddnnf->literals = std::map<int,int>();
    new_ddnnf->mentioned_vars = std::set<int>();
    new_ddnnf->true_node_id = true_node_id;
    new_ddnnf->false_node_id = false_node_id;
    new_ddnnf->root_id = root_id;
    new_ddnnf->total_variables = total_variables;
    new_ddnnf->prepare_literals(total_variables);
    // clone nodes
    for(auto node: nodes){
        new_ddnnf->nodes.push_back(new DDNNFNode(node->get_id(),new_ddnnf,node->get_type(),node->get_var()));
        for(auto child: node->get_children()){
            new_ddnnf->nodes[node->get_id()]->add_child(child);
        }
        for(auto parent: node->get_parents()){
            new_ddnnf->nodes[node->get_id()]->add_parent(parent);
        }
        if(node->is_literal()){
            new_ddnnf->literals[node->get_var()] = node->get_id();
            new_ddnnf->mentioned_vars.insert(abs(node->get_var()));
        }
    }
    // clone mentioned vars
    for(auto var: mentioned_vars){
        new_ddnnf->mentioned_vars.insert(var);
    }
    return new_ddnnf;
}

// void DDNNF::enumerate()const{
//     std::set<int> visited = std::set<int>();
//     std::map<int,std::vector<std::set<int>*>> partial_models = std::map<int,std::vector<std::set<int>*>>();
//     std::vector<int> parents_left = std::vector<int>(nodes.size(),0);
//     for(auto node: nodes){
//         partial_models.insert(std::make_pair(node->get_id(),std::vector<std::set<int>*>()));
//         parents_left[node->get_id()] = node->get_parents().size();
//     }
//     enumerate_rec(root_id,visited,partial_models, parents_left);
//     for(auto total_model : partial_models[root_id]){
//         for(auto var: *total_model){
//             std::cout<<var<<" ";
//         }
//         std::cout<<std::endl;
//     }
//     // free memory of root
//     for(auto model: partial_models[root_id]){
//         delete model;
//     }
//     partial_models.erase(root_id);
// }

// void DDNNF::enumerate_rec(
//     int node_id, 
//     std::set<int>& visited, 
//     std::map<int,std::vector<std::set<int>*>>& partial_models, 
//     std::vector<int>& parents_left)const{
//     // skip visited nodes
//     if(visited.find(node_id) != visited.end()){return;}
//     // mark current node as visited
//     visited.insert(node_id);

//     // enum children
//     std::set<int> children = nodes[node_id]->get_children();
//     for(auto child: children){
//         enumerate_rec(child,visited,partial_models, parents_left);
//     }

//     switch (nodes[node_id]->get_type()){
//         case DDNNF_TRUE:{
//             // add the empty set to partial models
//             partial_models[node_id].push_back(new std::set<int>());
//             return;
//         }break;
//         case DDNNF_FALSE:{
//             // no partial models
//             return;
//         }break;
//         case DDNNF_LITERAL:{
//             // add the literal to partial models
//             partial_models[node_id].push_back(new std::set<int>());
//             partial_models[node_id][0]->insert(nodes[node_id]->get_var());
//             return;
//         }break;
//         case DDNNF_OR:{
//             // partial_models are all models of the children
//             for(auto child: children){
//                 for(auto model: partial_models[child]){
//                     partial_models[node_id].push_back(model);
//                 }
//                 // avoid deleting child data immediately
//                 // (by adding a ghost parent):
//                 // child data will be deleted
//                 // anyway when the data of this
//                 // OR node will be deleted
//                 parents_left[child]++;
//             }
//         }break;
//         case DDNNF_AND:{
//             // partial_models are all possible combinations of models of the children
//             std::vector<int> combinations = std::vector<int>();
//             for(auto child: children){
//                 combinations.push_back(partial_models[child].size());
//             }
//             // indexes vector keeps track of which index of each child is being used
//             std::vector<int> indexes = std::vector<int>(children.size(),0);
            
//             // compute total number of possible combinations
//             int total_combinations = 1;
//             for(auto comb: combinations){
//                 total_combinations *= comb;
//             }

//             // for each combination
//             int current_combination = 0;
//             while(current_combination<total_combinations){
//                 // add the combination with current indexes to partial models of this node
//                 std::set<int>* new_model = new std::set<int>();
//                 int comb_index = 0;
//                 for(auto child: children){
//                     for(auto var: *partial_models[child][indexes[comb_index]]){
//                         new_model->insert(var);
//                     }
//                     comb_index++;
//                 }
//                 partial_models[node_id].push_back(new_model);

//                 // update indexes vector
//                 comb_index = 0;
//                 while(comb_index < indexes.size()){
//                     indexes[comb_index]++;
//                     if(indexes[comb_index] == combinations[comb_index]){
//                         indexes[comb_index] = 0;
//                         comb_index++;
//                     }else{break;}
//                 }

//                 // update counter
//                 current_combination++;
//             }
//         }break;
//     }

//     // free memory of children partial models
//     // if no parent for that child is left

//     // This memory delete only works because we
//     // assume that the dDNNF is simplified

//     // IMPORTANT!!! NEVER delete data of an OR
//     // node directly! Always delete on its children!
//     for(auto child_id: children){
//         parents_left[child_id]--;
//         if(parents_left[child_id] == 0){
//             // if node is not OR, delete node's data safely
//             if(nodes[child_id]->get_type() != DDNNF_OR){
//                 for(auto model: partial_models[child_id]){
//                     delete model;
//                 }
//                 partial_models.erase(child_id);
//             }else{
//                 // if node is OR node, remove one parent from grandchildren
//                 // if grandchild parents is zero remove safely

//                 // since I assume dDNNF is simplified, than no OR node has
//                 // an OR node child
//                 for(auto grandchild: nodes[child_id]->get_children()){
//                     parents_left[grandchild]--;
//                     if(parents_left[grandchild] == 0){
//                         for(auto model: partial_models[grandchild]){
//                             delete model;
//                         }
//                         partial_models.erase(grandchild);
//                     }
//                 }
//             }
//         }
//     }
// }

void DDNNF::serialize_c2d(const char * filename)const{
    // clone ddnnf and modify clone to be c2d serializable
    DDNNF cloned_ddnnf = clone();
    std::vector<bool> visited = std::vector<bool>(cloned_ddnnf.nodes.size(),false);
    cloned_ddnnf.make_c2d_rec(cloned_ddnnf.root_id, visited);
    cloned_ddnnf.recompute_indexes();
    cloned_ddnnf.recompute_mentioned_vars();
    // now cloned ddnnf can be serialized as a normal ddnnf
    cloned_ddnnf.serialize(filename);
}

void DDNNF::make_c2d_rec(int node_id, std::vector<bool>& visited){
    if(visited[node_id]){return;}
    visited[node_id] = true;
    std::set<int> children = nodes[node_id]->get_children();
    for(auto child: children){
        make_c2d_rec(child,visited);
    }
    switch(nodes[node_id]->get_type()){
        case DDNNF_OR:{
            // split OR node into multiple OR nodes
            // with only two children
            if(children.size() <= 2){return;}
            std::queue<int> children_id_queue = std::queue<int>();
            for(auto child: children){
                children_id_queue.push(child);
            }
            // IDEA: all nodes in queue are always children of node_id
            // at each iteration take firtst 2 nodes and remove
            // the edge from node_id to them,
            // then create a new OR node with these two nodes as children
            // and add an edge between the new OR node and node_id
            // then push the new OR node in the back of the queue
            // repeat until queue has only 2 nodes (the 2 allowed children)
            // this should result in a balanced tree of OR nodes
            while(children_id_queue.size() > 2){
                int first = children_id_queue.front();
                children_id_queue.pop();
                int second = children_id_queue.front();
                children_id_queue.pop();

                // remove old edges
                nodes[node_id]->remove_child(first);
                nodes[first]->remove_parent(node_id);
                nodes[node_id]->remove_child(second);
                nodes[second]->remove_parent(node_id);

                // create OR node
                int new_node_id = add_node(DDNNF_OR,0);

                // add new edges
                add_edge(new_node_id,first);
                add_edge(new_node_id,second);
                add_edge(node_id,new_node_id);
                
                // push node back into queue
                children_id_queue.push(new_node_id);
            }
        }break;
        default:return;
    }
}

void DDNNF::serialize_d4(const char * filename)const{
    std::ofstream out(filename);
    
    // a simplified ddnnf has true or false only as root
    if(false_node_id != -1){
        out<<"f 1 0"<<std::endl;
        out.close();
        return;
    }
    if(true_node_id != -1){
        out<<"t 1 0"<<std::endl;
        out.close();
        return;
    }

    // print nodes with ids shifted by 1
    int count = 1;
    std::map<int,int> node_id_to_d4_id = std::map<int,int>();
    for(auto iter=nodes.rbegin(); iter!=nodes.rend(); ++iter){
        auto node = *iter;
        node_id_to_d4_id[node->get_id()] = count;
        // root will have index 1 always
        switch(node->get_type()){
            // skip serializing root
            case DDNNF_AND:{
                out<<"a "<<count<<" 0"<<std::endl;
            }break;
            case DDNNF_OR:{
                out<<"o "<<count<<" 0"<<std::endl;
            }break;
            case DDNNF_LITERAL:{
                // wrap literal L in OR(L)
                out<<"o "<<count<<" 0"<<std::endl;
            }break;
            default:{
                // this should be unreachable
            }break;
        }
        count++;
    }
    int fake_true_node_id = count;

    // print edges
    for(DDNNFNode* node: nodes){
        int node_d4_id = node_id_to_d4_id[node->get_id()];
        if(node->is_literal()){
            // fake sending literal to true and add literal id
            out<<node_d4_id<<" "<<fake_true_node_id<<" "<<node->get_var()<<" 0"<<std::endl;
        }else{
            for(auto child: node->get_children()){
                int child_d4_id = node_id_to_d4_id[child];
                out<<node_d4_id<<" "<<child_d4_id<<" 0"<<std::endl;
            }
        }
    }

    // close stream
    out.close();
}
