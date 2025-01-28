#ifndef __DDNNF_H__
#define __DDNNF_H__

#include <vector>
#include <map>
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <stack>
#include <set>
#include <cmath>
#include <queue>
#include <string>

enum ddnnf_node_type {
    DDNNF_AND,
    DDNNF_OR,
    DDNNF_LITERAL,
    DDNNF_TRUE,
    DDNNF_FALSE
};

enum file_format {
    C2D_FILE,
    D4_FILE,
    DDNNF_FILE
};

// const long MC_TRUE = -1;
// const long MC_FALSE = -2;
// const long MC_UNKNOWN = -3;

class DDNNF;

// class MCMemoItem{
//     public:
//     std::set<int> vars;
//     long model_count;
//     MCMemoItem();
// };

class DDNNFNode {
    private:
    int id; // node id
    std::set<int> children; // node ids of children
    std::set<int> parents; // node ids of parents
    DDNNF* manager; // reference to manager
    ddnnf_node_type type;
    int var; // only used for literals

    public:
    DDNNFNode(int id,DDNNF* manager,ddnnf_node_type type, int var);
    ~DDNNFNode();
    void add_child(int child_id);
    void add_parent(int parent_id);
    bool is_root()const;
    const std::set<int>& get_children()const;
    const std::set<int>& get_parents()const;
    void remove_child(int child_id);
    void remove_parent(int parent_id);
    ddnnf_node_type get_type()const;
    int get_var()const;
    int get_id()const;
    void set_id(int id);
    bool is_literal()const;
    bool is_true()const;
    bool is_false()const;
    void remove_all_children();
    void change_parents_sign();
};

class DDNNF{
    private:
    //Variables
    std::vector<DDNNFNode*> nodes; //maps node ids to node pointers
    int root_id; // root node
    int total_variables; // amount of variables
    std::map<int,int> literals; // maps variable ids to literal node ids
    int true_node_id;
    int false_node_id;
    std::set<int> mentioned_vars;

    //Private Methods
    void prepare_literals(int num_vars);
    int add_node(ddnnf_node_type type, int var); // returns node id
    void add_edge(int parent_id, int child_id);
    //void mc_dfs(int node_id, std::vector<MCMemoItem>& memo,const std::map<int,bool>& vars)const;
    void simplify();
    void simplify_truth_rec(int node_id, std::vector<bool>& visited);
    void remove_unreferenced_nodes();
    void recompute_indexes();
    void recompute_indexes_rec(int node_id, std::vector<bool>& visited, std::vector<DDNNFNode*>& new_nodes_vector, std::map<int,int>& old_to_new_indexes);
    void recompute_mentioned_vars();
    //void enumerate_rec(int node_id, std::set<int>& visited, std::map<int,std::vector<std::set<int>*>>& partial_models,std::vector<int>& parents_left)const;
    void read_file(const char* filename, file_format format);
    void reset();
    //serialization options
    void make_c2d_rec(int node_id, std::vector<bool>& visited);

    public:
    DDNNF();
    ~DDNNF();
    DDNNFNode* get_node(int id);
    int get_literal_id(int var);
    bool is_root(int node_id);
    // reading files
    void read_c2d_file(const char* filename);
    void read_ddnnf_file(const char* filename);
    void read_d4_file(const char* filename);
    // serialization
    void serialize(const char* filename)const;
    void serialize_c2d(const char* filename)const;
    void serialize_d4(const char* filename)const;
    // long model_count(const std::set<int>& vars)const;
    void condition(int var);
    void condition_all(const std::set<int>& vars);
    // cloning
    DDNNF clone()const;
    DDNNF* clone_ptr()const;
    // void enumerate()const;
    long node_count()const;
    long edge_count()const;

};

#endif