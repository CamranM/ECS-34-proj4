#include "BusSystemIndexer.h"
#include <vector>
#include <unordered_map>
#include <algorithm>

struct CBusSystemIndexer::SImplementation{
    std::shared_ptr<CBusSystem> DBusSystem;
    std::vector<std::shared_ptr<SStop>> DSortedStopsByIndex;
    std::unordered_map<TNodeID,std::shared_ptr<SStop>> DStopsByNodeID;

    SImplementation(std::shared_ptr<CBusSystem> bussystem){
        DBusSystem = bussystem;
        for(size_t Index = 0; Index < DBusSystem->StopCount(); Index++){
            auto Stop = DBusSystem->StopByIndex(Index);
            DSortedStopsByIndex.push_back(Stop);
            DStopsByNodeID[Stop->NodeID()] = Stop;
        }
        std::sort(DSortedStopsByIndex.begin(), DSortedStopsByIndex.end(), [](std::shared_ptr<SStop> l, std::shared_ptr<SStop> r) -> bool{
            return l->ID() < r->ID();
        });
    }

    ~SImplementation(){

    }

    std::size_t StopCount() const noexcept{
        return DBusSystem->StopCount();
    }

    std::size_t RouteCount() const noexcept{
        return DBusSystem->RouteCount();
    }

    std::shared_ptr<SStop> SortedStopByIndex(std::size_t index) const noexcept{
        return DSortedStopsByIndex[index]; // do a check 
    }

    std::shared_ptr<SRoute> SortedRouteByIndex(std::size_t index) const noexcept{
        return nullptr;
    }

    std::shared_ptr<SStop> StopByNodeID(TNodeID id) const noexcept{
        return nullptr;
    }

    bool RoutesByNodeIDs(TNodeID src, TNodeID dest, std::unordered_set<std::shared_ptr<SRoute> > &routes) const noexcept{
        return false;
    }

    bool RouteBetweenNodeIDs(TNodeID src, TNodeID dest) const noexcept{
        return false;
    }

};

CBusSystemIndexer::CBusSystemIndexer(std::shared_ptr<CBusSystem> bussystem){
    DImplementation = std::make_unique<SImplementation>(bussystem);
}

CBusSystemIndexer::~CBusSystemIndexer(){

}

std::size_t CBusSystemIndexer::StopCount() const noexcept{
    return DImplementation->StopCount();
}

std::size_t CBusSystemIndexer::RouteCount() const noexcept{
    return DImplementation->RouteCount();
}

std::shared_ptr<CBusSystemIndexer::SStop> CBusSystemIndexer::SortedStopByIndex(std::size_t index) const noexcept{
    return DImplementation->SortedStopByIndex(index);
}

std::shared_ptr<CBusSystemIndexer::SRoute> CBusSystemIndexer::SortedRouteByIndex(std::size_t index) const noexcept{
    return DImplementation->SortedRouteByIndex(index);
}

std::shared_ptr<CBusSystemIndexer::SStop> CBusSystemIndexer::StopByNodeID(TNodeID id) const noexcept{
    return DImplementation->StopByNodeID(id);
}

bool CBusSystemIndexer::RoutesByNodeIDs(TNodeID src, TNodeID dest, std::unordered_set<std::shared_ptr<SRoute> > &routes) const noexcept{
    return DImplementation->RoutesByNodeIDs(src,dest,routes);
}

bool CBusSystemIndexer::RouteBetweenNodeIDs(TNodeID src, TNodeID dest) const noexcept{
    return DImplementation->RouteBetweenNodeIDs(src,dest);
}
