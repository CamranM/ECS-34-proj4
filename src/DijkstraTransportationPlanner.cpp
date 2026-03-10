#include "DijkstraTransportationPlanner.h"
#include "TransportationPlannerConfig.h"

#include "DijkstraPathRouter.h"

#include <vector>
#include <string>
#include <memory>
#include <algorithm>

struct CDijkstraTransportationPlanner::SImplementation
{
    std::shared_ptr<CStreetMap> StreetMap;
    CDijkstraPathRouter DistRouter;
    CDijkstraPathRouter TimeRouter;
    std::shared_ptr<STransportationPlannerConfig> Sconfig;
    std::shared_ptr<CBusSystem> SBusSystem;
    std::vector<std::shared_ptr<CStreetMap::SNode>> SortedNodes;

    static bool compareID(const std::shared_ptr<CStreetMap::SNode> &a, const std::shared_ptr<CStreetMap::SNode> &b)
    {
        return a->ID() < b->ID();
    }

    SImplementation(std::shared_ptr<SConfiguration> config)
    {
        Sconfig = std::dynamic_pointer_cast<STransportationPlannerConfig>(config);
        StreetMap = Sconfig->StreetMap();
        SBusSystem = Sconfig->BusSystem();

        // adds nodes to list of nodes and 2 path finders
        for (int i = 0; i < StreetMap->NodeCount(); i++)
        {
            auto node = StreetMap->NodeByIndex(i);
            SortedNodes.push_back(node);
            DistRouter.AddVertex(node);
            TimeRouter.AddVertex(node);
        }

        std::sort(SortedNodes.begin(), SortedNodes.end(), compareID);
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

    double FindShortestPath(TNodeID src, TNodeID dest, std::vector<TNodeID> &path) {

    };

    double FindFastestPath(TNodeID src, TNodeID dest, std::vector<TTripStep> &path) {

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