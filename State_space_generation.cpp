#include<iostream>
#include <list>
#include <vector>
#include <map>
#include <fstream>
#include <algorithm> 
#include <string>

using namespace std;

class Graph
{
    int num_of_nodes;												 // No. of vertices
	int count;
	public:
        Graph(int num_of_nodes);    //Constructor
        bool petri;
        list<int> *adj;
        list<int> all_states;
        vector<string> state_space;
        map<int, list<vector<int> > > states_space_by_node;
        map<int, int> vertices;
        map<string, string> parents;
        map<int, bool> task_child;
        map<string, string> input_trans;
        map<string, list<string> > output_trans;
        map<int, int> vertices_index;
        map<int, list<int> > reachable_nodes;
		void addEdge(int v, int w);		// to add an edge to graph
        list<int> getReachableStates(int id);
        void getStateSpace(int v, bool* visited_tasks);
        bool detectCycle();
        bool detectCycleRec(int v, bool visited[], bool *recStack);
        void getTaskPrec(int arr[], int n, int r, int index, int data[], int i, int node); 
        void generate_petri_net(string petri_net_file);
};

Graph::Graph(int nodes) {
    num_of_nodes = nodes;
    count = 0;
    petri = false;
	adj = new list<int>[num_of_nodes];
}

void Graph::addEdge(int v, int w)
{
    map<int,int>::iterator it;
    pair<map<int,int>::iterator,bool> ret;
    int first_node, second_node = 0;
    string first_place, second_place = "";
    ret = vertices.insert(pair<int, int>(v, count));
    if (ret.second) {
        first_node = count;
        vertices_index.insert(pair<int, int>(count, v));
        count++;
    } else {
        it = vertices.find(v);
        first_node = it->second;
    }
    ret = vertices.insert(pair<int, int>(w, count));
    if (ret.second) {
        second_node = count;
        vertices_index.insert(pair<int, int>(count, w));
        count++;
    } else {
        it = vertices.find(w);
        second_node = it->second;
    }
    adj[first_node].push_back(second_node);
    task_child[v] = true;

    if (petri) {
        first_place = 'p' + to_string(first_node+1);
        second_place = 'p' + to_string(second_node+1);
        string trans = 't' + to_string(first_node+1);

        map<string, list<string> >::iterator oi;
        for ( oi = output_trans.begin(); oi != output_trans.end(); oi++) {
            list<string>::iterator ii;
            if (find(begin(oi->second), end(oi->second), second_place) != end(oi->second)) {
                trans = oi->first;
                break;
            }
        }
        if (find(begin(output_trans[trans]), end(output_trans[trans]), second_place) == end(output_trans[trans])) {
            output_trans[trans].push_back(second_place);
        }
        input_trans.insert(pair<string, string>(first_place, trans));
        parents.insert(pair<string, string>(second_place, first_place));
    }
}

bool Graph::detectCycleRec(int task, bool visited[], bool *task_stack) 
{ 
    if(visited[task] == false) 
    { 
        visited[task] = true; // mark the current task as visited
        task_stack[task] = true;  //mark current task as curent recursion
  
        // recur for all states adjacent to this task.
        list<int>::iterator itr; 
        for(itr = adj[task].begin(); itr != adj[task].end(); ++itr) 
        { 
            if ( !visited[*itr] && detectCycleRec(*itr, visited, task_stack) ) 
                return true; 
            else if (task_stack[*itr]) 
                return true; 
        } 
  
    } 
    task_stack[task] = false;  // pop the task from task stack 
    return false; 
} 
  

// Using DFS to detects cycle
bool Graph::detectCycle() 
{
    bool *visited = new bool[num_of_nodes]; //visted tasks
    bool *task_stack = new bool[num_of_nodes];  //to keep tasks not recursed 
    for(int i = 0; i < num_of_nodes; i++) 
    { 
        visited[i] = false; 
        task_stack[i] = false; 
    } 

    for(int i = 0; i < num_of_nodes; i++) 
        if (detectCycleRec(i, visited, task_stack)) 
            return true; 
  
    return false; 
} 

// Using BFS to get all reachable states
list<int> Graph::getReachableStates(int v)
{
    bool *visited = new bool[num_of_nodes];
    for(int i = 0; i < num_of_nodes; i++)
        visited[i] = false;


    list<int> queue;
 
    visited[v] = true;
    queue.push_back(v);

    list<int>::iterator i;
    map<int,int>::iterator it;
    list<int> reachable_states;
    while(!queue.empty())
    {
        v = queue.front();
        queue.pop_front();
 
        for (i = adj[v].begin(); i != adj[v].end(); ++i)
        {
            if (!visited[*i])
            {
                visited[*i] = true;
                it = vertices_index.find(*i);
                reachable_states.push_back(it->second);
                queue.push_back(*i);
            }
        }
    }
    return reachable_states;  
}
  
void Graph::getTaskPrec(int arr[], int n, int r,  
                    int index, int data[], int i, int taskid)  
{  
    if (index == r)  
    {  
        list<int>:: iterator it;
        int pair = 0;
        bool flag = false;
        for (int i = 0; i < r; i++) {
            for (it = reachable_nodes[data[i]].begin(); it != reachable_nodes[data[i]].end(); ++it) {
                if (binary_search(data, data + r, *it)) {
                    pair++;
                    if (pair >= 1) {
                        flag = true;
                        break;
                    }
                }
            }
        }
        
        if(!flag) {
            //states_space_by_node[node].push_back(data);
            string state_str = "{" + to_string(taskid) + ", ";
            for (int j = 0; j < r; j++) 
                state_str += to_string(data[j]) + ", "; 
            state_str.pop_back(); 
            state_str.pop_back();
            state_str += "}";
            state_space.push_back(state_str);
            flag = false;
        }
        return;  
    }  

    if (i >= n)  
        return;  
  
    data[index] = arr[i];  
    getTaskPrec(arr, n, r, index + 1, data, i + 1, taskid);  
    getTaskPrec(arr, n, r, index, data, i+1, taskid);  
}  

void Graph::getStateSpace(int v, bool* visited_tasks) {
    vector<int> unreachable_states;
    list<int>:: iterator it;
    list<int>:: iterator itr;
    int taskid = vertices_index[v];
    if (task_child[taskid]) {
        for (it = all_states.begin(); it != all_states.end(); ++it) {
            itr = find(reachable_nodes[taskid].begin(), reachable_nodes[taskid].end(), *it);
            int cur_task_idx = vertices[*it];
            if(itr == reachable_nodes[taskid].end()) {
                //if (cur_task_idx > v) {
                if (!visited_tasks[cur_task_idx]) {
                    unreachable_states.push_back(*it);
                }
            }
        }
    }

    int n = unreachable_states.size();
    int arr[n];
    if (n == 0) {
        string state_str = "{" + to_string(taskid) + "}";
        state_space.push_back(state_str);
    }
    if (n == 1) {
        int reachable_task = unreachable_states[0];
        string temp_str = "{" + to_string(reachable_task) + ", " + to_string(taskid) + "}";
        if (find(state_space.begin(), state_space.end(), temp_str) == end(state_space)) {
            string state_str = "{" + to_string(taskid) + ", " + to_string(reachable_task) + "}";
            state_space.push_back(state_str);
        }
    }
    copy(unreachable_states.begin(), unreachable_states.end(), arr);
    for (int i = 0; i < n; i++) {
        int data[i];
        getTaskPrec(arr, n, i, 0, data, 0, taskid); 
    }

}

void Graph::generate_petri_net(string petri_net_file) {
    string places = "\tplace ";
    string trans = "\ttrans ";
    string arcs = "\tarcs( \n";
    ofstream pn;
    pn.open(petri_net_file);
    pn << "pn tpg := {" << endl;

    //map<string, string> states_trans(input_trans);

    map<string, string>::iterator t_i;
    for (t_i = input_trans.begin(); t_i != input_trans.end(); t_i++) {
        arcs += "\t\t" + t_i->first + " : " + t_i->second + ",\n";
    }

    for (int i = 1; i <= num_of_nodes; i++) {
        string place = "p" + to_string(i);
        places += place + ", ";
        string temp = 't' + to_string(i);
        trans += temp + ", ";
    }

    // for (int i = 1; i <= num_of_nodes; i++) {
    //     string temp = 't' + to_string(i);
    //     string p_t = 'p' + to_string(i);
    //     if (output_trans.find(temp) == output_trans.end()) {
    //         arcs += "\t\t" + temp + " : " + parents[p_t] + ",\n";
    //         arcs += "\t\t" + p_t + " : " + temp + ",\n";
    //         break;
    //     }
    // }

    
    map<string, list<string> >::iterator it;
    for (it = output_trans.begin(); it != output_trans.end(); it++) {
        list<string>::iterator itr;
        for (itr = it->second.begin(); itr != it->second.end(); itr++) {
            arcs += "\t\t" + it->first + " : " + *itr + ",\n";
        }
    }

    places.pop_back();
    places.pop_back();
    places += ";";
    pn << places <<endl;
    pn <<endl;
    pn << "\tinit(p1:1);\n\n";
    trans.pop_back();
    trans.pop_back();
    trans += ";";
    pn << trans <<endl;
    pn << endl;
 
    arcs.pop_back();
    arcs.pop_back();
    pn << arcs << endl;
    pn << "\t);\n";


    
    pn << "\tbigint ns := num_states;" << endl;
    pn << "\tbigint na := num_arcs;" << endl;
    pn << "\tvoid show_rs := show_states(false);" << endl;
    pn << "};" << endl;
    pn << "print(\"THERE ARE \",tpg.ns,\" STATES AND \",tpg.na,\" ARCS\\n\");" << endl;
    pn << "tpg.show_rs;" << endl;
    pn.close();
}

int main(int argc, char* argv[])
{
    string input_file;
    string output_file = "";
    string petri_net_file = "";
    if (argc == 3) {
        input_file = "2.tpg";
        if (strcmp(argv[1], "-s") == 0) {
            output_file = argv[2];
        }
        if (strcmp(argv[1], "-p") == 0) {
            petri_net_file = argv[2];
        }
    }
    if (argc == 4) {
        input_file = argv[1];
        if (strcmp(argv[2], "-s") == 0) {
            output_file = argv[3];
        }
        if (strcmp(argv[2], "-p") == 0) {
            petri_net_file = argv[3];
        }
    }

    if (argc == 6) {
        input_file = argv[1];
        if (strcmp(argv[2], "-s") == 0) {
            output_file = argv[3];
        }
        if (strcmp(argv[4], "-p") == 0) {
            petri_net_file = argv[5];
        }
    }

    int nodes, edge_node1, edge_node2;
    string prec;

    ifstream tpg (input_file);
    ifstream inp (input_file);

    map<int, bool> tasks;
    while (true) {
        if( inp.eof() ) break;
        inp >> edge_node1 >> prec >> edge_node2;
        tasks[edge_node1] = true;
        tasks[edge_node2] = true;
    }

    nodes = tasks.size();

    Graph g(nodes);

    if (!petri_net_file.empty()) {
        g.petri = true;
    }

    
    while (true) {
        if( tpg.eof() ) break;
        tpg >> edge_node1 >> prec >> edge_node2;
        g.addEdge(edge_node1, edge_node2);
    }

    map<int, int>::iterator itr;
    for(itr = g.vertices.begin(); itr != g.vertices.end(); itr++)
        g.all_states.push_back(itr->first);

    if (nodes != g.all_states.size()) {
        cout << "number of nodes are not matching" <<endl;
        return 0;
    }
    
    if (g.detectCycle()) {
        cout <<"ERROR: THE TASK PRECEDENCE GRAPH CONTAINS A CYCLE"<<endl;
        return 0;
    }

    if (!output_file.empty()) {
        ofstream file;
        file.open(output_file);

        vector<string> states;
        bool *visited_tasks = new bool[nodes];
        for (int i = 0; i < nodes; i++) {
            visited_tasks[i] = false;
            list<int> reachable_states;
            reachable_states = g.getReachableStates(i);
            g.reachable_nodes.insert(pair<int, list<int> >(g.vertices_index[i], reachable_states));
        }
        
        for (int i = 0; i < nodes; i++) {
            visited_tasks[i] = true;
            g.getStateSpace(i, visited_tasks);
        }
        
        g.state_space.push_back("{}");
        file << "THERE ARE " << g.state_space.size() << " REACHABLE STATES" << endl;
        for (int i = 0; i < g.state_space.size(); i++) {
            file <<g.state_space[i]<<endl;
        }
        file.close();
        tpg.close();
    }

    if (!petri_net_file.empty()) {
        g.generate_petri_net(petri_net_file);
    }
    return 0;
}