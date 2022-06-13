// C++ program for Kruskal's algorithm to find Minimum
// Spanning Tree of a given connected, undirected and
// weighted graph
#include <bits/stdc++.h>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/bridge-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/command-line.h"
#include "ns3/uinteger.h"
#include "ns3/string.h"
#include "ns3/flow-monitor-helper.h"
#include "ns3/ipv4-flow-classifier.h"

using namespace ns3;

// Creating shortcut for an integer pair
typedef  std::pair<int, int> iPair;

// Structure to represent a graph
struct Graph
{
    int V, E;
    std::vector< std::pair<double, iPair> > edges;

    // Constructor
    Graph(int V, int E)
    {
        this->V = V;
        this->E = E;
    }

    // Utility function to add an edge
    void addEdge(int u, int v, double w)
    {
        edges.push_back({w, {u, v}});
    }

    // Function to find MST using Kruskal's
    // MST algorithm
    std::vector< std::pair<double, iPair>> kruskalMST();
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

std::vector< std::pair<double, iPair>> Graph::kruskalMST()
{
    double mst_wt = 0; // Initialize result

    // Sort edges in increasing order on basis of cost
    sort(edges.begin(), edges.end());

    // Create disjoint sets
    DisjointSets ds(V);

    // A Vector for saving the pairs
    std::vector< std::pair<double, iPair> > links;
    // Iterate through all sorted edges
    std::vector< std::pair<double, iPair> >::iterator it;
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
	    // and add it to links
	    links.push_back({it->first, {u, v}});
            
	    // Update MST weight
            mst_wt += it->first;

            // Merge two sets
            ds.merge(set_u, set_v);
        }
    }
    
    // Printing the links
    /*for (auto link : links) {
	    std::cout << link.second.first << " ---- " << link.second.second  << " with weight " << link.first << "\n";
    }*/

    return links;
}

void l2system (std::vector< std::pair<double,iPair>> min_links, std::string genRate)
{
    // Creating computers and bridges
    NodeContainer computerNodes;
    computerNodes.Create (8);
    
    NodeContainer bridgeNodes;
    bridgeNodes.Create (6);  

    // Building Topology
    CsmaHelper compToNodeCsma;
    compToNodeCsma.SetChannelAttribute ("DataRate", DataRateValue (1000000));
    compToNodeCsma.SetChannelAttribute ("Delay", TimeValue (NanoSeconds(200)));

    // Computers <--> Nodes
    // links
    NetDeviceContainer link11 = compToNodeCsma.Install (NodeContainer (computerNodes.Get (0), bridgeNodes.Get (0)));
    NetDeviceContainer link21 = compToNodeCsma.Install (NodeContainer (computerNodes.Get (1), bridgeNodes.Get (0)));
    NetDeviceContainer link33 = compToNodeCsma.Install (NodeContainer (computerNodes.Get (2), bridgeNodes.Get (2)));
    NetDeviceContainer link43 = compToNodeCsma.Install (NodeContainer (computerNodes.Get (3), bridgeNodes.Get (2)));
    NetDeviceContainer link54 = compToNodeCsma.Install (NodeContainer (computerNodes.Get (4), bridgeNodes.Get (3)));
    NetDeviceContainer link64 = compToNodeCsma.Install (NodeContainer (computerNodes.Get (5), bridgeNodes.Get (3)));
    NetDeviceContainer link76 = compToNodeCsma.Install (NodeContainer (computerNodes.Get (6), bridgeNodes.Get (5)));
    NetDeviceContainer link86 = compToNodeCsma.Install (NodeContainer (computerNodes.Get (7), bridgeNodes.Get (5)));
    
    // Bridge Devices
    std::vector <NetDeviceContainer> allBridgeDevices;
    for (int i=0; i<6; i++){
        allBridgeDevices.push_back(NetDeviceContainer ());
    }

    // Adding Devices
    allBridgeDevices[0].Add (link11.Get (1));
    allBridgeDevices[0].Add (link21.Get (1));
    allBridgeDevices[2].Add (link33.Get (1));
    allBridgeDevices[2].Add (link43.Get (1));
    allBridgeDevices[3].Add (link54.Get (1));
    allBridgeDevices[3].Add (link64.Get (1));
    allBridgeDevices[5].Add (link76.Get (1));
    allBridgeDevices[5].Add (link86.Get (1));
    
    // Nodes <--> Nodes
    // links 
    for (auto min_link : min_links)
    {
       int firstBridge = min_link.second.first;
       int secondBridge = min_link.second.second;
       double weight = min_link.first;
       std::cout << firstBridge << " <-- " << weight << " --> " << secondBridge << "\n";
       CsmaHelper nodeToNodeCsma;
       nodeToNodeCsma.SetChannelAttribute ("DataRate", DataRateValue (1000000));
       nodeToNodeCsma.SetChannelAttribute ("Delay" , TimeValue ( MicroSeconds (weight/50 * 2)));
       NetDeviceContainer link = nodeToNodeCsma.Install (NodeContainer (bridgeNodes.Get (firstBridge - 1), bridgeNodes.Get (secondBridge - 1)));
       allBridgeDevices[firstBridge - 1].Add (link.Get (0));
       allBridgeDevices[secondBridge - 1].Add (link.Get (1));
    }


    // install bridges
    BridgeHelper bridge;
    bridge.Install (bridgeNodes.Get (0), allBridgeDevices[0]);
    bridge.Install (bridgeNodes.Get (1), allBridgeDevices[1]);
    bridge.Install (bridgeNodes.Get (2), allBridgeDevices[2]);
    bridge.Install (bridgeNodes.Get (3), allBridgeDevices[3]);
    bridge.Install (bridgeNodes.Get (4), allBridgeDevices[4]);
    bridge.Install (bridgeNodes.Get (5), allBridgeDevices[5]);

    // Add internet stack
    InternetStackHelper internet;
    internet.Install (computerNodes);

    // Assign IP addresses
    Ipv4AddressHelper ipv4;
    ipv4.SetBase ("10.10.45.0", "255.255.255.0"); // my student ID is 98109645

    NetDeviceContainer computerDevices;
    computerDevices.Add (link11.Get (0));
    computerDevices.Add (link21.Get (0));
    computerDevices.Add (link33.Get (0));
    computerDevices.Add (link43.Get (0));
    computerDevices.Add (link54.Get (0));
    computerDevices.Add (link64.Get (0));
    computerDevices.Add (link76.Get (0));
    computerDevices.Add (link86.Get (0));
    
    ipv4.Assign (computerDevices);
    
    Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

    // Create OnOff application
    
    uint16_t port = 9;
    OnOffHelper onOff6 ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address ("10.10.45.6"), port));
    onOff6.SetConstantRate (DataRate (genRate));

    OnOffHelper onOff7 ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address ("10.10.45.7"), port));
    onOff7.SetConstantRate (DataRate (genRate));

    ApplicationContainer app1;
    ApplicationContainer app2;
    app1.Add (onOff6.Install (computerNodes.Get (0)));
    app2.Add (onOff7.Install (computerNodes.Get (1)));
    app1.Start (Seconds (1.0));
    app1.Stop (Seconds (11.0));
    app2.Start (Seconds (1.0001));
    app2.Stop (Seconds (11.0001));


    // Install FlowMonitor on all nodes
    FlowMonitorHelper flowmon;
    Ptr<FlowMonitor> monitor = flowmon.InstallAll ();


    // Do the simulation
    Simulator::Stop (Seconds (12)); 
    Simulator::Run ();

    // Print statistics
    monitor->CheckForLostPackets ();
    Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowmon.GetClassifier ());
    FlowMonitor::FlowStatsContainer stats = monitor->GetFlowStats ();
    for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin (); i != stats.end (); ++i)
    {
        Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (i->first);
        std::cout << "Flow " << i->first << " (" << t.sourceAddress << " -> " << t.destinationAddress << ")\n";
        std::cout << "  Tx Packets: " << i->second.txPackets << "\n";
        std::cout << "  Tx Bytes:   " << i->second.txBytes << "\n";
        std::cout << "  TxOffered:  " << i->second.txBytes * 8.0 / 9.0 / 1000 / 1000  << " Mbps\n";
        std::cout << "  Rx Packets: " << i->second.rxPackets << "\n";
        std::cout << "  Rx Bytes:   " << i->second.rxBytes << "\n";
        std::cout << "  Throughput: " << i->second.rxBytes * 8.0 / 9.0 / 1000 / 1000  << " Mbps\n";
    }


    // Clean up
    Simulator::Destroy ();

}

// Driver program to test above functions
int main(int argc, char **argv)
{
    /* Let us create above shown weighted
       and undirected graph */
    int V = 6, E = 12;
    Graph g(V, E);

    //  making above shown graph
    g.addEdge(2, 1, 50);
    g.addEdge(3, 1, 50);
    g.addEdge(3, 2, 50);
    g.addEdge(4, 1, 186.6);
    g.addEdge(5, 2, 100);
    g.addEdge(6, 2, 145.5);
    g.addEdge(6, 3, 186.6);
    g.addEdge(6, 4, 50);
    g.addEdge(4, 3, 193.2);
    g.addEdge(5, 3, 145.5);
    g.addEdge(6, 5, 50);
    g.addEdge(4, 5, 50);

    std::vector < std::pair<double, iPair>> links = g.kruskalMST();


    // Network Simulation
    std::string genRate = "1000kb/s"; 
    CommandLine cmd;
    cmd.AddValue ("GenRate", "Set the generation rate of the flows", genRate);
    cmd.Parse (argc, argv);
    
    std::cout << "Simulation for Generation Rate of " << genRate << "\n"; 
    l2system(links, genRate);

    return 0;
}
