#include "CSVBusSystem.h"
#include <unordered_map>

struct CCSVBusSystem::SImplementation
{

    struct SStop : public CBusSystem::SStop
    {
        TStopID DID;
        CStreetMap::TNodeID DNodeID;

        ~SStop()
        {
        }

        TStopID ID() const noexcept{
            return DID; // returns the stop ID of the stop 
        }

        CStreetMap::TNodeID NodeID() const noexcept{
            return DNodeID; // returns the node of the stop 
        }
    };

    struct SRoute: public CBusSystem::SRoute{
        std::string route_name;
        std::vector<TStopID> Stop_list;
        ~SRoute(){

        }

        std::string Name() const noexcept{
            return route_name; // returns the route name given one SRoute object 
        }

        std::size_t StopCount() const noexcept{
            return Stop_list.size(); // returns the amount of stops given one SRoute object 
        }

        TStopID GetStopID(std::size_t index) const noexcept{ // given the index of a stop, returns which stop it is (within a route)
            if (Stop_list.size() <= index) {
                return CBusSystem::InvalidStopID;
            }
            return Stop_list[index];
            
        }
    };

    const std::string STOP_ID_HEADER = "stop_id";
    const std::string NODE_ID_HEADER = "node_id";

    std::vector<std::shared_ptr<SStop>> DStopsByIndex;
    std::unordered_map<TStopID, std::shared_ptr<SStop>> DStopsByID;

    bool ReadStops(std::shared_ptr<CDSVReader> stopsrc)
    {
        std::vector<std::string> TempRow;

        if (stopsrc->ReadRow(TempRow))
        {
            // finding the columns of the csv file and what their index is 
            size_t StopColumn = -1;
            size_t NodeColumn = -1;
            for (size_t Index = 0; Index < TempRow.size(); Index++)
            {
                if (TempRow[Index] == STOP_ID_HEADER)
                {
                    StopColumn = Index;
                }
                else if (TempRow[Index] == NODE_ID_HEADER)
                {
                    NodeColumn = Index;
                }
            }
            // if either column doesn't exist, return false 
            if (StopColumn == -1 || NodeColumn == -1)
            {
                return false;
            }
            while (stopsrc->ReadRow(TempRow))
            {
                TStopID StopID = std::stoull(TempRow[StopColumn]); // get the values from the csv file, convert from str to int (stoull)
                CStreetMap::TNodeID NodeID = std::stoull(TempRow[NodeColumn]);
                auto NewStop = std::make_shared<SStop>();

                // add the attributes from the row as the sstop class attributes 
                NewStop->DID = StopID;
                NewStop->DNodeID = NodeID;

                // add the Sstop object to the vector 
                DStopsByIndex.push_back(NewStop);
                //map the stop id to the new object in the unordered map 
                DStopsByID[StopID] = NewStop;
            }

            return true;
        }
        return false;
    }




    //const std::string STOP_ID_HEADER    = "stop_id"; //not needed since already declared in the previous block
    const std::string ROUTE_HEADER    = "route";

    std::vector< std::shared_ptr< SRoute > > DRouteByIndex; // routes only, no route object (this is used for the routesize member function)
    std::unordered_map< std::string, std::shared_ptr< SRoute > > DRoutes; //maps the route name to the root object 

    bool ReadRoutes(std::shared_ptr< CDSVReader > routesrc){
        std::vector<std::string> TempRow;

        if(routesrc->ReadRow(TempRow)){

            size_t RouteColumn = -1;
            size_t StopColumn = -1;

            // finding the columns of the csv file and what their index is 
            for(size_t Index = 0; Index < TempRow.size(); Index++){
                if (TempRow[Index] == ROUTE_HEADER){
                    RouteColumn = Index;
                }
                else if (TempRow[Index] == STOP_ID_HEADER){
                    StopColumn = Index;
                }
            }
            // if either column doesn't exist, return false since it's an invalid csv
            if(RouteColumn == -1|| StopColumn == -1){
                return false;
            }

            while(routesrc->ReadRow(TempRow)){

                TStopID StopID = std::stoull(TempRow[StopColumn]); // get the values from the csv file, convert from str to int (stoull)
                std::string RouteName = TempRow[RouteColumn];
                auto NewRoute = std::make_shared< SRoute >();

                // add the attributes from the row as the sroute class attributes 
                NewRoute->route_name = RouteName;
                
                
                if (DRoutes.find(RouteName) != DRoutes.end()) {
                    DRoutes[RouteName]->Stop_list.push_back(StopID); // if the name already exists in the map, add the stop to the routes stoplist 
                }
                else { // this means that the current route is not already in the map, and we must declare a new route 
                    DRoutes[RouteName] = NewRoute; // mapping the routename to the route object 
                    DRouteByIndex.push_back(NewRoute); //pushing the route only
                    NewRoute->Stop_list.push_back(StopID); // add the stop to the stop list, a vector within the sroute class

                }
            }

            return true;
        }
        return false;
    }

    SImplementation(std::shared_ptr< CDSVReader > stopsrc, std::shared_ptr< CDSVReader > routesrc){
        
        ReadStops(stopsrc);
        
        ReadRoutes(routesrc);
    }

    std::size_t StopCount() const noexcept{
        return DStopsByIndex.size();
        
    }

    std::size_t RouteCount() const noexcept{
        return DRouteByIndex.size();
        // returns the amount of routes given a csv file
    }

    std::shared_ptr<SStop> StopByIndex(std::size_t index) const noexcept{
        if (DStopsByIndex.size() <= index) {
            return nullptr;
        }
        return DStopsByIndex[index];
        // returns the stop given some index, only if that index is valid (less than or equal to the number of stops)

    }

    std::shared_ptr<SStop> StopByID(TStopID id) const noexcept{
        if (DStopsByID.find(id) != DStopsByID.end()) { // this means, if the stop id is actually found as a key in the map
            return DStopsByID.find(id)->second;
        }
        else {
            return nullptr;
        }
        // returns the stop object given the stopid, only if the stop id is an actual stop id 
        
    }

    std::shared_ptr<SRoute> RouteByIndex(std::size_t index) const noexcept{
        if (DRouteByIndex.size() <= index) {
            return nullptr;
        }
        return DRouteByIndex[index];
       // returns the route object given some index, only if that index is valid (less than or equal to the number of routes)
    }

    std::shared_ptr<SRoute> RouteByName(const std::string &name) const noexcept{
        if (DRoutes.find(name) != DRoutes.end()) { // this means, if the route is actually found as a key in the map
            return DRoutes.find(name)->second;
        }
        else {
            return nullptr;
        }
        // returns the route object if the name is a key in the map, returns a nullptr if it doesn't exist
    }
};
    








const CBusSystem::TStopID CBusSystem::InvalidStopID;





CCSVBusSystem::CCSVBusSystem(std::shared_ptr< CDSVReader > stopsrc, std::shared_ptr< CDSVReader > routesrc){
    DImplementation = std::make_unique<SImplementation>(stopsrc,routesrc);
}



CCSVBusSystem::~CCSVBusSystem()
{
}

std::size_t CCSVBusSystem::StopCount() const noexcept
{
    return DImplementation->StopCount();
}

std::size_t CCSVBusSystem::RouteCount() const noexcept
{
    return DImplementation->RouteCount();
}

std::shared_ptr<CBusSystem::SStop> CCSVBusSystem::StopByIndex(std::size_t index) const noexcept{
    return DImplementation->StopByIndex(index);

}

std::shared_ptr<CBusSystem::SStop> CCSVBusSystem::StopByID(TStopID id) const noexcept{
    return DImplementation->StopByID(id);

}

std::shared_ptr<CBusSystem::SRoute> CCSVBusSystem::RouteByIndex(std::size_t index) const noexcept{
    return DImplementation->RouteByIndex(index);

}

std::shared_ptr<CBusSystem::SRoute> CCSVBusSystem::RouteByName(const std::string &name) const noexcept{
    return DImplementation->RouteByName(name);

}
