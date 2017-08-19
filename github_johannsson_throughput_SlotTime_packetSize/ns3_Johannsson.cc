#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/internet-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/random-variable-stream.h"
#include "ns3/object.h"
#include "ns3/uinteger.h"
#include "ns3/traced-value.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/trace-helper.h"
#include "ns3/yans-wifi-helper.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>

using namespace std;
using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("sp_trace");

int main (int argc, char *argv[])
{
  LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
  // Number of nodes
  int nNodes = 1;
  uint32_t slot = 20; //slot time in microseconds
  uint32_t sifs = 10; //SIFS duration in microseconds

  uint32_t packetSize = 1472;
  uint32_t maxPacket = 40000;


  CommandLine cmd;
  cmd.AddValue ("n", "Number of nodes", nNodes);
  cmd.AddValue ("slot", "DCF slot time", slot);
  cmd.AddValue ("ps", "Packet size", packetSize);
  cmd.Parse (argc,argv);
  
  if (packetSize < 1472) {
    maxPacket = 120000;
  }

  int32_t pifs = slot+sifs;
  StringValue DataRate = StringValue("DsssRate11Mbps");

  // Set reference loss for 2.4 GHz
  Config::SetDefault ("ns3::LogDistancePropagationLossModel::ReferenceLoss", DoubleValue (40.046));

  // Create AP
  NodeContainer wifiApNode;
  wifiApNode.Create(1);

  // Create nodes
  NodeContainer wifiNodes;
  wifiNodes.Create(nNodes);

  // Create channel and phy
  YansWifiChannelHelper wifiChannel = YansWifiChannelHelper::Default ();
  YansWifiPhyHelper phy = YansWifiPhyHelper::Default ();
  phy.SetChannel (wifiChannel.Create ());

  // Set 802.11b
  WifiHelper wifi = WifiHelper::Default ();
  wifi.SetStandard ( WIFI_PHY_STANDARD_80211b);

  // Remote station
  wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager", "DataMode", DataRate, "ControlMode", DataRate);
  QosWifiMacHelper mac = QosWifiMacHelper::Default ();

  // Create wifi devices and set mac
  Ssid ssid = Ssid ( "bjarkiSsid");
  mac.SetType ("ns3::StaWifiMac", "Ssid", SsidValue (ssid), "ActiveProbing", BooleanValue (false));
  NetDeviceContainer wifiDevices = wifi.Install(phy, mac, wifiNodes);

  // Create AP device and set mac
  mac.SetType ("ns3::ApWifiMac", "Ssid", SsidValue (ssid));
  NetDeviceContainer apDevice = wifi.Install (phy, mac, wifiApNode);

  // Set DCF values
  Config::Set ("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Mac/Slot", TimeValue (MicroSeconds (slot)));
  Config::Set ("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Mac/Sifs", TimeValue (MicroSeconds (sifs)));
  Config::Set ("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Mac/Pifs", TimeValue (MicroSeconds (pifs)));

  // Mobility and position
  MobilityHelper mob;
  Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
  positionAlloc->Add (Vector (0.0, 0.0, 0.0));
  positionAlloc->Add (Vector (1.0, 0.0, 0.0));
  positionAlloc->Add (Vector (0.0, 1.0, 0.0));
  positionAlloc->Add (Vector (1.0, 1.0, 0.0));
  positionAlloc->Add (Vector (2.0, 0.0, 0.0));
  positionAlloc->Add (Vector (0.0, 2.0, 0.0));
  positionAlloc->Add (Vector (2.0, 1.0, 0.0));
  positionAlloc->Add (Vector (1.0, 2.0, 0.0));
  positionAlloc->Add (Vector (2.0, 2.0, 0.0));
  positionAlloc->Add (Vector (3.0, 1.0, 0.0));
  positionAlloc->Add (Vector (1.0, 3.0, 0.0));
  mob.SetPositionAllocator (positionAlloc);
  mob.SetMobilityModel ("ns3::ConstantPositionMobilityModel");

  mob.Install (wifiApNode);
  mob.Install (wifiNodes);

  // Internet stack
  InternetStackHelper stack;
  stack.Install (wifiApNode);
  stack.Install (wifiNodes);

  // IPv4
  Ipv4AddressHelper address;
  Ipv4Address addr;
  address.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer wifiNodesInterface;
  Ipv4InterfaceContainer apNodeInterface;

  apNodeInterface = address.Assign (apDevice);
  wifiNodesInterface = address.Assign (wifiDevices);

  // Confirm addresses
  // for (int i=0; i<nNodes; i++) {
  //   addr = wifiNodesInterface.GetAddress(i);
  //   cout << " Node " << i << " address: " << addr << endl;
  // }
  // addr = apNodeInterface.GetAddress(0);
  // cout << " AP address: "<< addr << endl;

  // Simulation time
  double StartTime = 0.0;
  double StopTime = 1;
  LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);

  // Server
  UdpServerHelper myServer(4001);
  ApplicationContainer serverApp = myServer.Install (wifiApNode.Get (0));;
  serverApp.Start (Seconds(StartTime));
  serverApp.Stop (Seconds(StopTime));

  // Client
  UdpClientHelper myClient (apNodeInterface.GetAddress (0), 4001);
  myClient.SetAttribute ("MaxPackets", UintegerValue (maxPacket));
  myClient.SetAttribute ("Interval", TimeValue (Time ("0.000015")));
  myClient.SetAttribute ("PacketSize", UintegerValue (packetSize));

  ApplicationContainer clientApp = myClient.Install (wifiNodes.Get (0));
  for (int i = 1; i<nNodes; i++) {
    myClient.Install (wifiNodes.Get (i));
  }

  clientApp.Start (Seconds(StartTime));
  clientApp.Stop (Seconds(StopTime));

  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  // Flow monitor
  FlowMonitorHelper flowmon;
  Ptr<FlowMonitor> monitor = flowmon.InstallAll();


  // Tracing
  // AsciiTraceHelper ascii;
  // phy.EnableAsciiAll(ascii.CreateFileStream("phy.tr"));

  // Run simulation
  cout << "C " << nNodes << " " << slot << " " << sifs << endl;
  cout << "Starting simulation..." << endl << endl;

  // Simulator::Schedule(Seconds(5.0), &PrintDrop);
  Simulator::Stop (Seconds(StopTime));
  Simulator::Run ();


  int tp = 0;
  monitor->CheckForLostPackets ();
  Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowmon.GetClassifier ());
  map<FlowId, FlowMonitor::FlowStats> stats = monitor->GetFlowStats ();
  for (map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin (); i != stats.end (); ++i)
  {
    if (i->first > 0) {
      Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (i->first);
      cout << i->first << " " << t.sourceAddress << " " << t.destinationAddress << " ";
      cout << i->second.rxBytes * 8.0 / (i->second.timeLastRxPacket.GetSeconds() - i->second.timeFirstTxPacket.GetSeconds())/1024/nNodes << "kbps\n";
      tp = tp + i->second.rxBytes * 8.0 / (i->second.timeLastRxPacket.GetSeconds() - i->second.timeFirstTxPacket.GetSeconds())/1024/nNodes;
    }
  }
  cout << "Total troughput: " << tp << " kbps" << endl;
  cout << "AVG: " << tp/nNodes << " kbps" << endl;

  cout << "Terminating simulation!" << endl;
  Simulator::Destroy ();


  // Get throughput from server
  uint32_t totalPackets = DynamicCast<UdpServer>(serverApp.Get (0))->GetReceived ();
  cout << totalPackets << endl;
  double throughput = totalPackets * packetSize * 8/((StopTime) * 1000000.0);
  cout << endl;
  cout << "V " << throughput/nNodes << " Mbit/s" << endl;

  return 0;
}
