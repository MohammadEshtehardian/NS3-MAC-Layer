/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2010 IITP RAS
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Pavel Boyko <boyko@iitp.ru>
 *
 * Classical hidden terminal problem and its RTS/CTS solution.
 *
 * Topology: [node 0] <-- -50 dB --> [node 1] <-- -50 dB --> [node 2]
 *
 * This example illustrates the use of
 *  - Wifi in ad-hoc mode
 *  - Matrix propagation loss model
 *  - Use of OnOffApplication to generate CBR stream
 *  - IP flow monitor
 */

#include "ns3/core-module.h"
#include "ns3/command-line.h"
#include "ns3/config.h"
#include "ns3/uinteger.h"
#include "ns3/boolean.h"
#include "ns3/string.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/ipv4-address-helper.h"
#include "ns3/udp-echo-helper.h"
#include "ns3/yans-wifi-channel.h"
#include "ns3/constant-position-mobility-model.h"
#include "ns3/propagation-loss-model.h"
#include "ns3/propagation-delay-model.h"
#include "ns3/on-off-helper.h"
#include "ns3/flow-monitor-helper.h"
#include "ns3/ipv4-flow-classifier.h"
#include "ns3/netanim-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/mobility-module.h"

using namespace ns3;

/// Run single 10 seconds experiment
void experiment (bool enableCtsRts, std::string wifiManager)
{
  // 0. Enable or disable CTS/RTS
  UintegerValue ctsThr = (enableCtsRts ? UintegerValue (100) : UintegerValue (2200));
  Config::SetDefault ("ns3::WifiRemoteStationManager::RtsCtsThreshold", ctsThr);

  // 1. Create 4 nodes
  NodeContainer nodes;
  nodes.Create (4);

  // 2. Place nodes somehow, this is required by every wireless simulation
  for (uint8_t i = 0; i < 4; ++i)
    {
      nodes.Get (i)->AggregateObject (CreateObject<ConstantPositionMobilityModel> ());
    }
  
  MobilityHelper mobility;

  Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");

  positionAlloc->Add (Vector (0.0, 0.0, 0.0));
  positionAlloc->Add (Vector (0.0, 10.0, 0.0));
  positionAlloc->Add (Vector (10.0, 10.0, 0.0));
  positionAlloc->Add (Vector (10.0, 0.0, 0.0));

  mobility.SetPositionAllocator (positionAlloc);

  mobility.Install (nodes);


  // 3. Create propagation loss matrix
  Ptr<MatrixPropagationLossModel> lossModel = CreateObject<MatrixPropagationLossModel> ();
  lossModel->SetDefaultLoss (120); // set default loss to 120 dB (no link)
  lossModel->SetLoss (nodes.Get (0)->GetObject<MobilityModel> (), nodes.Get (1)->GetObject<MobilityModel> (), 40); // set symmetric loss A1 <-> B1 to 40 dB
  lossModel->SetLoss (nodes.Get (2)->GetObject<MobilityModel> (), nodes.Get (1)->GetObject<MobilityModel> (), 50); // set symmetric loss B2 <-> B1 to 50 dB
  lossModel->SetLoss (nodes.Get (3)->GetObject<MobilityModel> (), nodes.Get (2)->GetObject<MobilityModel> (), 40); // set symmetric loss A2 <-> B2 to 40 dB

  // 4. Create & setup wifi channel
  Ptr<YansWifiChannel> wifiChannel = CreateObject <YansWifiChannel> ();
  wifiChannel->SetPropagationLossModel (lossModel);
  wifiChannel->SetPropagationDelayModel (CreateObject <ConstantSpeedPropagationDelayModel> ());

  // 5. Install wireless devices
  WifiHelper wifi;
  wifi.SetStandard (WIFI_STANDARD_80211b);
  wifi.SetRemoteStationManager ("ns3::" + wifiManager + "WifiManager");
  YansWifiPhyHelper wifiPhy;
  wifiPhy.SetChannel (wifiChannel);
  WifiMacHelper wifiMac;
  wifiMac.SetType ("ns3::AdhocWifiMac"); // use ad-hoc MAC
  NetDeviceContainer devices = wifi.Install (wifiPhy, wifiMac, nodes);

  // uncomment the following to have athstats output
  // AthstatsHelper athstats;
  // athstats.EnableAthstats(enableCtsRts ? "rtscts-athstats-node" : "basic-athstats-node" , nodes);

  // uncomment the following to have pcap output
  // wifiPhy.EnablePcap (enableCtsRts ? "rtscts-pcap-node" : "basic-pcap-node" , nodes);


  // 6. Install TCP/IP stack & assign IP addresses
  InternetStackHelper internet;
  internet.Install (nodes);
  Ipv4AddressHelper ipv4;
  ipv4.SetBase ("19.69.1.0", "255.255.255.0");
  ipv4.Assign (devices);

  // 7. Install applications: two CBR streams each saturating the channel
  ApplicationContainer cbrApps;
  uint16_t cbrPort1 = 12345;
  OnOffHelper onOffHelper1 ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address ("19.69.1.2"), cbrPort1));
  onOffHelper1.SetAttribute ("PacketSize", UintegerValue (1400));
  onOffHelper1.SetAttribute ("OnTime",  StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
  onOffHelper1.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));

  // flow 1:  node B1 -> node A1
  onOffHelper1.SetAttribute ("DataRate", StringValue ("3000000bps"));
  onOffHelper1.SetAttribute ("StartTime", TimeValue (Seconds (1.000000)));
  cbrApps.Add (onOffHelper1.Install (nodes.Get (0)));

  uint16_t cbrPort2 = 54321;
  OnOffHelper onOffHelper2 ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address ("19.69.1.3"), cbrPort2));
  onOffHelper2.SetAttribute ("PacketSize", UintegerValue (1400));
  onOffHelper2.SetAttribute ("OnTime",  StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
  onOffHelper2.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));

  // flow 2:  node B2 -> node A2
  onOffHelper2.SetAttribute ("DataRate", StringValue ("3000000bps"));
  onOffHelper2.SetAttribute ("StartTime", TimeValue (Seconds (1.000000)));
  cbrApps.Add (onOffHelper2.Install (nodes.Get (3)));

  /** \internal
   * We also use separate UDP applications that will send a single
   * packet before the CBR flows start.
   * This is a workaround for the lack of perfect ARP, see \bugid{187}
   */
  uint16_t  echoPort1 = 9;
  UdpEchoClientHelper echoClientHelper1 (Ipv4Address ("19.69.1.2"), echoPort1);
  echoClientHelper1.SetAttribute ("MaxPackets", UintegerValue (1));
  echoClientHelper1.SetAttribute ("Interval", TimeValue (Seconds (0.1)));
  echoClientHelper1.SetAttribute ("PacketSize", UintegerValue (10));
  ApplicationContainer pingApps;

  echoClientHelper1.SetAttribute ("StartTime", TimeValue (Seconds (0.001)));
  pingApps.Add (echoClientHelper1.Install (nodes.Get (0)));

  uint16_t  echoPort2 = 8;
  UdpEchoClientHelper echoClientHelper2 (Ipv4Address ("19.69.1.3"), echoPort2);
  echoClientHelper2.SetAttribute ("MaxPackets", UintegerValue (1));
  echoClientHelper2.SetAttribute ("Interval", TimeValue (Seconds (0.1)));
  echoClientHelper2.SetAttribute ("PacketSize", UintegerValue (10));

  // using different start times to workaround Bug 388 and Bug 912
  echoClientHelper2.SetAttribute ("StartTime", TimeValue (Seconds (0.001)));
  pingApps.Add (echoClientHelper2.Install (nodes.Get (3)));

  // 8. Install FlowMonitor on all nodes
  FlowMonitorHelper flowmon;
  Ptr<FlowMonitor> monitor = flowmon.InstallAll ();

  // 9. Run simulation for 10 seconds
  Simulator::Stop (Seconds (10));
  Simulator::Run ();

  // 10. Print per flow statistics
  monitor->CheckForLostPackets ();
  Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowmon.GetClassifier ());
  FlowMonitor::FlowStatsContainer stats = monitor->GetFlowStats ();
  for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin (); i != stats.end (); ++i)
    {
      // first 2 FlowIds are for ECHO apps, we don't want to display them
      //
      // Duration for throughput measurement is 9.0 seconds, since
      //   StartTime of the OnOffApplication is at about "second 1"
      // and
      //   Simulator::Stops at "second 10".
      if (i->first > 2)
        {
          Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (i->first);
          std::cout << "Flow " << i->first - 2 << " (" << t.sourceAddress << " -> " << t.destinationAddress << ")\n";
          std::cout << "  Tx Packets: " << i->second.txPackets << "\n";
          std::cout << "  Tx Bytes:   " << i->second.txBytes << "\n";
          std::cout << "  TxOffered:  " << i->second.txBytes * 8.0 / 9.0 / 1000 / 1000  << " Mbps\n";
          std::cout << "  Rx Packets: " << i->second.rxPackets << "\n";
          std::cout << "  Rx Bytes:   " << i->second.rxBytes << "\n";
          std::cout << "  Throughput: " << i->second.rxBytes * 8.0 / 9.0 / 1000 / 1000  << " Mbps\n";
        }
    }

  // 11. Cleanup
  Simulator::Destroy ();
}

int main (int argc, char **argv)
{
  std::string wifiManager ("Arf");
  CommandLine cmd (__FILE__);
  cmd.AddValue ("wifiManager", "Set wifi rate manager (Aarf, Aarfcd, Amrr, Arf, Cara, Ideal, Minstrel, Onoe, Rraa)", wifiManager);
  cmd.Parse (argc, argv);

  std::cout << "Hidden station experiment with RTS/CTS disabled:\n" << std::flush;
  experiment (false, wifiManager);
  std::cout << "------------------------------------------------\n";
  std::cout << "Hidden station experiment with RTS/CTS enabled:\n";
  experiment (true, wifiManager);

  return 0;
}
