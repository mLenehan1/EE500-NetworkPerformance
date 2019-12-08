/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
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
 * Authors: Joe Kopena <tjkopena@cs.drexel.edu>
 * Modified by: Longhao Zou, Oct 2016 for EE500 <longhao.zou@dcu.ie>
 * 
 
            EE500 Assignment 2016-2017
            Default WiFi Network Topology
  
                WiFi 192.168.0.0
            -------------------------
            |AP (node 0:192.168.0.1)|
            -------------------------
             *         *           *    
            /          |            \
  Traffic 1/  Traffic 2|    ------   \ Traffic N
          /            |              \
      user 1       user 2     ------   user N
   (node 1         (node 2     ------  (node N
   :192.168.0.2    :192.168.0.3 ------ :192.168.0.N+1
   :1000)          :1001)        ------:1000+(N-1))
   
   PS: In this example, I just implemented only 1 WiFi user.
   
 */
  
#include <ctime>

#include <sstream>

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/mobility-module.h"
#include "ns3/wifi-module.h"
#include "ns3/internet-module.h"

#include "ns3/stats-module.h"

#include "wifi-example-apps.h"

using namespace ns3;
using namespace std;

NS_LOG_COMPONENT_DEFINE ("WiFiExampleSim");

void TxCallback (Ptr<CounterCalculator<uint32_t> > datac,
                 std::string path, Ptr<const Packet> packet) {
  NS_LOG_INFO ("Sent frame counted in " <<
               datac->GetKey ());
  datac->Update ();
  // end TxCallback
}

//----------------------------------------------------------------------
//-- main
//----------------------------------------------
int main (int argc, char *argv[]) {
  //LogComponentEnable ("WiFiDistanceApps", LOG_LEVEL_INFO);
  double distance = 50.0;
  double distance2 = 100.0;
  double simTime = 20; //Simulation Running Time (in seconds)
  int users = 1;
  string txInterval = "0.05";
  string dbPrefix = "DBFile";
  string format ("db"); //Default as .db format (Please refer to sqlite-data-output.cc and sqlite-data-output.h located in /src/stats/model)

  string experiment ("wifi-example-sim"); //the name of your experiment
  string strategy ("wifi-default"); 
  string input;
  string runID; 

  {
    stringstream sstr;
    sstr << "run-" << time (NULL);
    runID = sstr.str ();
  }

  // Set up command line parameters used to control the experiment.
  CommandLine cmd;
  cmd.AddValue ("distance", "Distance apart to place nodes (in meters).",
                distance);
  cmd.AddValue ("txInterval", "Interval between packet trasmissions (in seconds).", txInterval);
  cmd.AddValue ("users", "Number of Users", users);
  cmd.AddValue ("dbPrefix", "Database File Name", dbPrefix); 
  //cmd.AddValue ("format", "Format to use for data output.", 
    //            format);
  cmd.AddValue ("simTime", "Simulation Running Time (in seconds)", simTime);
  cmd.AddValue ("experiment", "Identifier for experiment.",
                experiment);
  cmd.AddValue ("strategy", "Identifier for strategy.",
                strategy);
  cmd.AddValue ("run", "Identifier for run.",
                runID);
  cmd.Parse (argc, argv);

  if (format != "omnet" && format != "db") {
      NS_LOG_ERROR ("Unknown output format '" << format << "'");
      return -1;
    }

  #ifndef STATS_HAS_SQLITE3
  if (format == "db") {
      NS_LOG_ERROR ("sqlite support not compiled in.");
      return -1;
    }
  #endif

  {
    stringstream sstr ("");
    if(dbPrefix.compare("P1QAP2")==0){
        sstr << txInterval;
    } else if(dbPrefix.compare("P1QCP3")==0){
        sstr << distance;
        sstr << " " << users;
    } else if(dbPrefix.compare("P1QCP2")==0){
        sstr << users;
    } else{
        sstr << distance;
    }
    input = sstr.str ();
  }

  //------------------------------------------------------------
  //-- Create nodes and network stacks
  //--------------------------------------------
  NS_LOG_INFO ("Creating nodes.");
  NodeContainer nodes;
  nodes.Create (users+1);

  NS_LOG_INFO ("Installing WiFi and Internet stack.");
  WifiHelper wifi = WifiHelper::Default ();
  NqosWifiMacHelper wifiMac = NqosWifiMacHelper::Default ();
  wifiMac.SetType ("ns3::AdhocWifiMac");
  YansWifiPhyHelper wifiPhy = YansWifiPhyHelper::Default ();
  YansWifiChannelHelper wifiChannel = YansWifiChannelHelper::Default ();
  wifiPhy.SetChannel (wifiChannel.Create ());
  NetDeviceContainer nodeDevices = wifi.Install (wifiPhy, wifiMac, nodes);

  InternetStackHelper internet;
  internet.Install (nodes);
  Ipv4AddressHelper ipAddrs;
  ipAddrs.SetBase ("192.168.0.0", "255.255.255.0");
  ipAddrs.Assign (nodeDevices);
  
  //------------------------------------------------------------
  //-- Setup physical layout
  //--------------------------------------------
  NS_LOG_INFO ("Installing static mobility; distance " << distance << " .");
  MobilityHelper mobility;
  Ptr<ListPositionAllocator> positionAlloc =
    CreateObject<ListPositionAllocator>();
  positionAlloc->Add (Vector (0.0, 0.0, 0.0));
  if(dbPrefix.compare("P1QCP1")!=0){
    for(int i=0; i<users; i++){
      positionAlloc->Add (Vector (0.0, distance, 0.0));
    }
  } else {
    positionAlloc->Add (Vector (0.0, distance, 0.0));
    positionAlloc->Add (Vector (0.0, distance2, 0.0));
  }
  mobility.SetPositionAllocator (positionAlloc);
  mobility.Install (nodes);

  //------------------------------------------------------------
  //-- Create the traffic between AP and WiFi Users
  //------------------------------------------------------------
  //------------------------------------------------------------
  //-- 1. Create the first traffic for the first WiFi user on WiFi AP (source)
  //--------------------------------------------
  NS_LOG_INFO ("Create traffic source & sink.");
  Ptr<Sender> sender[users];
  Ptr<Node> appSource = NodeList::GetNode (0);
  for(int i=0; i<users; i++){
    sender[i] = CreateObject<Sender>();
    sender[i]->SetAttribute("Port", UintegerValue(1000+i));//Lisening Port of the first WiFi user
    sender[i]->SetAttribute("PacketSize", UintegerValue (1000)); //bytes
    sender[i]->SetAttribute("Interval", StringValue ("ns3::ConstantRandomVariable[Constant=" + txInterval + "]")); //seconds
    sender[i]->SetAttribute("NumPackets",UintegerValue (100000000));
    appSource->AddApplication (sender[i]);
    sender[i]->SetStartTime (Seconds (0));
  }

  //------------------------------------------------------------
  //-- 2. Create the first WiFi User (sink)
  //--------------------------------------------
  Ptr<Receiver> receiver[users];
  for(int i=0; i<users; i++){
    Ptr<Node> appSink = NodeList::GetNode (i+1);
    receiver[i] = CreateObject<Receiver>();
    receiver[i]->SetAttribute("Port", UintegerValue(1000+i));//Lisening Port
    appSink->AddApplication (receiver[i]);
    receiver[i]->SetStartTime (Seconds (0));
  
    //Set IP address of the first User to AP (source)
    string ipString = "192.168.0."+std::to_string(i+2);
    const char* ip = ipString.c_str();
    //cout << ip;
    Config::Set ("/NodeList/*/ApplicationList/"+std::to_string(i)+"/$Sender/Destination",
               Ipv4AddressValue (ip));
  }

  //------------------------------------------------------------
  //-- Setup stats and data collection 
  //--  for the WiFi AP and the first WiFi User
  //--------------------------------------------

  // Create a DataCollector object to hold information about this run.
  DataCollector dataofuser[users];
  for(int i=0; i<users; i++){
    dataofuser[i].DescribeRun (experiment,
                      strategy,
                      input+" user"+std::to_string(i+1),
                      runID);

    // Add any information we wish to record about this run.
    dataofuser[i].AddMetadata ("author", "EE500-Michael"); //Please replace XXX with your first name!
  }
  // Create a counter to track how many frames are generated.  Updates
  // are triggered by the trace signal generated by the WiFi MAC model
  // object.  Here we connect the counter to the signal via the simple
  // TxCallback() glue function defined above.
  Ptr<CounterCalculator<uint32_t> > totalTx[users];
  Ptr<PacketCounterCalculator> totalRx[users];
  Ptr<PacketCounterCalculator> appTx[users];
  Ptr<CounterCalculator<> > appRx[users];
  Ptr<PacketSizeMinMaxAvgTotalCalculator> appTxPkts[users];
  Ptr<TimeMinMaxAvgTotalCalculator> delayStat[users];
  for(int i=0; i<users; i++){
    totalTx[i] = CreateObject<CounterCalculator<uint32_t> >();
    totalRx[i] = CreateObject<PacketCounterCalculator>();
    appTx[i] = CreateObject<PacketCounterCalculator>();
    appRx[i] = CreateObject<CounterCalculator<> >();
    appTxPkts[i] = CreateObject<PacketSizeMinMaxAvgTotalCalculator>();
    delayStat[i] = CreateObject<TimeMinMaxAvgTotalCalculator>();


    totalTx[i]->SetKey ("wifi-tx-frames");
    totalTx[i]->SetContext ("node[0]");
    Config::Connect ("/NodeList/0/DeviceList/*/$ns3::WifiNetDevice/Mac/MacTx",
                     MakeBoundCallback (&TxCallback, totalTx[i]));
    dataofuser[i].AddDataCalculator (totalTx[i]);

  // This is similar, but creates a counter to track how many frames
  // are received.  Instead of our own glue function, this uses a
  // method of an adapter class to connect a counter directly to the
  // trace signal generated by the WiFi MAC.
    totalRx[i]->SetKey ("wifi-rx-frames");
    totalRx[i]->SetContext ("node["+std::to_string(i+1)+"]");
    Config::Connect ("/NodeList/"+std::to_string(i+1)+"/DeviceList/*/$ns3::WifiNetDevice/Mac/MacRx",
                     MakeCallback (&PacketCounterCalculator::PacketUpdate,
                                   totalRx[i]));
    dataofuser[i].AddDataCalculator (totalRx[i]);

  // This counter tracks how many packets---as opposed to frames---are
  // generated.  This is connected directly to a trace signal provided
  // by our Sender class.
    appTx[i]->SetKey ("sender-tx-packets");
    appTx[i]->SetContext ("node[0]");
    Config::Connect ("/NodeList/0/ApplicationList/"+std::to_string(i)+"/$Sender/Tx",
                     MakeCallback (&PacketCounterCalculator::PacketUpdate,
                                   appTx[i]));
    dataofuser[i].AddDataCalculator (appTx[i]);

  // Here a counter for received packets is directly manipulated by
  // one of the custom objects in our simulation, the Receiver
  // Application.  The Receiver object is given a pointer to the
  // counter and calls its Update() method whenever a packet arrives.
    appRx[i]->SetKey ("receiver-rx-packets");
    appRx[i]->SetContext ("node["+std::to_string(i+1)+"]");
    receiver[i]->SetCounter (appRx[i]);
    dataofuser[i].AddDataCalculator (appRx[i]);

  /**
   * Just to show this is here...
   Ptr<MinMaxAvgTotalCalculator<uint32_t> > test = 
   CreateObject<MinMaxAvgTotalCalculator<uint32_t> >();
   test->SetKey("test-dc");
   data.AddDataCalculator(test);

   test->Update(4);
   test->Update(8);
   test->Update(24);
   test->Update(12);
  **/

  // This DataCalculator connects directly to the transmit trace
  // provided by our Sender Application.  It records some basic
  // statistics about the sizes of the packets received (min, max,
  // avg, total # bytes), although in this scenaro they're fixed.
    appTxPkts[i]->SetKey ("tx-pkt-size");
    appTxPkts[i]->SetContext ("node[0]");
    Config::Connect ("/NodeList/0/ApplicationList/"+std::to_string(i)+"/$Sender/Tx",
                     MakeCallback
                       (&PacketSizeMinMaxAvgTotalCalculator::PacketUpdate,
                       appTxPkts[i]));
    dataofuser[i].AddDataCalculator (appTxPkts[i]);

  // Here we directly manipulate another DataCollector tracking min,
  // max, total, and average propagation delays.  Check out the Sender
  // and Receiver classes to see how packets are tagged with
  // timestamps to do this.
    delayStat[i]->SetKey ("delay");
    delayStat[i]->SetContext (".");
    receiver[i]->SetDelayTracker (delayStat[i]); //nanoseconds
    dataofuser[i].AddDataCalculator (delayStat[i]);
  }

  //------------------------------------------------------------
  //-- Run the simulation
  //--------------------------------------------
  NS_LOG_INFO ("Run Simulation.");
  Simulator::Stop(Seconds(simTime));
  Simulator::Run ();
  
  //------------------------------------------------------------
  //-- Generate statistics output.
  //--------------------------------------------

  // Pick an output writer based in the requested format.
  Ptr<DataOutputInterface> output = 0;
  if (format == "omnet") {
      NS_LOG_INFO ("Creating omnet formatted data output.");
      output = CreateObject<OmnetDataOutput>();
    } else if (format == "db") {
    #ifdef STATS_HAS_SQLITE3
      NS_LOG_INFO ("Creating sqlite formatted data output.");
      output = CreateObject<SqliteDataOutput>();
    #endif
    } else {
      NS_LOG_ERROR ("Unknown output format " << format);
    }

  // Finally, have that writer interrogate the DataCollector and save
  // the results.
  if (output != 0)
  {
    output->SetFilePrefix(dbPrefix);
    for(int i=0; i<users; i++){
      output->Output (dataofuser[i]);
    }
  }
  // Free any memory here at the end of this example.
  
  Simulator::Destroy ();

  // end main
}
