Overview:
The DijkstraTransportationPlanner class finds the fastest routes and the shortest routes between a source and destination node. For the fastest time, it considers two types of modes of transportation: walking and busing, or biking only (bikes are not allowed on buses).

Constructor:
CDijkstraTransportationPlanner(std::shared_ptr<SConfiguration> config);

Takes in a SConfiguration that contains the StreetMap, Bus System, default speed limits, speeds of different modes of transportation, bus wait time, and other necessary configuration information. 
These objects are used to construct the internal graph that will be used to find the shortest and fastest paths. 


Destructor: 
~CDijkstraTransportationPlanner()

Destroys the DijkstraTransportationPlanner object. Internal resources destroyed and freed by destructor ~SImplementation().


Static Functions:
static bool compareID(const std::shared_ptr<CStreetMap::SNode> &a, const std::shared_ptr<CStreetMap::SNode> &b)

Used within the class to make SNodes comparable for sorting. Compares the SNodes by their ID.


General Functions:
std::size_t NodeCount() const noexcept override;

Returns the number of nodes in the graph.

std::shared_ptr<CStreetMap::SNode> SortedNodeByIndex(std::size_t index) const noexcept override;

Returns the node at location index sorted in ascending order by NodeID.

double FindShortestPath(TNodeID src, TNodeID dest, std::vector< TNodeID > &path) override;

Takes in a source NodeID, destination NodeID, and a vector of NodeIDs. Returns the shortest distance path between source and destination. The NodeIds traversed are stored in the path vector in order of traversal. 

double FindFastestPath(TNodeID src, TNodeID dest, std::vector< TTripStep > &path) override;

Takes in a source NodeID, destination NodeID, and a vector of TTripSteps. Returns the shortest time path between source and destination. The list of TTripSteps traversed are stored in the path vector in order of traversal. TTripSteps is a pair consisting of mode of transportation and the corresponding NodeID. See TransportationPlanner for more details.

bool GetPathDescription(const std::vector< TTripStep > &path, std::vector< std::string > &desc) const override;

Takes in a vector of TTripSteps and returns a human-readable list of instructions to follow the path. 
