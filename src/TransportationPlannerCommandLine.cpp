#include "TransportationPlannerCommandLine.h"
#include "StringUtils.h"
#include <vector>

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

    void OutputString(const std::string &str)
    {
    }

    bool InputLine(std::string &line)
    {
        return false;
    }

    void OutputError(const std::string &str)
    {
    }

    bool ProcessCommands()
    {
        std::string CurrentLine;
        OutputString("> ");
        while (!Dcmdsrc->End() && InputLine(CurrentLine))
        {
            auto CommandArgs = StringUtils::Split(CurrentLine);
            if (CommandArgs[0] == "exit")
            {
            }
            else if (CommandArgs[0] == "help")
            {
            }
            else if (CommandArgs[0] == "count")
            {
            }
        }
        OutputString("> ");
        return false;
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
