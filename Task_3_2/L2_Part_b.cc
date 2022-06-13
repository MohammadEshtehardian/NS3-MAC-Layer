#include <bits/stdc++.h>
#include <iostream>
#include <fstream>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/bridge-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/flow-monitor-helper.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/netanim-module.h"
#include "ns3/nstime.h"
using namespace std;
using namespace ns3;
NS_LOG_COMPONENT_DEFINE("L2");
  
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
int main(int argc, char *argv[])
{

    // enable log
    LogComponentEnable ("L2", LOG_LEVEL_INFO);
    CommandLine cmd (__FILE__);
    cmd.Parse (argc, argv);


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
    
    // find the MST
    vector < pair<int, int> > mst_edges = g.kruskalMST();
  
    NS_LOG_INFO ("Print paths of SPT:");
    vector< pair<int, int> >::iterator it;
    for (it=mst_edges.begin(); it!=mst_edges.end(); it++)
    {
        int u = it->first;
        int v = it->second;
  
        cout << u << " " << v << "\n";
    }


    NS_LOG_INFO ("Create nodes.");
    // node container for computers
    NodeContainer computers_container;
    computers_container.Create(8);

    // node container for bridges
    NodeContainer bridges_container;
    bridges_container.Create(6);

    NS_LOG_INFO ("Build Topology");
    // csma helper for connecting computers to bridges
    CsmaHelper csma_comp_br;
    csma_comp_br.SetChannelAttribute("DataRate", StringValue("1Mbps"));
    csma_comp_br.SetChannelAttribute("Delay", TimeValue(MicroSeconds(10.0/50.0*1)));

    // computers and bridges device container
    NetDeviceContainer computers_device_container;
    vector<NetDeviceContainer> bridges_device_containers;
    for (int i=0; i<6; i++)
    {
        NetDeviceContainer bridge_device_container;
        bridges_device_containers.push_back(bridge_device_container);
    }
    NetDeviceContainer link;

    // install csma between computers and bridges
    for(int i=0; i<8; i++)
    {
        int j;
        if(i==0 || i==1) j=0;
        else if(i==2 || i==3) j=2;
        else if(i==4 || i==5) j=3;
        else j=5;
        link = csma_comp_br.Install(NodeContainer(computers_container.Get(i), bridges_container.Get(j)));
        computers_device_container.Add (link.Get(0));
        bridges_device_containers[j].Add (link.Get(1));
    }

    // install csma between bridges
    vector< pair<float, iPair> >::iterator it_graph;
    CsmaHelper csma;
    for (it=mst_edges.begin(); it!=mst_edges.end(); it++)
    {
        int u = it->first;
        int v = it->second;
        float w = 50.0;

        for (it_graph=g.edges.begin(); it_graph!=g.edges.end(); it_graph++)
        {
            if (it_graph->second.first==u && it_graph->second.second==v)
            {
                w = it_graph->first;
                break;
            } 
        }

        csma.SetChannelAttribute("DataRate", StringValue("1Mbps"));
        csma.SetChannelAttribute("Delay", TimeValue(MicroSeconds(w/50.0*1)));
        link = csma.Install(NodeContainer(bridges_container.Get(u-1), bridges_container.Get(v-1)));
        bridges_device_containers[u-1].Add (link.Get(0));
        bridges_device_containers[v-1].Add (link.Get(1));
    }

    // install bridge helper on bridges
    BridgeHelper bridge_helper;
    for(int i=0; i<6; i++)
    {
        bridge_helper.Install(bridges_container.Get(i), bridges_device_containers[i]);
    }

    // set layer 3 parameters
    NS_LOG_INFO ("Assign IP Addresses.");
    InternetStackHelper internet;
    internet.Install(computers_container);
    Ipv4AddressHelper address;
    address.SetBase("10.10.91.0", "255.255.255.0");
    Ipv4InterfaceContainer csmaInterfaces;
    csmaInterfaces = address.Assign (computers_device_container);
    Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

    // create UDP packets
    NS_LOG_INFO ("Create Applications.");
    ApplicationContainer app1;
    ApplicationContainer app2;
    uint16_t port = 9;   

    // flow from C1 to C6
    OnOffHelper onoff1 ("ns3::UdpSocketFactory", Address (InetSocketAddress (Ipv4Address ("10.10.91.6"), port)));
    onoff1.SetConstantRate (DataRate ("1000kb/s"));
    app1.Add(onoff1.Install (computers_container.Get(0)));

    PacketSinkHelper sink ("ns3::UdpSocketFactory", Address (InetSocketAddress (Ipv4Address::GetAny (), port)));
    ApplicationContainer sink1 = sink.Install (computers_container.Get(5));
    sink1.Start (Seconds (1.0));
    sink1.Stop (Seconds (11.0));

    // flow from C2 to C7
    OnOffHelper onoff2 ("ns3::UdpSocketFactory", Address (InetSocketAddress (Ipv4Address ("10.10.91.7"), port)));
    onoff2.SetConstantRate (DataRate ("1000kb/s"));
    app2.Add(onoff2.Install (computers_container.Get(1)));    

    ApplicationContainer sink2 = sink.Install (computers_container.Get(6));
    sink2.Start (Seconds (1.1));
    sink2.Stop (Seconds (11.1));

    // start the application
    app1.Start (Seconds (1.0));
    app1.Stop (Seconds (11.0));
    app2.Start (Seconds (1.1));
    app2.Stop (Seconds (11.1));

    // enable trace files
    // AsciiTraceHelper ascii;
    // csma.EnableAsciiAll (ascii.CreateFileStream ("L2.tr"));

    // enabel pcap files
    // csma_comp_br.EnablePcapAll ("L2", false);

    NS_LOG_INFO ("Run Simulation.");
    FlowMonitorHelper flowmon;
    Ptr<FlowMonitor> monitor = flowmon.InstallAll ();

    Simulator::Stop (Seconds (12));
    Simulator::Run ();

    monitor->CheckForLostPackets ();
    Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowmon.GetClassifier ());
    FlowMonitor::FlowStatsContainer stats = monitor->GetFlowStats ();
    for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin (); i != stats.end (); ++i)
        {
            Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (i->first);
            cout << "Flow " << i->first << " (" << t.sourceAddress << " -> " << t.destinationAddress << ")\n";
            cout << "  Tx Packets: " << i->second.txPackets << "\n";
            cout << "  Tx Bytes:   " << i->second.txBytes << "\n";
            cout << "  TxOffered:  " << i->second.txBytes * 8.0 / 9.0 / 1000 / 1000  << " Mbps\n";
            cout << "  Rx Packets: " << i->second.rxPackets << "\n";
            cout << "  Rx Bytes:   " << i->second.rxBytes << "\n";
            cout << "  Throughput: " << i->second.rxBytes * 8.0 / 9.0 / 1000 / 1000  << " Mbps\n";
            cout << "  Delay:  " << (i->second.delaySum/i->second.rxPackets).As(Time::AUTO) << "\n";
        }

    // 11. Cleanup
    Simulator::Destroy ();
  
    return 0;
}