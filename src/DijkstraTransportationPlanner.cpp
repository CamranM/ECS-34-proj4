#include "DijkstraTransportationPlanner.h"
#include "TransportationPlannerConfig.h"
#include "GeographicUtils.h"
#include "DijkstraPathRouter.h"
#include "BusSystemIndexer.h"

#include <vector>
#include <string>
#include <memory>
#include <algorithm>
#include <unordered_map>

#include <iostream>

struct CDijkstraTransportationPlanner::SImplementation
{

    // make the key easier to reference
    using Key = std::pair<CDijkstraPathRouter::TVertexID, CDijkstraPathRouter::TVertexID>;
    // using nodeID = CStreetMap::TNodeID;
    using way = std::shared_ptr<CStreetMap::SWay>;

    struct EdgeInfo
    {

        bool isBus = false;
        bool isWalk = true;
        double distance = 0.0;
        double time = 0.0;
        double speedlimit = 0.0;
    };

    // function derived from https://en.cppreference.com/w/cpp/string/basic_string/stol
    struct EdgeInfoHash
    {
        std::size_t operator()(const Key &e) const noexcept
        {
            std::size_t h1 = std::hash<CDijkstraPathRouter::TVertexID>{}(e.first);
            std::size_t h2 = std::hash<CDijkstraPathRouter::TVertexID>{}(e.second);
            return h1 ^ (h2 << 1); // this combines the hashes
        }
    };

    std::shared_ptr<CStreetMap> StreetMap;
    CDijkstraPathRouter DistRouter;
    CDijkstraPathRouter BikeTimeRouter;
    CDijkstraPathRouter WBTimeRouter;
    std::shared_ptr<STransportationPlannerConfig> Sconfig;
    std::shared_ptr<CBusSystem> SBusSystem;
    std::vector<std::shared_ptr<CStreetMap::SNode>> SortedNodes;
    std::unordered_map<CStreetMap::TNodeID, std::vector<way>> path_detail;
    std::shared_ptr<CBusSystemIndexer> busindex;
    std::unordered_map<CStreetMap::TNodeID, CDijkstraPathRouter::TVertexID> node_to_vertex;
    std::unordered_map<CStreetMap::TNodeID, CDijkstraPathRouter::TVertexID> node_to_vertex_walk;
    std::unordered_map<CStreetMap::TNodeID, CDijkstraPathRouter::TVertexID> node_to_vertex_bike;

    static bool compareID(const std::shared_ptr<CStreetMap::SNode> &a, const std::shared_ptr<CStreetMap::SNode> &b)
    {
        return a->ID() < b->ID();
    }

    SImplementation(std::shared_ptr<SConfiguration> config)
    {

        Sconfig = std::dynamic_pointer_cast<STransportationPlannerConfig>(config);
        StreetMap = Sconfig->StreetMap();
        SBusSystem = Sconfig->BusSystem();
        busindex = std::make_shared<CBusSystemIndexer>(SBusSystem);

        for (size_t i = 0; i < StreetMap->NodeCount(); i++)
        { // check way for
            auto node = StreetMap->NodeByIndex(i);
            SortedNodes.push_back(node);
            // printf("type of node->ID():");
            std::cout << typeid(node->ID()).name() << std::endl;
            node_to_vertex[node->ID()] = DistRouter.AddVertex(node->ID());
            std::cout << "Node id: \n"
                      << node->ID() << std::endl;
            std::cout << "Vertex id: \n"
                      << node_to_vertex[node->ID()] << std::endl;
            node_to_vertex_bike[node->ID()] = BikeTimeRouter.AddVertex(node->ID());
            node_to_vertex_walk[node->ID()] = WBTimeRouter.AddVertex(node->ID());
        }

        std::sort(SortedNodes.begin(), SortedNodes.end(), compareID);

        for (size_t i = 0; i < StreetMap->WayCount(); i++)
        {
            auto way = StreetMap->WayByIndex(i);
            if (way->NodeCount() < 2)
            {
                continue;
            }

            for (size_t j = 0; j < way->NodeCount() - 1; j++)
            {
                auto firstID = way->GetNodeID(j);
                // mapping nodes to ways for easy access after getting path from shortestpath
                path_detail[firstID].push_back(way);

                auto secondID = way->GetNodeID(j + 1);
                path_detail[secondID].push_back(way);
                CStreetMap::SLocation firstloc = StreetMap->NodeByID(firstID)->Location();
                CStreetMap::SLocation secondloc = StreetMap->NodeByID(secondID)->Location();

                double distance = SGeographicUtils::HaversineDistanceInMiles(firstloc, secondloc);
                DistRouter.AddEdge(node_to_vertex[firstID], node_to_vertex[secondID], distance, !(way->HasAttribute("oneway")));
                WBTimeRouter.AddEdge(node_to_vertex_walk[firstID], node_to_vertex_walk[secondID], distance / Sconfig->DWalkSpeed, true); // can always walk both ways
                if (!(way->GetAttribute("bicyle") == "no"))                                                                              // if bikes allowed
                {
                    BikeTimeRouter.AddEdge(node_to_vertex_bike[firstID], node_to_vertex_bike[secondID], distance / Sconfig->DBikeSpeed, !(way->GetAttribute("oneway:bicyle") == "no"));
                }
            }
        }

        for (size_t i = 0; i < StreetMap->NodeCount(); i++)
        {
            std::cout << node_to_vertex[StreetMap->NodeByIndex(i)->ID()] << std::endl;
        }
        printf("Vertext count: \n");
        std::cout << DistRouter.VertexCount() << std::endl;

        for (size_t i = 0; i < SBusSystem->RouteCount(); i++)
        {
            auto route = SBusSystem->RouteByIndex(i);

            // loop through all stops and find the shortest path to stop
            for (size_t j = 0; j < route->StopCount() - 1; j++)
            {
                auto firststopid = route->GetStopID(j);
                auto secondstopid = route->GetStopID(j + 1);
                auto firststop = SBusSystem->StopByID(firststopid)->NodeID();
                auto secondstop = SBusSystem->StopByID(secondstopid)->NodeID();
                way foundway = nullptr;
                double dist;
                double total_time = 0.0;
                std::vector<CDijkstraPathRouter::TVertexID> path;
                dist = DistRouter.FindShortestPath(node_to_vertex[firststop], node_to_vertex[secondstop], path);
                if (!(dist == CPathRouter::NoPathExists))
                {
                    printf("Starting the pair of stops: ");

                    // check for bus route

                    //
                    std::cout << path.size() << std::endl;
                    std::cout << "\nFirst stop: " << node_to_vertex[firststop] << std::endl;
                    std::cout << "\nSecond stop: " << node_to_vertex[secondstop] << std::endl;
                    printf("List of vertex ids in path: \n");
                    for (auto ver : path)
                    {
                        std::cout << ver << "\n"
                                  << std::endl;
                    }
                    // std::cout << path.size() << std::endl;
                    //  std::cout<<typeid(CDijkstraPathRouter::TVertexID).name()<<std::endl;
                    for (size_t k = 0; k < path.size() - 1; k++)
                    {
                        // auto tag1 = DistRouter.GetVertexTag(path[k]);
                        // auto tag2 = DistRouter.GetVertexTag(path[k + 1]);

                        // if (!tag1.has_value() || !tag2.has_value())
                        // {
                        //     std::cout << "Vertex tag missing at index " << k << std::endl;
                        //     continue;
                        // }

                        // auto first_node = std::any_cast<CStreetMap::TNodeID>(tag1);
                        // auto next_node = std::any_cast<CStreetMap::TNodeID>(tag2);

                        // std::cout << "first_node=" << first_node << " next_node=" << next_node << std::endl;
                        // getting errors with just any_cast, trying static_casting:
                        // https://www.geeksforgeeks.org/dsa/implicit-and-explicit-type-casting/

                        // printf("type being returned by getvertextgag:");
                        // std::cout << "k: " << firststop << std::endl;
                        std::cout << "k: " << path[k] << std::endl;
                        std::cout << "k+1: " << path[k + 1] << std::endl;
                        // printf("veretex:");
                        // std::cout << node_to_vertex[firststop] << std::endl;
                        // printf("isnoway right after this:");
                        //  auto first_node = DistRouter.GetVertexTag(path[k]);

                        // auto next_node = DistRouter.GetVertexTag(path[k + 1]);
                        //  std::cout << hi << std::endl;
                        auto first_node = std::any_cast<CStreetMap::TNodeID>(DistRouter.GetVertexTag(path[k]));
                        auto next_node = std::any_cast<CStreetMap::TNodeID>(DistRouter.GetVertexTag(path[k + 1]));
                        // printf("type");
                        // auto checktag1 = std::any_cast<short>(DistRouter.GetVertexTag(path[k]));
                        // auto checktag2 = std::any_cast<short>(DistRouter.GetVertexTag(path[k + 1]));
                        // printf("type");
                        // std::cout << checktag.type().name() << std::endl;
                        // printf("cant get its own path...?:");
                        // auto first_node = std::any_cast<CStreetMap::TNodeID>(DistRouter.GetVertexTag(path[k]));
                        // printf("next one:");
                        // std::cout << (k + 1) << std::endl;
                        // auto next_node = std::any_cast<CStreetMap::TNodeID>(DistRouter.GetVertexTag(path[k + 1]));

                        auto &firstway = path_detail[first_node];
                        auto &secondway = path_detail[next_node];
                        for (auto first : firstway)
                        {
                            for (auto second : secondway)
                            {
                                if (first == second)
                                {
                                    foundway = first;
                                    break;
                                }
                            }
                            if (foundway)
                            {
                                break;
                            }
                        }
                        double speed = Sconfig->DDefaultSpeedLimit;
                        //     printf("how far we getin");
                        if (foundway)
                        {
                            double dist = SGeographicUtils::HaversineDistanceInMiles(StreetMap->NodeByID(first_node)->Location(), StreetMap->NodeByID(next_node)->Location());
                            if (foundway->HasAttribute("maxspeed"))
                            {

                                try
                                {
                                    speed = std::stod(foundway->GetAttribute("maxspeed"));
                                }
                                catch (const std::exception &e)
                                {
                                    // if its not a double, just ignore it and use default speed
                                }
                            }
                        }
                        total_time += dist / speed;

                    } // try 30 seconds
                    WBTimeRouter.AddEdge(node_to_vertex[firststop], node_to_vertex[secondstop], total_time + ((Sconfig->DBusStopTime) / 3600));
                }

                else
                {
                    break;
                }
                path.clear();
            }
        }
    }

    ~SImplementation()
    {
    }

    std::size_t NodeCount() const noexcept
    {
        return SortedNodes.size();
    }

    std::shared_ptr<CStreetMap::SNode> SortedNodeByIndex(std::size_t index) const noexcept
    {
        if (index < SortedNodes.size())
        {
            return SortedNodes[index];
        }

        return nullptr;
    }

    double FindShortestPath(TNodeID src, TNodeID dest, std::vector<TNodeID> &path)
    {
        return 0.0;
        // std::cout << src << std::endl;
        // std::cout << node_to_vertex.count(src) << std::endl;

        // std::cout << "Src: " << src << std::endl;
        // std::cout << "dest: " << dest << std::endl;
        if (node_to_vertex.count(src) > 0 && node_to_vertex.count(dest) > 0)
        {
            // printf("sohere");
            path.clear();

            std::vector<CDijkstraPathRouter::TVertexID> result;
            double dist = DistRouter.FindShortestPath(node_to_vertex[src], node_to_vertex[dest], result);

            // convert results (which are vertex ids) into node ids again
            for (auto id : result)
            {
                path.push_back(std::any_cast<CStreetMap::TNodeID>(DistRouter.GetVertexTag(id)));
            }

            return dist;
        }
        printf("prinitng here short");
        return CPathRouter::NoPathExists;
    }

    double FindFastestPath(TNodeID src, TNodeID dest, std::vector<TTripStep> &path)
    {
        return 0.0;
        bool walk_exist = node_to_vertex_walk.count(src) > 0 && node_to_vertex_walk.count(dest) > 0;
        bool bike_exist = node_to_vertex_bike.count(src) > 0 && node_to_vertex_bike.count(dest) > 0;

        std::vector<CDijkstraPathRouter::TVertexID> walkpath;
        std::vector<CDijkstraPathRouter::TVertexID> bikepath;
        path.clear();

        double walkbus;
        double bike;

        bool walk_chosen = false;

        if (walk_exist && bike_exist) // if both exist, calculate both
        {
            printf("boht exist");
            walkbus = WBTimeRouter.FindShortestPath(node_to_vertex_walk[src], node_to_vertex_walk[dest], walkpath);
            bike = BikeTimeRouter.FindShortestPath(node_to_vertex_bike[src], node_to_vertex_bike[dest], bikepath);

            walk_chosen = walkbus < bike;
            std::cout << "walk faster?:" << walk_chosen << std::endl;
        }
        else if (walk_exist) // only walk/bus
        {
            walkbus = WBTimeRouter.FindShortestPath(node_to_vertex_walk[src], node_to_vertex_walk[dest], walkpath);
            walk_chosen = true;
        }
        else if (bike_exist)
        { // only bike

            bike = BikeTimeRouter.FindShortestPath(node_to_vertex_bike[src], node_to_vertex_bike[dest], bikepath);
        }
        else
        {
            return CPathRouter::NoPathExists;
        }

        if (walk_chosen)
        {
            // return walkbus;
            printf("so walkchose?");
            auto mode = CTransportationPlanner::ETransportationMode::Walk;
            // bool route_exists;
            // route_exists = busindex->RouteBetweenNodeIDs(std::any_cast<CStreetMap::TNodeID>(WBTimeRouter.GetVertexTag(walkpath[0])), std::any_cast<CStreetMap::TNodeID>(WBTimeRouter.GetVertexTag(walkpath[1])));
            // starting node
            auto step = CTransportationPlanner::TTripStep{mode, std::any_cast<CStreetMap::TNodeID>(WBTimeRouter.GetVertexTag(walkpath[0]))};
            path.push_back(step);

            for (size_t i = 0; i < walkpath.size() - 1; i++)
            {
                bool route_exists;
                route_exists = busindex->RouteBetweenNodeIDs(std::any_cast<CStreetMap::TNodeID>(WBTimeRouter.GetVertexTag(walkpath[i])), std::any_cast<CStreetMap::TNodeID>(WBTimeRouter.GetVertexTag(walkpath[i + 1])));
                // if a bus route exists, take the bus (still need to implement what specific route)
                if (route_exists)
                {
                    mode = CTransportationPlanner::ETransportationMode::Bus;
                    step = CTransportationPlanner::TTripStep{mode, std::any_cast<CStreetMap::TNodeID>(WBTimeRouter.GetVertexTag(walkpath[i + 1]))};
                    path.push_back(step);
                }
                else
                {
                    mode = CTransportationPlanner::ETransportationMode::Walk;
                    step = CTransportationPlanner::TTripStep{mode, std::any_cast<CStreetMap::TNodeID>(WBTimeRouter.GetVertexTag(walkpath[i + 1]))};
                    path.push_back(step);
                }
            }
            step = CTransportationPlanner::TTripStep{mode, std::any_cast<CStreetMap::TNodeID>(WBTimeRouter.GetVertexTag(walkpath.back()))};
            path.push_back(step);
            return walkbus;
        }
        printf("we biking"); // 1.5390765243504847
        // return bike;
        printf("is this happening bike??shoudlnt be");
        auto mode = CTransportationPlanner::ETransportationMode::Bike;
        // bool route_exists;
        // route_exists = busindex->RouteBetweenNodeIDs(std::any_cast<CStreetMap::TNodeID>(WBTimeRouter.GetVertexTag(walkpath[0])), std::any_cast<CStreetMap::TNodeID>(WBTimeRouter.GetVertexTag(walkpath[1])));
        // starting node
        auto step = CTransportationPlanner::TTripStep{mode, std::any_cast<CStreetMap::TNodeID>(BikeTimeRouter.GetVertexTag(bikepath[0]))};
        path.push_back(step);

        for (size_t i = 1; i < bikepath.size(); i++)
        {

            step = CTransportationPlanner::TTripStep{mode, std::any_cast<CStreetMap::TNodeID>(WBTimeRouter.GetVertexTag(bikepath[i]))};
            path.push_back(step);
        }
        // auto step = CTransportationPlanner::TTripStep{mode, std::any_cast<CStreetMap::TNodeID>(WBTimeRouter.GetVertexTag(walkpath.back()))};
        // path.push_back(step);
        return bike;
    }

    bool GetPathDescription(const std::vector<TTripStep> &path, std::vector<std::string> &desc) const
    {
        return 0.0;
    }
};
CDijkstraTransportationPlanner::CDijkstraTransportationPlanner(std::shared_ptr<SConfiguration> config)
{
    DImplementation = std::make_unique<SImplementation>(config);
}

CDijkstraTransportationPlanner::~CDijkstraTransportationPlanner()
{
}

std::size_t CDijkstraTransportationPlanner::NodeCount() const noexcept
{
    return DImplementation->NodeCount();
}

std::shared_ptr<CStreetMap::SNode> CDijkstraTransportationPlanner::SortedNodeByIndex(std::size_t index) const noexcept
{
    return DImplementation->SortedNodeByIndex(index);
}

double CDijkstraTransportationPlanner::FindShortestPath(TNodeID src, TNodeID dest, std::vector<TNodeID> &path)
{
    return DImplementation->FindShortestPath(src, dest, path);
}

double CDijkstraTransportationPlanner::FindFastestPath(TNodeID src, TNodeID dest, std::vector<TTripStep> &path)
{
    return DImplementation->FindFastestPath(src, dest, path);
}

bool CDijkstraTransportationPlanner::GetPathDescription(const std::vector<TTripStep> &path, std::vector<std::string> &desc) const
{
    return DImplementation->GetPathDescription(path, desc);
}
