#include "ns3/core-module.h"
#include "ns3/mobility-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/animation-interface.h"
#include "ns3/wifi-module.h"
#include "ns3/udp-socket-factory.h"
#include "ns3/packet-sink-helper.h"
#include "ns3/packet-sink.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/on-off-helper.h"
#include "ns3/uinteger.h"
#include "ns3/nstime.h"
#include<map>
#include<fstream>
#include<random>
#include<functional>
#include<cmath>
#include <list>
#include <utility>
#include <sstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <fstream>
#include <vector>
#include<chrono>
#include<algorithm>
using namespace ns3;
uint32_t numNodes=100;
uint32_t g_maxChildrenNodeId=0;
uint32_t srcNode=20;
uint32_t destNode=22;
double interval = 1.0;
std::vector<int>path;
std::vector<int> g_path;
std::vector<bool> hasReceivedPacket;
uint32_t numPackets=1;
uint32_t packetSize=5;
NodeContainer nodes;
Ipv4InterfaceContainer i;
std::vector<std::vector<bool>> visitedNodes;
std::vector<Ptr<PacketSink>> sinkApps;
std::map<Ipv4Address, uint32_t> ipToNodeIdMap;
std::multimap<double, double> distanceToVelocityDiff;
double overhead=0.0;

struct Neighbor{

    uint32_t nodeId;
    double linkDuration;
    Neighbor(uint32_t id, double duration) : nodeId(id), linkDuration(duration) {}

};
std::map<uint32_t, std::vector<Neighbor>> node_neighbors;
double CalculateOverhead(NodeContainer nodes)
{
    std::set<uint32_t> uniqueNeighbors;

    for (uint32_t i = 0; i <= 9; ++i) {
        auto it = node_neighbors.find(i);
        if (it != node_neighbors.end()) {
            for (Neighbor &neighbor : it->second) {
                uniqueNeighbors.insert(neighbor.nodeId);
            }
        }
    }

    overhead += uniqueNeighbors.size();
    return overhead/(overhead+numPackets);
}
void UpdateOverhead() {
    overhead = CalculateOverhead(nodes); 
    Simulator::Schedule(Seconds(1.0), &UpdateOverhead);
}



double FindDistance(Vector pos1, Vector pos2) {
    return std::sqrt(std::pow(pos1.x - pos2.x, 2) + std::pow(pos1.y - pos2.y, 2) + std::pow(pos1.z - pos2.z, 2));
}
double CalculateAngle(Vector position, Vector destination) {
    return std::atan2(destination.y - position.y, destination.x - position.x) - std::atan2(position.y, position.x);
}
double link_duration(double coverage_range, double distance, Vector velocity1, Vector velocity2, Vector position1, Vector position2, double current_visibility) {
    const double max_visibility = 50.0;
    Ptr<ConstantVelocityMobilityModel> destMobModel = NodeList::GetNode(destNode)->GetObject<ConstantVelocityMobilityModel>();
    Vector destPos = destMobModel->GetPosition();  
    double theta1 = CalculateAngle(position1, destPos);
    double theta2 = CalculateAngle(position2, destPos);
    
    double rel_vx = velocity1.x * std::cos(theta1) - velocity2.x * std::cos(theta2);
    double rel_vy = velocity1.y * std::sin(theta1) - velocity2.y * std::sin(theta2);
    
    double relative_velocity = std::sqrt(rel_vx * rel_vx + rel_vy * rel_vy);

    if (relative_velocity == 0) {
        return 0;
    }

    // Check if the second node is moving towards the destination
    double angle_between_velocity_and_destination = std::acos((velocity2.x * (destPos.x - position2.x) + velocity2.y * (destPos.y - position2.y)) /
                                                               (std::sqrt(velocity2.x * velocity2.x + velocity2.y * velocity2.y) *
                                                                std::sqrt(std::pow(destPos.x - position2.x, 2) + std::pow(destPos.y - position2.y, 2))));

    double link_duration_val;
    if (angle_between_velocity_and_destination < M_PI / 2) {
        // Second node is moving towards the destination
        link_duration_val = ((coverage_range + distance) / relative_velocity) * (current_visibility / max_visibility);
    } else {
        // Second node is moving away from the destination
        link_duration_val = ((coverage_range - distance) / relative_velocity) * (current_visibility / max_visibility);
    }

    return link_duration_val;
}
void SortNeighborsByLinkDuration() {
    for (auto& entry : node_neighbors) {
        std::sort(entry.second.begin(), entry.second.end(), [](const Neighbor& a, const Neighbor& b) {
            if (a.nodeId == destNode) return true;  // Always place destination node first.
            if (b.nodeId == destNode) return false;
            return a.linkDuration < b.linkDuration;
        });
    }
}
void UpdateNeighborMap(NodeContainer nodes) {
    node_neighbors.clear();

    const double default_coverage_range = 200.0;
    const double extended_coverage_range = 300.0;
    const double visibility = 10.0;

    for (uint32_t i = 0; i < nodes.GetN(); i++) {
        Ptr<ConstantVelocityMobilityModel> mobModel_i = nodes.Get(i)->GetObject<ConstantVelocityMobilityModel>();
        Vector position_i = mobModel_i->GetPosition();
        Vector velocity_i = mobModel_i->GetVelocity();

        Ptr<ConstantVelocityMobilityModel> mobModel_dest = nodes.Get(destNode)->GetObject<ConstantVelocityMobilityModel>();
        Vector position_dest = mobModel_dest->GetPosition();
        Vector velocity_dest = mobModel_dest->GetVelocity();

        double current_coverage_range_for_dest = (i < 10 || destNode < 10) ? extended_coverage_range : default_coverage_range;
        double distance_to_dest = FindDistance(position_i, position_dest);
        double duration_to_dest = link_duration(current_coverage_range_for_dest, distance_to_dest, velocity_i, velocity_dest, position_i, position_dest, visibility);

        if (distance_to_dest <= current_coverage_range_for_dest && duration_to_dest > 0) {
            node_neighbors[i].insert(node_neighbors[i].begin(), Neighbor(destNode, duration_to_dest));
        }

        for (uint32_t j = 0; j < nodes.GetN(); j++) {
            if (i == j) {
                continue;  // Skip self
            }

            Ptr<ConstantVelocityMobilityModel> mobModel_j = nodes.Get(j)->GetObject<ConstantVelocityMobilityModel>();
            Vector position_j = mobModel_j->GetPosition();
            Vector velocity_j = mobModel_j->GetVelocity();

            // Adjust the coverage range based on whether node is RSU or vehicle
            double current_coverage_range;
            if (i < 10 || j < 10) {
                current_coverage_range = extended_coverage_range;
            } else {
                current_coverage_range = default_coverage_range;
            }

            if (i >= 10 && j >= 10 && node_neighbors[i].size() >= 5) {
                continue; // Limit to 5 neighbors for nodes > 9 (non RSU nodes)
            }

            double distance = FindDistance(position_i, position_j);
            double duration = link_duration(current_coverage_range, distance, velocity_i, velocity_j, position_i, position_j, visibility);

            if (distance <= current_coverage_range && duration > 0) {
                node_neighbors[i].emplace_back(j, duration);
            }
        }
    }

    // Ensure nodes 0 to 9 are connected with each other with link duration of 0
    for (uint32_t i = 0; i < 10; i++) {
        for (uint32_t j = i + 1; j < 10; j++) {
            node_neighbors[i].emplace_back(j, 0.0);
            node_neighbors[j].emplace_back(i, 0.0);
        }
    }

    SortNeighborsByLinkDuration();
    Simulator::Schedule(Seconds(1.0), &UpdateNeighborMap, nodes);
}
void PrintMapToFile() {
    std::ofstream outFile("node_neighbors.txt", std::ios_base::app); 
    for (const auto& pair : node_neighbors) {
        outFile << "Node " << pair.first << ": ";
        for (const Neighbor& neighbor : pair.second) {
            outFile << neighbor.nodeId << "(" << neighbor.linkDuration << "s) ";
        }
        outFile << "\n";
    }
    outFile << "------------------\n"; 
    outFile.close();
    Simulator::Schedule(Seconds(5.0), &PrintMapToFile);
}
/*void SendMessage()
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(65432);
    inet_pton(AF_INET, "127.0.0.1", &(servaddr.sin_addr));

    connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr));

    const char *message = "file created";
    send(sockfd, message, strlen(message), 0);

    char buffer[1024] = {0};
    ssize_t bytesRead = read(sockfd, buffer, 1024);

    if (bytesRead > 0) {
        std::cout << "Server response: " << buffer << " At time: " << Simulator::Now().GetSeconds() << "\n";
        
        std::vector<int> path;
        std::stringstream ss(buffer);
        std::string token;
        while (std::getline(ss, token, ',')) {
            path.push_back(std::stoi(token));
        }

        std::cout << "Received path: ";
        for (int node_id : path) {
            std::cout << node_id << " ";
        }
        std::cout << std::endl;

        g_path = path;

    } else {
        std::cerr << "Error reading from server." << std::endl;
    }

    close(sockfd);
}*/

void PrintPosition(NodeContainer nodes) {
    Ptr<ConstantVelocityMobilityModel> mobModel;
    for (uint32_t i = 0; i < nodes.GetN(); i++) {
        mobModel = nodes.Get(i)->GetObject<ConstantVelocityMobilityModel>();
        Vector position = mobModel->GetPosition();
        Vector velocity = mobModel->GetVelocity();
        std::cout << "Node:" << i << " ";
        std::cout << "Position: " << position << " ";
        std::cout << "Velocity " << velocity << " ";
        std::cout << "At time " << Simulator::Now().GetSeconds() << std::endl;
    }
    Simulator::Schedule(Seconds(1.0), &PrintPosition, nodes);
}
void GenerateNeighborFile(NodeContainer nodes) {
    std::ofstream outFile("neighbors.csv");

    outFile << "current_node_id,current_node_speed,current_node_visibility,destination_speed,current_node_angle_from_destination,";
    outFile << "neighbor_1_id,neighbor_1_speed,neighbor_1_distance,neighbor_1_angle_from_destination,neighbor_1_link_duration,";
    outFile << "neighbor_2_id,neighbor_2_speed,neighbor_2_distance,neighbor_2_angle_from_destination,neighbor_2_link_duration,";
    outFile << "neighbor_3_id,neighbor_3_speed,neighbor_3_distance,neighbor_3_angle_from_destination,neighbor_3_link_duration,";
    outFile << "neighbor_4_id,neighbor_4_speed,neighbor_4_distance,neighbor_4_angle_from_destination,neighbor_4_link_duration,";
    outFile << "neighbor_5_id,neighbor_5_speed,neighbor_5_distance,neighbor_5_angle_from_destination,neighbor_5_link_duration\n";

    const double current_visibility = 10.0;  

    for (const auto& entry : node_neighbors) {
        uint32_t current_node_id = nodes.Get(entry.first)->GetId();
        Ptr<ConstantVelocityMobilityModel> currentMobModel = nodes.Get(entry.first)->GetObject<ConstantVelocityMobilityModel>();
        Vector current_velocity = currentMobModel->GetVelocity();

        Ptr<ConstantVelocityMobilityModel> destMobModel = nodes.Get(destNode)->GetObject<ConstantVelocityMobilityModel>();
        Vector dest_velocity = destMobModel->GetVelocity();
        Vector dest_position = destMobModel->GetPosition();
        double angle_to_dest = CalculateAngle(currentMobModel->GetPosition(), dest_position);

        outFile << current_node_id << "," << current_velocity << "," << current_visibility << "," << dest_velocity << "," << angle_to_dest << ",";

        for (size_t i = 0; i < 5; i++) {
            if (i < entry.second.size()) {
                const Neighbor& neighbor = entry.second[i];
                uint32_t neighbor_id = nodes.Get(neighbor.nodeId)->GetId();
                Ptr<ConstantVelocityMobilityModel> neighborMobModel = nodes.Get(neighbor.nodeId)->GetObject<ConstantVelocityMobilityModel>();
                Vector neighbor_position = neighborMobModel->GetPosition();
                Vector neighbor_velocity = neighborMobModel->GetVelocity();
                double distance_to_neighbor = FindDistance(currentMobModel->GetPosition(), neighbor_position);
                double angle_to_neighbor = CalculateAngle(currentMobModel->GetPosition(), neighbor_position);
                
                outFile << neighbor_id << "," << neighbor_velocity << "," << distance_to_neighbor << "," << angle_to_neighbor << "," << neighbor.linkDuration;
            } else {
                outFile << "9999999,9999999,9999999,9999999,9999999";
            }

            if (i != 4) outFile << ",";
        }
        outFile << "\n";
    }
    
    outFile.close();
}
void PacketReceivedCallback(Ptr<const Packet> packet, const Address &addr)
    {   
    InetSocketAddress inetAddr = InetSocketAddress::ConvertFrom(addr);
    Ipv4Address ipAddr = inetAddr.GetIpv4();
    
    uint32_t nodeId = ipToNodeIdMap[ipAddr];

    std::cout << "Received packet of size " << packet->GetSize() 
              << " bytes from node ID " << nodeId << std::endl;
    }



   void SetupOnOffApplications(NodeContainer nodes, Ipv4InterfaceContainer interfaces, 
                            std::string protocol, uint16_t port, DataRate dataRate, 
                            Time initialStartTime, Time durationPerNode){
    uint32_t pathLength = g_path.size();

    if (pathLength < 2)
    {
        NS_LOG_UNCOND("At least two nodes are required for communication.");
        return;
    }

    OnOffHelper onOffHelper(protocol, Address());
    onOffHelper.SetAttribute("DataRate", DataRateValue(dataRate));
    onOffHelper.SetAttribute("PacketSize", UintegerValue(512));

    PacketSinkHelper packetSinkHelper(protocol, InetSocketAddress(Ipv4Address::GetAny(), port));

    Time currentStartTime = initialStartTime;
    for (uint32_t i = 0; i < pathLength - 1; i++)
    {
        ipToNodeIdMap[interfaces.GetAddress(g_path[i + 1])] = g_path[i+1];
        Ptr<Node> senderNode = nodes.Get(g_path[i]);
        Ptr<Node> receiverNode = nodes.Get(g_path[i+1]);
        
        AddressValue remoteAddress(InetSocketAddress(interfaces.GetAddress(g_path[i + 1]), port));
        onOffHelper.SetAttribute("Remote", remoteAddress);

        ApplicationContainer senderApps = onOffHelper.Install(senderNode);
        senderApps.Start(currentStartTime);
        senderApps.Stop(currentStartTime + durationPerNode);

        ApplicationContainer receiverApps = packetSinkHelper.Install(receiverNode);
        receiverApps.Start(currentStartTime);
        receiverApps.Stop(currentStartTime + durationPerNode);
        Ptr<PacketSink> sink = DynamicCast<PacketSink>(receiverApps.Get(0));
        sink->TraceConnectWithoutContext("Rx", MakeCallback(&PacketReceivedCallback));
        sinkApps.push_back(sink);

        currentStartTime += durationPerNode;
    }
}


 void PrintReceivedBytes()
    {
        for (auto &sink : sinkApps)
        {
            std::cout << "Total bytes received: " << sink->GetTotalRx() << " bytes" << std::endl;
        }
    }
 void runApps()
{
    Time now = Simulator::Now();
    SetupOnOffApplications(nodes, i, "ns3::UdpSocketFactory", 8080, 
                            DataRate("1Mbps"), now, now + Seconds(0.5));
}
void reduceSpeed(NodeContainer nodes)
{
    Ptr<ConstantVelocityMobilityModel> mobModel1;
    Ptr<ConstantVelocityMobilityModel> mobModel2;

    std::multimap<double, double> distanceToVelocityDiff;

    for(uint32_t i=0; i<numNodes-1; i++)
    {
        mobModel1 = nodes.Get(i)->GetObject<ConstantVelocityMobilityModel>();

        for(uint32_t j=i+1; j<numNodes-2; j++)
        {
            mobModel2 = nodes.Get(j)->GetObject<ConstantVelocityMobilityModel>();

            Vector pos1 = mobModel1->GetPosition();
            Vector pos2 = mobModel2->GetPosition();
            double dist = FindDistance(pos1, pos2);
            
            if(dist <= 20)
            {
                Vector v1 = mobModel1->GetVelocity();
                Vector v2 = mobModel2->GetVelocity();
                if((v1.x*v2.x>0)||(v1.y*v2.y>0))
                {

                double speed1 = sqrt(v1.x*v1.x + v1.y*v1.y + v1.z*v1.z);
                double speed2 = sqrt(v2.x*v2.x + v2.y*v2.y + v2.z*v2.z);

                if( speed1>0 && speed2>0 && (speed2 > speed1))
                {
                    double diff = abs((speed2 - speed1) / 2);
                    if(v1.x>0)
                    {
                        v1.x += diff;

                        
                    }
                    else if(v1.y>0)
                    {
                         v1.y +=  diff;

                    }
                    if(v2.x>0)
                    {
                        v2.x-=diff;
                    }
                    else if(v2.y>0)
                    {
                        v2.y-=diff;
                    }

                    mobModel1->SetVelocity(v1);
                    mobModel2->SetVelocity(v2);

                    distanceToVelocityDiff.insert({dist, diff});
                }
            }
        }
    }
    }
    for(auto const &pair : distanceToVelocityDiff)
    {
        std::cout << "Distance: " << pair.first << ", Velocity Difference: " << pair.second << std::endl;
    }
    std::ofstream outFile("speedVsdist.txt");
    if(outFile.is_open())
    {
        for(auto const &pair : distanceToVelocityDiff)
        {
            outFile << "Distance: " << pair.first << ", Velocity Difference: " << pair.second << std::endl;
        }
        outFile.close();
    }
    else
    {
        std::cerr << "Unable to open the file for writing." << std::endl;
    }
        Simulator::Schedule(Seconds(1.0),&reduceSpeed,nodes);

}

int main()
{
    std::ofstream clearFile("node_neighbors.txt", std::ios_base::trunc);

    clearFile.close();
    std::ofstream clearFile2("speedVsdist.txt", std::ios_base::trunc);

    clearFile2.close();
    uint32_t mainRoadYMin=475;
    uint32_t mainRoadYMax=525;
    uint32_t xMin=0;
    uint32_t xMax=6000;
    uint32_t yMin=0;
    uint32_t yMax=1000;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(10, 15);
    nodes.Create(numNodes);
    hasReceivedPacket.assign(numNodes, false);
    visitedNodes.resize(numNodes, std::vector<bool>(numNodes, false));
    InternetStackHelper stack;
    stack.Install(nodes);
    WifiHelper wifi;
    wifi.SetStandard(WIFI_PHY_STANDARD_80211b);
    YansWifiPhyHelper wifiPhy = YansWifiPhyHelper::Default();
    wifiPhy.Set("TxPowerStart", DoubleValue(25)); 
    wifiPhy.Set("TxPowerEnd", DoubleValue(25)); 
    WifiMacHelper wifiMac;
    YansWifiChannelHelper wifiChannel = YansWifiChannelHelper::Default();
    wifiPhy.SetChannel(wifiChannel.Create());
    NetDeviceContainer devices = wifi.Install(wifiPhy, wifiMac, nodes);
    Ipv4AddressHelper ipv4;
    ipv4.SetBase("10.0.0.0", "255.0.0.0");
    i = ipv4.Assign(devices);
    MobilityHelper mobility;
    mobility.SetMobilityModel("ns3::ConstantVelocityMobilityModel");
    mobility.Install(nodes);
    Ptr<ConstantVelocityMobilityModel>mobModel;

    for( uint32_t i=0;i<numNodes;i++)
    {
        //double vel = dis(gen);  
        double vel=20;
        mobModel = nodes.Get(i)->GetObject<ConstantVelocityMobilityModel>();
        if (i==0)
        {
            mobModel->SetPosition(Vector(500,500,0));
            mobModel->SetVelocity(Vector(0,0,0));

        }
        else if (i==1)
        {
            mobModel->SetPosition(Vector(1100,475,0));
            mobModel->SetVelocity(Vector(0,0,0));
        }
        else if (i==2)
        {
            mobModel->SetPosition(Vector(2000,525,0));
            mobModel->SetVelocity(Vector(0,0,0));
        }
        else if (i==3)
        {
            mobModel->SetPosition(Vector(2600,475,0));
            mobModel->SetVelocity(Vector(0,0,0));
        }
        else if (i==4)
        {
            mobModel->SetPosition(Vector(3500,525,0));
            mobModel->SetVelocity(Vector(0,0,0));
        }
        else if (i==5)
        {
            mobModel->SetPosition(Vector(4000,475,0));
            mobModel->SetVelocity(Vector(0,0,0));
        }
        else if (i==6)
        {
            mobModel->SetPosition(Vector(4600,525,0));
            mobModel->SetVelocity(Vector(0,0,0));
        }
        else if (i==7)
        {
            mobModel->SetPosition(Vector(5200,475,0));
            mobModel->SetVelocity(Vector(0,0,0));
        }
        else if (i==8)
        {
            mobModel->SetPosition(Vector(5800,525,0));
            mobModel->SetVelocity(Vector(0,0,0));
        }
        else if (i==9)
        {
            mobModel->SetPosition(Vector(6000,475,0));
            mobModel->SetVelocity(Vector(0,0,0));
        }
        else if(i>9 && i<29)
        {
            mobModel->SetPosition(Vector(xMin+i*10,487.5,0));
            mobModel->SetVelocity(Vector(vel,0,0));
        }
        else if (i>=30 && i<50)
        {
            mobModel->SetPosition(Vector(xMax-i*10,512.5,0));
            mobModel->SetVelocity(Vector(-vel,0,0));

        }
        else if(i>=50 && i< 55)
        {
            mobModel->SetPosition(Vector(yMax-i,487.5,0));
            mobModel->SetVelocity(Vector(0,-vel,0));
        }
        else if(i>=56 && i<60)
        {
            if (i<58)
            {
                mobModel->SetPosition(Vector(493.75,900-i,0));
                mobModel->SetVelocity(Vector(0,-vel,0));
            }
            else{
                mobModel->SetPosition(Vector(506.25,525+i,0));
                mobModel->SetVelocity(Vector(0,vel,0));
            }
        }
        else if(i>=60 && i<75)
        {
            if(i<62)
            {
                mobModel->SetPosition(Vector(1091,475-i,0));
                mobModel->SetVelocity(Vector(0,-vel,0));
            }
            else
            {
                mobModel->SetPosition(Vector(1106,10+i,0));
                mobModel->SetVelocity(Vector(0,vel,0));
                
            }

        }
        else if(i>=75 && i<80)
        {
            if(i<77)
            {
                mobModel->SetPosition(Vector(1993.75,525+i,0));
                mobModel->SetVelocity(Vector(0,vel,0));
            }
            else
            {
                mobModel->SetPosition(Vector(2006.25,900-i,0));
                mobModel->SetVelocity(Vector(0,-vel,0));
                
            }

        }
        else if (i>=80 && i<85)
        {
            if(i<87)
            {
                mobModel->SetPosition(Vector(2593.75,525+i,0));
                mobModel->SetVelocity(Vector(0,vel,0));


            }
            else
            {
                mobModel->SetPosition(Vector(2606.25,900-i,0));
                mobModel->SetVelocity(Vector(0,-vel,0));

            }
        

        }
        else if(i>=85 && i<90)
        {
            if(i<88)
            {
                mobModel->SetPosition(Vector(3493.75,525+i,0));
                mobModel->SetVelocity(Vector(0,vel,0));
            }
            else
            {
                mobModel->SetPosition(Vector(3506.25,900-i,0));
                mobModel->SetVelocity(Vector(0,-vel,0));

            }

        }
        else if(i>=90 && i<95)
        {
            if(i<97)
            {
                mobModel->SetPosition(Vector(3993.75,525+i,0));
                mobModel->SetVelocity(Vector(0,vel,0));

            }
            else
            {
                mobModel->SetPosition(Vector(4006.25,900-i,0));
                mobModel->SetVelocity(Vector(0,-vel,0));

            }
            

        }
        else if(i>=95 && i<100)
        {
            if(i<98)
            {
                mobModel->SetPosition(Vector(4593.75,525+i,0));
                mobModel->SetVelocity(Vector(0,vel,0));

            }
            else
            {
                mobModel->SetPosition(Vector(4606.25,900-i,0));
                mobModel->SetVelocity(Vector(0,-vel,0));

            }


        }
        else if(i>=100 && i<105)
        {
            if(i<103)
            {
                mobModel->SetPosition(Vector(5193.75,525+i,0));
                mobModel->SetVelocity(Vector(0,vel,0));

            }
            else
            {
                mobModel->SetPosition(Vector(5206.25,900-i,0));
                mobModel->SetVelocity(Vector(0,-vel,0));

            }
            

        }
        else if (i>=105 && i<=159){
            if(i<108)
            {
                mobModel->SetPosition(Vector(5793.75,525+i,0));
                mobModel->SetVelocity(Vector(0,vel,0));

            }
            else
            {
                mobModel->SetPosition(Vector(5806.25,900-i,0));
                mobModel->SetVelocity(Vector(0,-vel,0));

            }
        }
    }
    AnimationInterface anim("jp1v2.xml");
    Simulator::Stop(Seconds(10.0));
    //Simulator::Schedule(Seconds(3.0), &SendMessage);
    Simulator::Schedule(Seconds(1.0), &UpdateNeighborMap, nodes);
    Simulator::Schedule(Seconds(1.0), &PrintMapToFile);
    Simulator::Schedule(Seconds(2.0), &GenerateNeighborFile, nodes);
    Simulator::Schedule(Seconds(5.0),runApps);
    Simulator::Schedule(Seconds(1.0), &UpdateOverhead);

    Simulator::Schedule(Seconds(1.0),&reduceSpeed,nodes);
    FlowMonitorHelper flowmonHelper;
    Ptr<FlowMonitor> monitor = flowmonHelper.InstallAll();
    Simulator::Run();
    PrintReceivedBytes();
    monitor->CheckForLostPackets();
    Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier>(flowmonHelper.GetClassifier());
    std::map<FlowId, FlowMonitor::FlowStats> stats = monitor->GetFlowStats();

    double totalDelay = 0.0;
    double totalPDR = 0.0;
    uint32_t flowCount = 0;

    for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator iter = stats.begin(); iter != stats.end(); ++iter)
    {
        Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow(iter->first);

        double meanDelay = iter->second.delaySum.GetSeconds() / iter->second.rxPackets;
        double pdr = double(iter->second.rxPackets) / double(iter->second.txPackets);

        totalDelay += meanDelay;
        totalPDR += pdr;
        flowCount++;
    }

    double averageDelay = flowCount ? totalDelay / flowCount : 0.0;
    double averagePDR = flowCount ? totalPDR / flowCount : 0.0;

    std::cout << "Overall average delay: " << averageDelay << " seconds" << std::endl;
    std::cout << "Overall average PDR: " << averagePDR * 100 << "%" << std::endl;
    std::cout<<"Overhead"<<overhead;




    return 0;
}
