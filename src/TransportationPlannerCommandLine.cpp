#include "TransportationPlannerCommandLine.h"

struct CTransportationPlannerCommandLine::SImplementation
{
    std::shared_ptr<CDataSource> Dcmdsrc;
    std::shared_ptr<CDataSink> Doutsink;
    std::shared_ptr<CDataSink> Derrsink;
    std::shared_ptr<CDataFactory> Dresults;
    std::shared_ptr<CTransportationPlanner> Dplanner;
    SImplementation(std::shared_ptr<CDataSource> cmdsrc,
                    std::shared_ptr<CDataSink> outsink,
                    std::shared_ptr<CDataSink> errsink,
                    std::shared_ptr<CDataFactory> results,
                    std::shared_ptr<CTransportationPlanner> planner)
    {
    }
    ~SImplementation()
    {
    }

    bool ProcessCommands()
    {
    }
};

CTransportationPlannerCommandLine::CTransportationPlannerCommandLine(std::shared_ptr<CDataSource> cmdsrc,
                                                                     std::shared_ptr<CDataSink> outsink,
                                                                     std::shared_ptr<CDataSink> errsink,
                                                                     std::shared_ptr<CDataFactory> results,
                                                                     std::shared_ptr<CTransportationPlanner> planner)
{
    DImplementation = std::make_unique<SImplementation>(cmdsrc, outsink, errsink, results, planner);
}

CTransportationPlannerCommandLine::~CTransportationPlannerCommandLine()
{
}

bool CTransportationPlannerCommandLine::ProcessCommands()
{
    return DImplementation->ProcessCommands();
}
