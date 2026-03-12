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
        { // adding nodes to all maps
            auto node = StreetMap->NodeByIndex(i);
            SortedNodes.push_back(node);
            node_to_vertex[node->ID()] = DistRouter.AddVertex(node->ID());

            node_to_vertex_bike[node->ID()] = BikeTimeRouter.AddVertex(node->ID());
            node_to_vertex_walk[node->ID()] = WBTimeRouter.AddVertex(node->ID());
        }

        // make list of sortednodes
        std::sort(SortedNodes.begin(), SortedNodes.end(), compareID);

        for (size_t i = 0; i < StreetMap->WayCount(); i++)
        {
            auto way = StreetMap->WayByIndex(i);

            for (size_t j = 0; j < way->NodeCount() - 1; j++)
            {
                auto firstID = way->GetNodeID(j);
                // mapping nodes to ways for easy access after getting path from shortestpath
                path_detail[firstID].push_back(way);

                auto secondID = way->GetNodeID(j + 1);
                path_detail[secondID].push_back(way);
                CStreetMap::SLocation firstloc = StreetMap->NodeByID(firstID)->Location();
                CStreetMap::SLocation secondloc = StreetMap->NodeByID(secondID)->Location();

                // add the time edges to the respective graphs
                double distance = SGeographicUtils::HaversineDistanceInMiles(firstloc, secondloc);
                DistRouter.AddEdge(node_to_vertex[firstID], node_to_vertex[secondID], distance, !(way->HasAttribute("oneway")));
                WBTimeRouter.AddEdge(node_to_vertex_walk[firstID], node_to_vertex_walk[secondID], distance / Sconfig->DWalkSpeed, true); // can always walk both ways
                if (!(way->GetAttribute("bicyle") == "no"))                                                                              // if bikes allowed
                {
                    BikeTimeRouter.AddEdge(node_to_vertex_bike[firstID], node_to_vertex_bike[secondID], distance / Sconfig->DBikeSpeed, !(way->GetAttribute("oneway:bicyle") == "no"));
                }
            }
        }

        // adding bus route edges now

        for (size_t i = 0; i < SBusSystem->RouteCount(); i++)
        {
            // for every route
            auto route = SBusSystem->RouteByIndex(i);

            // loop through all pairs of consecutive stops (stop1 and stop2, stop 2 and stop3, etc) and find the shortest path to stop
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
                if (!(dist == CPathRouter::NoPathExists)) // check if path exists
                {
                    // check for bus route
                    for (size_t k = 0; k < path.size() - 1; k++)
                    {
                        // get node ids from router
                        auto first_node = std::any_cast<CStreetMap::TNodeID>(DistRouter.GetVertexTag(path[k]));
                        auto next_node = std::any_cast<CStreetMap::TNodeID>(DistRouter.GetVertexTag(path[k + 1]));

                        // check for the way between the nodes and see if there is a speed limit
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
                        // set to default speed
                        double speed = Sconfig->DDefaultSpeedLimit;

                        if (foundway) // if there is a way, check for speed limit
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

                    } // add edge
                    WBTimeRouter.AddEdge(node_to_vertex[firststop], node_to_vertex[secondstop], total_time + ((Sconfig->DBusStopTime) / 3600));
                }

                else // if no path between stops, incorrectly formed bus route (cant have no path between stops)
                {
                    break;
                }
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

        if (node_to_vertex.count(src) > 0 && node_to_vertex.count(dest) > 0)
        { // if valid start and dest

            path.clear(); // clear vector

            std::vector<CDijkstraPathRouter::TVertexID> result;
            double dist = DistRouter.FindShortestPath(node_to_vertex[src], node_to_vertex[dest], result);

            // convert results (which are vertex ids) into node ids again
            for (auto id : result)
            {
                path.push_back(std::any_cast<CStreetMap::TNodeID>(DistRouter.GetVertexTag(id)));
            }

            return dist;
        }
        // not valid
        return CPathRouter::NoPathExists;
    }

    double FindFastestPath(TNodeID src, TNodeID dest, std::vector<TTripStep> &path)
    {
        // check if vertices exist in either path
        bool walk_exist = node_to_vertex_walk.count(src) > 0 && node_to_vertex_walk.count(dest) > 0;
        bool bike_exist = node_to_vertex_bike.count(src) > 0 && node_to_vertex_bike.count(dest) > 0;

        std::vector<CDijkstraPathRouter::TVertexID> walkpath;
        std::vector<CDijkstraPathRouter::TVertexID> bikepath;
        path.clear();

        double walkbus;
        double bike;

        bool walk_chosen = false;

        if (walk_exist && bike_exist) // if both exist, calculate both and choose faster
        {
            walkbus = WBTimeRouter.FindShortestPath(node_to_vertex_walk[src], node_to_vertex_walk[dest], walkpath);
            bike = BikeTimeRouter.FindShortestPath(node_to_vertex_bike[src], node_to_vertex_bike[dest], bikepath);

            walk_chosen = walkbus < bike;
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
            // nodeids dont exist in either walk/bus nor bike paths
            return CPathRouter::NoPathExists;
        }

        if (walk_chosen) // if walk/bus path
        {
            auto mode = CTransportationPlanner::ETransportationMode::Walk; // start with walk

            // starting node
            auto step = CTransportationPlanner::TTripStep{mode, std::any_cast<CStreetMap::TNodeID>(WBTimeRouter.GetVertexTag(walkpath[0]))};
            path.push_back(step);

            // for every node in the walkpath
            for (size_t i = 0; i < walkpath.size() - 1; i++)
            {
                bool route_exists;
                route_exists = busindex->RouteBetweenNodeIDs(std::any_cast<CStreetMap::TNodeID>(WBTimeRouter.GetVertexTag(walkpath[i])), std::any_cast<CStreetMap::TNodeID>(WBTimeRouter.GetVertexTag(walkpath[i + 1])));
                // if a bus route exists, take the bus, always faster
                if (route_exists)
                {
                    // puts correct mode and pushes TTripStep
                    mode = CTransportationPlanner::ETransportationMode::Bus;
                    step = CTransportationPlanner::TTripStep{mode, std::any_cast<CStreetMap::TNodeID>(WBTimeRouter.GetVertexTag(walkpath[i + 1]))};
                    path.push_back(step);
                }
                else // no bus, walk
                {
                    mode = CTransportationPlanner::ETransportationMode::Walk;
                    step = CTransportationPlanner::TTripStep{mode, std::any_cast<CStreetMap::TNodeID>(WBTimeRouter.GetVertexTag(walkpath[i + 1]))};
                    path.push_back(step);
                }
            }

            return walkbus;
        }
        // biking path
        auto mode = CTransportationPlanner::ETransportationMode::Bike;

        // starting node
        auto step = CTransportationPlanner::TTripStep{mode, std::any_cast<CStreetMap::TNodeID>(BikeTimeRouter.GetVertexTag(bikepath[0]))};
        path.push_back(step);

        for (size_t i = 1; i < bikepath.size(); i++)
        {
            // loop through, add bike and nodeid
            step = CTransportationPlanner::TTripStep{mode, std::any_cast<CStreetMap::TNodeID>(WBTimeRouter.GetVertexTag(bikepath[i]))};
            path.push_back(step);
        }

        return bike;
    }

    bool GetPathDescription(const std::vector<TTripStep> &path, std::vector<std::string> &desc) const
    {
        return false;
        if (path.size() < 2)
        { // no path
            return false;
        }

        desc.clear();
        // get the starting node
        auto nodeid = std::any_cast<CStreetMap::TNodeID>(DistRouter.GetVertexTag(path[0].second));

        std::string output = "Start at " + SGeographicUtils::ConvertLLToDMS(StreetMap->NodeByID(nodeid)->Location());
        desc.push_back(output);
        std::string mode;
        std::string prev_mode;
        std::string prev_name;

        double prev_dist;
        bool same_street = false;
        auto prev_node = StreetMap->NodeByID(nodeid); // stores the first node of the street
        for (size_t i = 1; i < path.size(); i++)      // loop through starting at 1, 0 was the starting point
        {
            auto &step = path[i]; // get the TTripstep
            // set the mode
            if (step.first == CTransportationPlanner::ETransportationMode::Bike)
            {
                mode = "Bike";
            }
            else if (step.first == CTransportationPlanner::ETransportationMode::Walk)
            {
                mode = "Walk";
            }
            else
            {
                mode = "Bus";
            }

            auto node = StreetMap->NodeByID(step.second);

            std::string name = "";
            std::string describe = "along"; // default is along
            // put name of street, "" for no noame
            // if name is same as rev_name and dir is same as prev dir
            // increment distance
            // else
            // samestree = false and push output
            // rseet prev_dist to 0
            //
            // if the node has a name set the name to name, other leave blank
            if (node->HasAttribute("name"))
            {
                name = node->GetAttribute("name");
            }
            // if the name is the same, we are on the same street, dont add instructions yet
            same_street = (node->GetAttribute("name") == prev_name);
            prev_dist += SGeographicUtils::HaversineDistanceInMiles(prev_node->Location(), node->Location());
            if (!same_street) // we add the instructions for the prev street and start a new street
            {
                if (name == "") // street name unknown
                {
                    if (i < path.size() - 1) // not at the last step
                    {
                        auto next_node = StreetMap->NodeByID(path[i + 1].second);
                        if (next_node->HasAttribute("name"))
                        { // if the next step has a name
                            name = next_node->GetAttribute("name");
                            describe = "toward"; // set descriptor to toward
                        }
                        else
                        {
                            describe = ""; // so it just says "walk for dist"
                        }
                        // no name, just walk in direction for distance
                    }
                    else // if its the last step, its the end
                    {
                        name = "End";
                    }
                }
                // push instructions
                prev_name = name;
                std::string direction = SGeographicUtils::BearingToDirection(SGeographicUtils::CalculateBearing(prev_node->Location(), node->Location()));
                output = mode + " " + direction + " " + name + " for " + std::to_string(prev_dist) + " mi";
                prev_dist = 0.0;
            }
            // same street, just loop again
        }
        return (!desc.empty());
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
