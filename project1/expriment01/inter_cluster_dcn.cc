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
 */
// ===========================================================================
//
//
//===========================================================================

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/csma-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/tcp-l4-protocol.h"
#include "ns3/applications-module.h"
#include <string>

using namespace ns3;


void tcpConnect(Ptr< Node > sNode, Ipv4Address sIP, unsigned int sPort,  Ptr< Node > cNode, \
                unsigned sStart, unsigned sEnd, unsigned cStart, unsigned cEnd) {
  ApplicationContainer sinkApp;
  ApplicationContainer clientApp;
  PacketSinkHelper packetSinkHelper ("ns3::TcpSocketFactory", InetSocketAddress(sIP, sPort));
  sinkApp = packetSinkHelper.Install (sNode);
  sinkApp.Start(Seconds (sStart));
  sinkApp.Stop(Seconds (sEnd));

  OnOffHelper client("ns3::TcpSocketFactory", InetSocketAddress(sIP, sPort));
  client.SetAttribute ("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=50]"));
  client.SetAttribute ("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
  client.SetAttribute ("DataRate", DataRateValue (DataRate ("1.0Mbps")));
  client.SetAttribute ("PacketSize", UintegerValue (2000));
  
  clientApp = client.Install (cNode);
  clientApp.Start(Seconds (cStart));
  clientApp.Stop (Seconds (cEnd));
}


int main (int argc, char *argv[])
{
  // LogComponentEnable ("PacketSink", LOG_LEVEL_INFO);
  // LogComponentEnable ("OnOffApplication", LOG_LEVEL_INFO);

  unsigned int nSer = 8;
  unsigned int nToR = 4;
  unsigned int nAggr = 2;
  unsigned int nCore = 1;
  
  // get nodes
  NodeContainer nodes[4];
  nodes[0].Create (nSer); // 8 servers
  nodes[1].Create (nToR); // 4 ToR switches
  nodes[2].Create (nAggr); // 2 Aggregation switches
  nodes[3].Create (nCore); // 1 core switch
  unsigned int totalSeg = nToR + nAggr + nAggr*nCore;

  NodeContainer networkSegment[totalSeg];
  for (unsigned int i = 0; i < nToR + nAggr + nAggr*nCore; ++i) {
    if (i < nToR) {
      networkSegment[i].Add(nodes[1].Get(i));
      networkSegment[i].Add (nodes[0].Get(2*i));
      networkSegment[i].Add (nodes[0].Get(2*i+1));
    }
    else if (i < nToR + nAggr){
      networkSegment[i].Add(nodes[2].Get(i-4));
      networkSegment[i].Add (nodes[1].Get(2*(i-4)));
      networkSegment[i].Add (nodes[1].Get(2*(i-4)+1));
    }
    else {
      networkSegment[i].Add(nodes[3].Get(0));
      networkSegment[i].Add(nodes[2].Get(i-6));
    }
  }

  // initiate Ethenet channels and install net devices
  CsmaHelper csma[nToR + nAggr];
  NetDeviceContainer devices[nToR + nAggr];
  for (unsigned int i = 0; i < nToR + nAggr; ++i) {
    csma[i].SetChannelAttribute ("DataRate", StringValue ("1.0Mbps"));
    if (i > 3) {
      csma[i].SetChannelAttribute ("DataRate", StringValue ("2.0Mbps"));
    }
    csma[i].SetChannelAttribute ("Delay", TimeValue (NanoSeconds (500)));
    devices[i] = csma[i].Install(networkSegment[i]);
  }

  // initiate PPP channels and install net devices
  PointToPointHelper ppp[nAggr];
  NetDeviceContainer p2pDevices[nAggr];
  for (unsigned int i = 0; i < nAggr; ++i) {
    ppp[i].SetDeviceAttribute ("DataRate", StringValue ("1.5Mbps"));
    ppp[i].SetChannelAttribute ("Delay", StringValue ("500ns"));
    p2pDevices[i] = ppp[i].Install(networkSegment[i + nToR + nAggr]);
  }
  
  // install protocols
  InternetStackHelper stack;
  for (auto node : nodes) {
    stack.Install (node);
  }

  // assign ip address
  Ipv4AddressHelper address;
  Ipv4InterfaceContainer interfacesToR[nToR];
  char ip[20];
  for (unsigned int i = 0; i < nToR; ++i) {
    sprintf(ip, "10.0.%d.0", i+1);
    address.SetBase (ip, "255.255.255.0");
    interfacesToR[i] = address.Assign (devices[i]);
  }
  Ipv4InterfaceContainer interfacesAggr[nAggr];
  for (unsigned int i = 0; i < nAggr; ++i) {
    sprintf(ip, "10.%d.1.0", i+1);
    address.SetBase (ip, "255.255.255.0");
    interfacesAggr[i] = address.Assign (devices[i+nToR]);
  }
  Ipv4InterfaceContainer interfacesCore[nAggr];
  for (unsigned int i = 0; i < nAggr; ++i) {
    sprintf(ip, "192.168.%d.0", i+1);
    address.SetBase (ip, "255.255.255.0");
    interfacesCore[i] = address.Assign (p2pDevices[i]);
  }

  // applications
  unsigned port = 5000;
  tcpConnect(nodes[0].Get(0), interfacesToR[0].GetAddress(1), port+15, nodes[0].Get(4), 1, 60, 1, 51);
  tcpConnect(nodes[0].Get(5), interfacesToR[2].GetAddress(2), port+62, nodes[0].Get(1), 1, 60, 1, 51);
  tcpConnect(nodes[0].Get(2), interfacesToR[1].GetAddress(1), port+37, nodes[0].Get(6), 1, 60, 1, 51);
  tcpConnect(nodes[0].Get(7), interfacesToR[3].GetAddress(2), port+84, nodes[0].Get(3), 1, 60, 1, 51);

  
  // set traces
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
  csma[0].EnablePcap ("inter_cluster_ser", devices[0].Get (1), true); //n1
  csma[0].EnablePcap ("inter_cluster_tor", devices[0].Get (0), true); //t1
  csma[4].EnablePcap ("inter_cluster_aggr", devices[4].Get (0), true); //a1
  ppp[0].EnablePcap ("inter_cluster_core", p2pDevices[0].Get (0), true); //c1


  
  Simulator::Stop (Seconds(50));
  Simulator::Run ();
  Simulator::Destroy ();

  return 0;
}