#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/mobility-module.h"
#include "ns3/internet-module.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/ssid.h"
#include "ns3/netanim-module.h"
#include "ns3/gnuplot.h"
#include "ns3/flow-monitor-helper.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/propagation-loss-model.h"
#include "ns3/propagation-delay-model.h"
#include "ns3/wifi-module.h"
#include <iomanip>

using namespace ns3;

AnimationInterface * pAnim = 0;

void modify()
{
    static uint32_t resourceID= pAnim->AddResource ("/home/abrantedevops/assets/smartphone.png");
    static uint32_t resourceID2= pAnim->AddResource ("/home/abrantedevops/assets/wireless-router.png");
    pAnim->UpdateNodeImage (0, resourceID);
    pAnim->UpdateNodeImage (1, resourceID);
    pAnim->UpdateNodeImage (2, resourceID);
    pAnim->UpdateNodeImage (3, resourceID2);
    pAnim->UpdateNodeSize (0, 3, 3);    
    pAnim->UpdateNodeSize (1, 3, 3);
    pAnim->UpdateNodeSize (2, 3, 3);
    pAnim->UpdateNodeSize (3, 3, 3);
    pAnim->EnablePacketMetadata(true);

}

NS_LOG_COMPONENT_DEFINE ("wifi_project");

int 
main(int argc, char* argv[])
{
    bool verbose = true;
    uint32_t nWifi = 3;

    CommandLine cmd (__FILE__);
    cmd.AddValue ("nWifi", "Number of wifi STA devices", nWifi);
    cmd.AddValue ("verbose", "Tell echo applications to log if true", verbose);


    cmd.Parse (argc,argv);

    if (verbose)
    {
        LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
        LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);
    }


    NodeContainer wifiStaNodes;
    wifiStaNodes.Create (nWifi);
    
    NodeContainer wifiApNode;
    wifiApNode.Create (1);


    WifiHelper wifi;
    wifi.SetStandard(WIFI_STANDARD_80211ac);

    YansWifiPhyHelper phy;
    phy.SetPcapDataLinkType (YansWifiPhyHelper::DLT_IEEE802_11_RADIO);
    


    YansWifiChannelHelper channel = YansWifiChannelHelper::Default ();
    channel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
    channel.AddPropagationLoss("ns3::RangePropagationLossModel",
                               "MaxRange", DoubleValue(60));
    phy.SetChannel (channel.Create ());

    Ssid ssid = Ssid ("abrante");
    
    WifiMacHelper macsta;
    macsta.SetType ("ns3::StaWifiMac",
                "Ssid", SsidValue (ssid),
                "ActiveProbing", BooleanValue (false));

    WifiMacHelper macap;
    macap.SetType ("ns3::ApWifiMac",
                "Ssid", SsidValue (ssid));
    
    NetDeviceContainer staDevices;
    staDevices = wifi.Install (phy, macsta, wifiStaNodes);

    NetDeviceContainer apDevices;
    apDevices = wifi.Install (phy, macap, wifiApNode);

    MobilityHelper mobility;

    mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
                                    "MinX", DoubleValue (6.0),
                                    "MinY", DoubleValue (17.0),
                                    "DeltaX", DoubleValue (16.0),
                                    "DeltaY", DoubleValue (30.0),
                                    "GridWidth", UintegerValue (2),
                                    "LayoutType", StringValue ("RowFirst"));

    mobility.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
                                "Bounds", RectangleValue (Rectangle (-50, 50, -50, 50)));

    mobility.Install (wifiStaNodes);

    mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
    mobility.Install (wifiApNode);


    Ptr<ConstantPositionMobilityModel> apMobility = wifiApNode.Get(0)->GetObject<ConstantPositionMobilityModel>();
    Ptr<RandomWalk2dMobilityModel> staMobility1 = wifiStaNodes.Get(0)->GetObject<RandomWalk2dMobilityModel>();
    Ptr<RandomWalk2dMobilityModel> staMobility2 = wifiStaNodes.Get(1)->GetObject<RandomWalk2dMobilityModel>();
    Ptr<RandomWalk2dMobilityModel> staMobility3 = wifiStaNodes.Get(2)->GetObject<RandomWalk2dMobilityModel>();

    Vector3D apPosition = apMobility->GetPosition();
    Vector3D staPosition1 = staMobility1->GetPosition();
    Vector3D staPosition2 = staMobility2->GetPosition();
    Vector3D staPosition3 = staMobility3->GetPosition();

    double distance1 = sqrt(pow(apPosition.x - staPosition1.x, 2) + pow(apPosition.y - staPosition1.y, 2));
    double distance2 = sqrt(pow(apPosition.x - staPosition2.x, 2) + pow(apPosition.y - staPosition2.y, 2));
    double distance3 = sqrt(pow(apPosition.x - staPosition3.x, 2) + pow(apPosition.y - staPosition3.y, 2));

    std::cout << "Distância entre AP e STA1: " << distance1 << std::endl;
    std::cout << "Distância entre AP e STA2: " << distance2 << std::endl;
    std::cout << "Distância entre AP e STA3: " << distance3 << std::endl;

    InternetStackHelper stack;
    stack.Install (wifiApNode);
    stack.Install (wifiStaNodes);
 

    Ipv4AddressHelper address;

    address.SetBase ("10.1.3.0", "255.255.255.0");
    Ipv4InterfaceContainer staNodeInterface;
    Ipv4InterfaceContainer apNodeInterface;
    staNodeInterface = address.Assign (staDevices);
    apNodeInterface = address.Assign (apDevices);

    std::cout << "----------------------------------------" << std::endl;
    std::cout << "Ponto de Acesso IP: " << apNodeInterface.GetAddress (0) << std::endl;
    std::cout << "Dispositivo 1 IP: " << staNodeInterface.GetAddress (0) << std::endl;
    std::cout << "Dispositivo 2 IP: " << staNodeInterface.GetAddress (1) << std::endl;
    std::cout << "Dispositivo 3 IP: " << staNodeInterface.GetAddress (2) << std::endl;

    Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

    uint16_t port = 9;

    OnOffHelper onoff ("ns3::UdpSocketFactory", 
                        Address (InetSocketAddress (apNodeInterface.GetAddress (0), port)));
    onoff.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
    onoff.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
    onoff.SetAttribute ("DataRate", StringValue ("10Mbps"));


    ApplicationContainer apps = onoff.Install (wifiStaNodes.Get (0));
    apps.Start (Seconds (1.0));
    apps.Stop (Seconds (10.0));


    PacketSinkHelper sink ("ns3::UdpSocketFactory", 
                            Address (InetSocketAddress (Ipv4Address::GetAny (), port)));
    apps = sink.Install (wifiApNode);
    apps.Start (Seconds (1.0));
    apps.Stop (Seconds (10.0));


    port = 10;

    onoff.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
    onoff.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
    onoff.SetAttribute ("DataRate", StringValue ("10Mbps"));

    apps = onoff.Install (wifiStaNodes.Get (1));
    apps.Start (Seconds (1.0));
    apps.Stop (Seconds (10.0));


    sink.SetAttribute ("Local", AddressValue (InetSocketAddress (Ipv4Address::GetAny (), port)));
    apps = sink.Install (wifiApNode);
    apps.Start (Seconds (1.0));
    apps.Stop (Seconds (10.0));


    port = 11;

    onoff.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
    onoff.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
    onoff.SetAttribute ("DataRate", StringValue ("10Mbps"));

    apps = onoff.Install (wifiStaNodes.Get (2));
    apps.Start (Seconds (1.0));
    apps.Stop (Seconds (10.0));


    sink.SetAttribute ("Local", AddressValue (InetSocketAddress (Ipv4Address::GetAny (), port)));
    apps = sink.Install (wifiApNode);
    apps.Start (Seconds (1.0));
    apps.Stop (Seconds (10.0));
    


    AsciiTraceHelper ascii;
    phy.EnableAsciiAll (ascii.CreateFileStream ("wifi_project_ap.tr"));
    phy.EnablePcap ("wifi_project_ap", apDevices.Get (0));
    


    FlowMonitorHelper flowmon;
    Ptr<FlowMonitor> monitor = flowmon.InstallAll ();


    pAnim = new AnimationInterface ("wifi_project.xml");

    pAnim->UpdateNodeDescription(wifiApNode.Get(0), "Ponto_de_Acesso");
    pAnim->UpdateNodeDescription(wifiStaNodes.Get(0), "Dispositivo_1");
    pAnim->UpdateNodeDescription(wifiStaNodes.Get(1), "Dispositivo_2");
    pAnim->UpdateNodeDescription(wifiStaNodes.Get(2), "Dispositivo_3");
    pAnim->UpdateNodeColor(wifiApNode.Get(0), 0, 255, 0);

    pAnim->SetBackgroundImage ("/home/abrantedevops/assets/bg.png", 0, 0, 0.14, 0.14, 1.0);


    Simulator::Schedule(Seconds(0.0), &modify);
    Simulator::Stop (Seconds (10.0));

    Simulator::Run ();

    monitor->CheckForLostPackets ();
    Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowmon.GetClassifier ());
    std::map<FlowId, FlowMonitor::FlowStats> stats = monitor->GetFlowStats ();
    for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin (); i != stats.end (); ++i)
    {
        Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (i->first);
        std::cout << " ----------------------------------\n";
        std::cout << "Flow " << i->first << " (UDP " << t.sourceAddress << "/49153" << " ----> " << t.destinationAddress << "/9" << ")\n";
        std::cout << "  Tx Packets: " << i->second.txPackets << "\n";
        std::cout << "  Tx Bytes: " << i->second.txBytes << "\n";
        std::cout << "  Rx Packets: " << i->second.rxPackets << "\n";
        std::cout << "  Rx Bytes: " << i->second.rxBytes << "\n";
        std::cout << "  Throughput: " << i->second.rxBytes * 8.0 / 10.0 / 1024 / 1024 << " Mbps\n";
        double meanDelayMs = i->second.delaySum.GetSeconds() / i->second.rxPackets * 1000;
        std::cout << "  Mean Delay: " << std::fixed << std::setprecision(5) << meanDelayMs << " ms\n";
        std::cout << "  Pkt Lost Ratio: ";
        if (i->second.rxPackets > 0) {
        double pktLostRatio = ((double)i->second.txPackets - (double)i->second.rxPackets) / (double)i->second.txPackets * 10;
        std::cout << std::fixed << std::setprecision(1) << pktLostRatio << "%\n";
        } else {
        std::cout << "100%\n";
        }
    }

    monitor->SerializeToXmlFile ("wifi_project.flowmon", true, true);

    std::string fileNameWithNoExtension = "wifi_project_tr";
    std::string graphicsFileName        = fileNameWithNoExtension + ".eps";
    std::string plotFileName            = fileNameWithNoExtension + ".plt";
    std::string plotTitle               = "Wifi Project - IFPB";
    std::string dataTitle               = "Throughtput";

    Gnuplot plot (graphicsFileName);
    plot.SetTitle (plotTitle);

    plot.SetTerminal ("eps");
    plot.SetLegend ("Time (s)", "Throughtput (Mbps)");
    plot.AppendExtra ("set xrange [0:5]");

    Gnuplot2dDataset dataset;
    dataset.SetTitle (dataTitle);
    dataset.SetStyle (Gnuplot2dDataset::LINES_POINTS);
    dataset.SetExtra ("pointtype 7 pointsize 1 pointinterval 0.5 lc rgb 'blue'");


    double x = 0;
    double y = 0;

    for (uint32_t i = 0; i < stats.size (); ++i){
        x = i;
        y = stats[i].rxBytes * 8.0 / 10.0 / 1024 / 1024;
        dataset.Add (x, y);

        if (y != 0) {
        std::stringstream label;
        label << std::fixed << std::setprecision(3) << y;
        plot.AppendExtra("set label '" + label.str() + "' at " + std::to_string(x) + "," + std::to_string(y) + " offset 0.5,0.5 font ',10'");
        }
    }

    plot.AddDataset (dataset);
    std::ofstream plotFile (plotFileName.c_str ());
    plot.GenerateOutput (plotFile);
    plotFile.close ();



    std::string fileNameWithNoExtension2 = "wifi_project_de";
    std::string graphicsFileName2        = fileNameWithNoExtension2 + ".eps";
    std::string plotFileName2            = fileNameWithNoExtension2 + ".plt";
    std::string plotTitle2               = "Wifi Project - IFPB";
    std::string dataTitle2               = "Mean Delay";

    Gnuplot plot2 (graphicsFileName2);
    plot2.SetTitle (plotTitle2);

    plot2.SetTerminal ("eps");
    plot2.SetLegend ("Time (s)", "Mean Delay (ms)");
    plot2.AppendExtra ("set xrange [0:4]");
    

    Gnuplot2dDataset dataset2;
    dataset2.SetTitle (dataTitle2);
    dataset2.SetStyle (Gnuplot2dDataset::LINES_POINTS);
    dataset2.SetExtra ("pointtype 13 pointsize 1 pointinterval 0.5 lc rgb 'red'");

    double x2 = 0;
    double y2 = 0;

    for (uint32_t i = 0; i < stats.size (); ++i){
        x2 = i;
        y2 = stats[i].delaySum.GetSeconds() / stats[i].rxPackets * 1000;
        dataset2.Add (x2, y2);

        if (!std::isnan(y2)) {
        std::stringstream label;
        label << std::fixed << std::setprecision(6) << y2;
        plot2.AppendExtra("set label '" + label.str() + "' at " + std::to_string(x2) + "," + std::to_string(y2) + " offset 0.5,0.5 font ',10'");
        }
    }

    plot2.AddDataset (dataset2);
    std::ofstream plotFile2 (plotFileName2.c_str ());
    plot2.GenerateOutput (plotFile2);
    plotFile2.close ();

    Simulator::Destroy ();
    return 0;

}