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

    bool ProcessCommands(){

        std::vector <char> temp_vector;

        std::vector <char> help_vector;
      
        
        while (true) {
            std::string word;
            while (true) {
                
                if (Dcmdsrc->Read(temp_vector, 1) == false) {
                    return true;
                }
                if (temp_vector[0] == '\n') {
                    break;
                }
                word = word + temp_vector[0];
            }
            std::string temp_word = "";
            std::vector <std::string> one_command;
            int i = 0;
            for (int i:word) {
                if (word[i] == ' ') {
                    one_command.push_back(temp_word);
                    temp_word = "";
                    break;
                }
                temp_word = temp_word + word[i];
            }


            if (one_command[0] == "help") {
                
            }
        }
        
        

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
