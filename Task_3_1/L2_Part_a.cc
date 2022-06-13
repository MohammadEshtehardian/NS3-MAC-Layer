// C++ program for Kruskal's algorithm to find Minimum
// Spanning Tree of a given connected, undirected and
// weighted graph
#include<bits/stdc++.h>
using namespace std;
  
// Creating shortcut for an integer pair
typedef  pair<int, int> iPair;
  
// Structure to represent a graph
struct Graph
{
    int V, E;
    vector< pair<float, iPair> > edges;
  
    // Constructor
    Graph(int V, int E)
    {
        this->V = V;
        this->E = E;
    }
  
    // Utility function to add an edge
    void addEdge(int u, int v, float w)
    {
        edges.push_back({w, {u, v}});
    }
  
    // Function to find MST using Kruskal's
    // MST algorithm
    vector < pair<int, int> > kruskalMST();
};
  
// To represent Disjoint Sets
struct DisjointSets
{
    int *parent, *rnk;
    int n;
  
    // Constructor.
    DisjointSets(int n)
    {
        // Allocate memory
        this->n = n;
        parent = new int[n+1];
        rnk = new int[n+1];
  
        // Initially, all vertices are in
        // different sets and have rank 0.
        for (int i = 0; i <= n; i++)
        {
            rnk[i] = 0;
  
            //every element is parent of itself
            parent[i] = i;
        }
    }
  
    // Find the parent of a node 'u'
    // Path Compression
    int find(int u)
    {
        /* Make the parent of the nodes in the path
           from u--> parent[u] point to parent[u] */
        if (u != parent[u])
            parent[u] = find(parent[u]);
        return parent[u];
    }
  
    // Union by rank
    void merge(int x, int y)
    {
        x = find(x), y = find(y);
  
        /* Make tree with smaller height
           a subtree of the other tree  */
        if (rnk[x] > rnk[y])
            parent[y] = x;
        else // If rnk[x] <= rnk[y]
            parent[x] = y;
  
        if (rnk[x] == rnk[y])
            rnk[y]++;
    }
};
  
 /* Functions returns weight of the MST*/ 
  
vector< pair<int, int> > Graph::kruskalMST()
{
    int mst_wt = 0; // Initialize result
    vector< pair<int, int> > mst_edges;

    // Sort edges in increasing order on basis of cost
    sort(edges.begin(), edges.end());
  
    // Create disjoint sets
    DisjointSets ds(V);
  
    // Iterate through all sorted edges
    vector< pair<float, iPair> >::iterator it;
    for (it=edges.begin(); it!=edges.end(); it++)
    {
        int u = it->second.first;
        int v = it->second.second;
  
        int set_u = ds.find(u);
        int set_v = ds.find(v);
  
        // Check if the selected edge is creating
        // a cycle or not (Cycle is created if u
        // and v belong to same set)
        if (set_u != set_v)
        {
            // Current edge will be in the MST
            // so add it
            mst_edges.push_back({u, v});
  
            // Update MST weight
            mst_wt += it->first;
  
            // Merge two sets
            ds.merge(set_u, set_v);
        }
    }
  
    return mst_edges;
}
  
// Driver program to test above functions
int main()
{
    /* Let us create above shown weighted
       and undirected graph */
    int V = 6, E = 12;
    Graph g(V, E);
  
    //  making above shown graph
    g.addEdge(1, 2, 50);
    g.addEdge(1, 3, 50);
    g.addEdge(1, 4, 186.6);
    g.addEdge(2, 3, 50);
    g.addEdge(2, 5, 100);
    g.addEdge(2, 6, 145.5);
    g.addEdge(3, 4, 193.2);
    g.addEdge(3, 5, 145.5);
    g.addEdge(3, 6, 186.6);
    g.addEdge(4, 5, 50);
    g.addEdge(4, 6, 50);
    g.addEdge(5, 6, 50);
    

    cout << "Edges of MST are \n";
    vector < pair<int, int> > mst_edges = g.kruskalMST();
  
    vector< pair<int, int> >::iterator it;
    for (it=mst_edges.begin(); it!=mst_edges.end(); it++)
    {
        int u = it->first;
        int v = it->second;
  
        cout << u << " " << v << "\n";
    }
  
    return 0;
}