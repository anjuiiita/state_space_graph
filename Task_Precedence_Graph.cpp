#include<iostream>
#include <list>
#include <vector>
#include <map>
#include <fstream>

using namespace std;

class Graph
{
	int num_of_nodes;												 // No. of vertices
	int count;
	public:
		Graph(int num_of_nodes);					// Constructor
        list<int> *adj;
        map<int, int> vertices;
        map<int, int> vertices_index;
		void addEdge(int v, int w);		// to add an edge to graph
        list<int> getReachableStates(int id);
        bool detectCycle();
        bool detectCycleRec(int v, bool visited[], bool *recStack);

};

Graph::Graph(int nodes)
{
    num_of_nodes = nodes;
    count = 0;
	adj = new list<int>[num_of_nodes];
}

void Graph::addEdge(int v, int w)
{

    map<int,int>::iterator it;
    pair<map<int,int>::iterator,bool> ret;
    int first_node, second_node = 0;
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
 

int main(int argc, char* argv[])
{
    if (argc < 2) {
        cout <<"No input file provided" <<endl;
        return 0;
    }
    int nodes, edges, edge_node1, edge_node2;
    string prec;

    string filename = argv[1];
    fstream f;
    f.open(filename);
    ifstream tpg (filename);

    tpg >> nodes;
    tpg >> edges;

	Graph g(nodes);
    cout <<  nodes << edges <<endl;
    for (int i = 0; i < edges; i++) {
        tpg >> edge_node1 >> prec >> edge_node2;
        g.addEdge(edge_node1, edge_node2);
    }
    tpg.close();
  
    if (g.detectCycle()) {
        cout <<"ERROR: THE TASK PRECEDENCE GRAPH CONTAINS A CYCLE"<<endl;
        return 0;
    }

    list<int> reachable_states;
    vector<string> states;
    map<int,int>::iterator itr;
    for (int i = 0; i < nodes; i++) {
        reachable_states = g.getReachableStates(i);
        string states_str = "{";
        list<int>::iterator it;
        for (it = reachable_states.begin(); it != reachable_states.end(); ++it) {
            states_str += to_string(*it) + ",";
        }
        int size = reachable_states.size();
        if (size == 0) {
            itr = g.vertices_index.find(i);
            states_str += to_string(itr->second) + ",";
        } else {
            itr = g.vertices_index.find(i);
            states.push_back("{" + to_string(itr->second) +"}");
        }
        states_str.pop_back();
        states_str += "}";
        states.push_back(states_str);
    }
    states.push_back("{}");
    cout << "THERE ARE " << states.size() << " REACHABLE STATES" << endl;

    for (int i = 0; i < states.size(); i++) {
        cout <<states[i]<<endl;
    }

	return 0;
}