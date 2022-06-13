#include<iostream>
#include<stdio.h>
#include <iterator>
#include <list>
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
#include "ns3/mobility-model.h"
#include "ns3/mobility-helper.h"
#include "ns3/constant-position-mobility-model.h"
using namespace std;
using namespace ns3;
NS_LOG_COMPONENT_DEFINE("L2");

#define max 6

vector<int> dijkstra(double G[max][max], int n, int startnode, int node);


int main(int argc, char *argv[]) {

   // enable log
   LogComponentEnable ("L2", LOG_LEVEL_INFO);
   CommandLine cmd (__FILE__);
   cmd.Parse (argc, argv);

   double G[max][max] = {{0, 50, 50, 186.6, 0, 0}
                        ,{50, 0, 50, 0, 100, 145.5}
                        ,{50, 50, 0, 193.2, 186.6}
                        ,{186.6, 0, 193.2, 50, 50}
                        ,{0, 100, 145.5, 50, 0, 50}
                        ,{0, 145.5, 186.6, 50, 50, 0}};
   int n = 6;
   int u = 0;
   vector<int> g1, g2, g3;
   g1 = dijkstra(G, n, u, 2);
   g2 = dijkstra(G, n, u, 3);
   g3 = dijkstra(G, n, u, 5);

   NS_LOG_INFO ("Create nodes.");
   // node container for computers
   NodeContainer computers_container;
   computers_container.Create(8);

   // node container for bridges
   NodeContainer bridges_container;
   bridges_container.Create(6);

   // create mobility
   for (uint8_t i = 0; i < 8; ++i)
   {
      computers_container.Get (i)->AggregateObject (CreateObject<ConstantPositionMobilityModel> ());
   }

   for (uint8_t i = 0; i < 6; ++i)
   {
      bridges_container.Get (i)->AggregateObject (CreateObject<ConstantPositionMobilityModel> ());
   }


   MobilityHelper computer_mobility;
   Ptr<ListPositionAllocator> computer_positionAlloc = CreateObject<ListPositionAllocator> ();

   computer_positionAlloc-> Add (Vector (30.0, 2.0, 0.0));
   computer_positionAlloc-> Add (Vector (45.0, 0.0, 0.0));
   computer_positionAlloc-> Add (Vector (85.0, 12.0, 0.0));
   computer_positionAlloc-> Add (Vector (80.0, 25.0, 0.0));
   computer_positionAlloc-> Add (Vector (90.0, 35.0, 0.0));
   computer_positionAlloc-> Add (Vector (80.0, 60.0, 0.0));
   computer_positionAlloc-> Add (Vector (-10.0, 55.0, 0.0));
   computer_positionAlloc-> Add (Vector (-7.0, 35.0, 0.0));

   computer_mobility.SetPositionAllocator(computer_positionAlloc);
   computer_mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
   computer_mobility.Install(computers_container);


   MobilityHelper bridge_mobility;
   Ptr<ListPositionAllocator> bridge_positionAlloc = CreateObject<ListPositionAllocator> ();

   bridge_positionAlloc-> Add (Vector (40.0, 10.0, 0.0));
   bridge_positionAlloc-> Add (Vector (15.0, 20.0, 0.0));
   bridge_positionAlloc-> Add (Vector (70.0, 20.0, 0.0));
   bridge_positionAlloc-> Add (Vector (70.0, 50.0, 0.0));
   bridge_positionAlloc-> Add (Vector (50.0, 70.0, 0.0));
   bridge_positionAlloc-> Add (Vector (5.0, 40.0, 0.0));

   bridge_mobility.SetPositionAllocator(bridge_positionAlloc);
   bridge_mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
   bridge_mobility.Install(bridges_container);

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
   CsmaHelper csma;
   for (size_t i = 0; i < g1.size()-1; i++)
   {
      int u, v;
      double w;
      u = g1[i];
      v = g1[i+1];
      w = G[u][v];
      csma.SetChannelAttribute("DataRate", StringValue("1Mbps"));
      csma.SetChannelAttribute("Delay", TimeValue(MicroSeconds(w/50.0*1)));
      link = csma.Install(NodeContainer(bridges_container.Get(u), bridges_container.Get(v)));
      bridges_device_containers[u].Add (link.Get(0));
      bridges_device_containers[v].Add (link.Get(1));
   }

   for (size_t i = 0; i < g2.size()-1; i++)
   {
      int u, v;
      double w;
      u = g2[i];
      v = g2[i+1];
      w = G[u][v];
      csma.SetChannelAttribute("DataRate", StringValue("1Mbps"));
      csma.SetChannelAttribute("Delay", TimeValue(MicroSeconds(w/50.0*1)));
      link = csma.Install(NodeContainer(bridges_container.Get(u), bridges_container.Get(v)));
      bridges_device_containers[u].Add (link.Get(0));
      bridges_device_containers[v].Add (link.Get(1));
   }

   for (size_t i = 0; i < g3.size()-1; i++)
   {
      int u, v;
      double w;
      u = g3[i];
      v = g3[i+1];
      w = G[u][v];
      csma.SetChannelAttribute("DataRate", StringValue("1Mbps"));
      csma.SetChannelAttribute("Delay", TimeValue(MicroSeconds(w/50.0*1)));
      link = csma.Install(NodeContainer(bridges_container.Get(u), bridges_container.Get(v)));
      bridges_device_containers[u].Add (link.Get(0));
      bridges_device_containers[v].Add (link.Get(1));
   }

   // install bridge helper on bridges
   BridgeHelper bridge_helper;
   for(int i=0; i<6; i++)
   {
      bridge_helper.Install(bridges_container.Get(i), bridges_device_containers[i]);
   }

   // set layer 3 parameters
   InternetStackHelper internet;
   internet.Install(computers_container);
   Ipv4AddressHelper address;
   address.SetBase("10.10.91.0", "255.255.255.0");
   Ipv4InterfaceContainer csmaInterfaces;
   csmaInterfaces = address.Assign (computers_device_container);
   Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

   // create UDP packets
   ApplicationContainer app1;
   ApplicationContainer app2;
   uint16_t port = 9;   

   // flow from C1 to C6
   OnOffHelper onoff1 ("ns3::UdpSocketFactory", Address (InetSocketAddress (Ipv4Address ("10.10.91.6"), port)));
   onoff1.SetConstantRate (DataRate ("10000kb/s"));
   app1.Add(onoff1.Install (computers_container.Get(0)));

   // flow from C2 to C7
   OnOffHelper onoff2 ("ns3::UdpSocketFactory", Address (InetSocketAddress (Ipv4Address ("10.10.91.7"), port)));
   onoff2.SetConstantRate (DataRate ("10000kb/s"));
   app2.Add(onoff2.Install (computers_container.Get(1)));    

   // start the application
   app1.Start (Seconds (1.0));
   app1.Stop (Seconds (11.0));
   app2.Start (Seconds (1.0));
   app2.Stop (Seconds (11.0));

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


vector<int> dijkstra(double G[max][max], int n, int startnode, int node) {

   int pred[max];
   double cost[max][max], distance[max];
   int visited[max] ,count, mindistance, nextnode, i, j;
   vector<int> g;
   g.push_back(node);

   for(i = 0; i < n; i++)
      for(j = 0; j < n; j++)
         if(G[i][j] == 0)
            cost[i][j] = 10000;
         else
            cost[i][j] = G[i][j];

   for(i = 0; i < n; i++) {
      distance[i] = cost[startnode][i];
      pred[i] = startnode;
      visited[i] = 0;
   }

   distance[startnode] = 0;
   visited[startnode] = 1;

   count = 1;
   while(count < n-1) {

      mindistance = 10000;

      for(i = 0; i < n; i++)
         if(distance[i] < mindistance&&!visited[i]) {
         mindistance = distance[i];
         nextnode = i;
      }

      visited[nextnode] = 1;

      for(i = 0; i < n; i++)
         if(!visited[i])

      if(mindistance + cost[nextnode][i] < distance[i]) {
         distance[i] = mindistance+cost[nextnode][i];
         pred[i] = nextnode;
      }
      count++;
   }

   for(i = 0; i < n; i++)
   if(i != startnode) {
      //cout << "\nDistance of node" << i << " = "<< distance[i];
      if (i == node)
         cout << "\nPath = " << i+1;
      j = i;
      do {
         j = pred[j];
         if (i == node){
            cout << " <- " << j+1;
            g.push_back(j);
         }
      }while(j != startnode);
   }
   cout << "\n\n";
   return g;
}