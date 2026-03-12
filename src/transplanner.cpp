#include "TransportationPlannerCommandLine.h"
#include "StandardDataSink.h"
#include "StandardDataSource.h"
#include "StandardErrorDataSink.h"
#include "FileDataFactory.h"
#include "DSVReader.h"
#include "XMLReader.h"
#include "CSVBusSystem.h"
#include "OpenStreetMap.h"
#include "TransportationPlannerConfig.h"
#include "DijkstraTransportationPlanner.h"

#include <string>
#include <iostream>
#include <vector>
#include <memory>

using std::cerr;
using std::cout;
using std::endl;

int ParseArgs(const std::vector<std::string> &args, std::string &datapath, std::string &resultspath);

int main(int argc, char *argv[])
{
    std::string DataPath = "./data";
    std::string ResultsPath = "./results";
    std::vector<std::string> Arguments;

    for (int i = 1; i < argc; i++)
    {
        Arguments.push_back(argv[i]);
    }

    if (ParseArgs(Arguments, DataPath, ResultsPath) != 0)
    {
        cerr << "Syntax Error: transplanner [--data=path | --results=path]" << endl;
        return 1;
    }

    auto StdIn = std::make_shared<CStandardDataSource>();
    auto StdOut = std::make_shared<CStandardDataSink>();
    auto StdError = std::make_shared<CStandardErrorDataSink>();
    auto Results = std::make_shared<CFileDataFactory>(ResultsPath);
    auto Data = std::make_shared<CFileDataFactory>(DataPath);

    auto StreetMapSource = Data->CreateSource("city.osm");
    auto StopsSource = Data->CreateSource("stops.csv");
    auto RoutesSource = Data->CreateSource("routes.csv");

    auto StreetMapXMLReader = std::make_shared<CXMLReader>(StreetMapSource);
    auto StopsDSVReader = std::make_shared<CDSVReader>(StopsSource, ',');
    auto RoutesDSVReader = std::make_shared<CDSVReader>(RoutesSource, ',');

    auto StreetMap = std::make_shared<COpenStreetMap>(StreetMapXMLReader);
    auto BusSystem = std::make_shared<CCSVBusSystem>(StopsDSVReader, RoutesDSVReader);
    auto Config = std::make_shared<STransportationPlannerConfig>(StreetMap, BusSystem);
    auto TransportationPlanner = std::make_shared<CDijkstraTransportationPlanner>(Config);

    CTransportationPlannerCommandLine TPCommandLine(StdIn, StdOut, StdError, Results, TransportationPlanner);
    
    TPCommandLine.ProcessCommands();

    return 0;
}

int ParseArgs(const std::vector<std::string> &args, std::string &datapath, std::string &resultspath)
{
    for (const auto &Argument : args)
    {
        if (Argument.find("--data=") == 0)
        {
            datapath = Argument.substr(7);
        }
        else if (Argument.find("--results=") == 0)
        {
            resultspath = Argument.substr(10);
        }
        else
        {
            return 1;
        }
    }
    return 0;
}