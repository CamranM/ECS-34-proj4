#include "DijkstraTransportationPlanner.h"
#include "TransportationPlannerConfig.h"
#include "GeographicUtils.h"
#include "DijkstraPathRouter.h"

#include <vector>
#include <string>
#include <memory>
#include <algorithm>

struct CDijkstraTransportationPlanner::SImplementation
{
    std::shared_ptr<CStreetMap> StreetMap;
    CDijkstraPathRouter DistRouter;
    CDijkstraPathRouter BikeTimeRouter;
    CDijkstraPathRouter WBTimeRouter;
    std::shared_ptr<STransportationPlannerConfig> Sconfig;
    std::shared_ptr<CBusSystem> SBusSystem;
    std::vector<std::shared_ptr<CStreetMap::SNode>> SortedNodes;

    struct EdgeInfo
    {
        bool isBus = false;
        bool isWalk = false;
        double distance = 0.0;
        double time = 0.0;
    };

    static bool compareID(const std::shared_ptr<CStreetMap::SNode> &a, const std::shared_ptr<CStreetMap::SNode> &b)
    {
        return a->ID() < b->ID();
    }

    SImplementation(std::shared_ptr<SConfiguration> config)
    {
        Sconfig = std::dynamic_pointer_cast<STransportationPlannerConfig>(config);
        StreetMap = Sconfig->StreetMap();
        SBusSystem = Sconfig->BusSystem();

        // adds nodes to list of nodes, 1 dist finder
        for (size_t i = 0; i < StreetMap->NodeCount(); i++)
        { // check way for
            auto node = StreetMap->NodeByIndex(i);
            SortedNodes.push_back(node);
            DistRouter.AddVertex(node);
            BikeTimeRouter.AddVertex(node);
            WBTimeRouter.AddVertex(node);
        }

        std::sort(SortedNodes.begin(), SortedNodes.end(), compareID);

        // allocate memory ahead of time?

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
                auto secondID = way->GetNodeID(j + 1);
                CStreetMap::SLocation firstloc = StreetMap->NodeByID(firstID)->Location();
                CStreetMap::SLocation secondloc = StreetMap->NodeByID(secondID)->Location();

                double distance = SGeographicUtils::HaversineDistanceInMiles(firstloc, secondloc);
                DistRouter.AddEdge(firstID, secondID, distance, !(way->HasAttribute("oneway")));
                WBTimeRouter.AddEdge(firstID, secondID, distance / Sconfig->DWalkSpeed, true); // can always walk both ways
                if (!(way->GetAttribute("bicyle") == "no"))                                    // if bikes allowed
                {
                    BikeTimeRouter.AddEdge(firstID, secondID, distance / Sconfig->DBikeSpeed, !(way->HasAttribute("oneway")));
                }
            }
        }

        // need to add bus routes and pathing (shortest path, no fastest)
        for (size_t i = 0; i < SBusSystem->RouteCount(); i++)
        {
            auto route = SBusSystem->RouteByIndex(i);

            // loop through all stops and find the shortest path to stop
            for (size_t j = 0; j < route->StopCount() - 1; j++)
            {
                auto firststop = route->GetStopID(j);
                auto secondstop = route->GetStopID(j + 1);

                std::vector<CDijkstraPathRouter::TVertexID> path;
                double dist = DistRouter.FindShortestPath(firststop, secondstop, path);
                WBTimeRouter.AddEdge(firststop, secondstop, dist / Sconfig->DDefaultSpeedLimit);
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
        return DistRouter.FindShortestPath(src, dest, path);
    };

    double FindFastestPath(TNodeID src, TNodeID dest, std::vector<TTripStep> &path)
    {
        std::vector<CDijkstraPathRouter::TVertexID> walkpath;
        std::vector<CDijkstraPathRouter::TVertexID> bikepath;

        double walkbus = WBTimeRouter.FindShortestPath(src, dest, walkpath);
        double bike = BikeTimeRouter.FindShortestPath(src, dest, bikepath);
    };

    bool GetPathDescription(const std::vector<TTripStep> &path, std::vector<std::string> &desc) const {
    };
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