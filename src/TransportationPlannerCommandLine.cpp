#include "TransportationPlannerCommandLine.h"
#include "GeographicUtils.h"

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
    Dcmdsrc = cmdsrc;
    Doutsink = outsink;
    Derrsink = errsink;
    Dresults = results;
    Dplanner = planner;
    }
    ~SImplementation()
    {
    }

    bool ProcessCommands(){

        CTransportationPlanner::TNodeID last_src;
        CTransportationPlanner::TNodeID last_dest;
        std::vector<CTransportationPlanner::TTripStep> last_fastest_path;
        double last_time_hr;

        std::vector <char> temp_vector;

        std::string help = 
                            " ------------------------------------------------------------------------\n"
                            "help     Display this help menu\n"
                            "exit     Exit the program\n"
                            "count    Output the number of nodes in the map\n"
                            "node     Syntax \"node [0, count)\" \n"
                            "         Will output node ID and Lat/Lon for node\n"
                            "fastest  Syntax \"fastest start end\" \n"
                            "         Calculates the time for fastest path from start to end\n"
                            "shortest Syntax \"shortest start end\" \n"
                            "         Calculates the distance for the shortest path from start to end\n"
                            "save     Saves the last calculated path to file\n"
                            "print    Prints the steps for the last calculated path\n"
                            ;
        std::vector <char> help_vector(help.begin(), help.end());

        std::string node_error = "Invalid node command, see help.\n";
        std::vector <char> node_error_vector(node_error.begin(), node_error.end());

        std::string node_param_error = "Invalid node parameter, see help.\n";
        std::vector <char> node_param_error_vector(node_param_error.begin(), node_param_error.end());

        std::string fastest_error = "Invalid fastest command, see help.\n";
        std::vector <char> fastest_error_vector(fastest_error.begin(), fastest_error.end());

        std::string fastest_param_error = "Invalid fastest parameter, see help.\n";
        std::vector <char> fastest_param_error_vector(fastest_param_error.begin(), fastest_param_error.end());

        std::string shortest_error = "Invalid shortest command, see help.\n";
        std::vector <char> shortest_error_vector(shortest_error.begin(), shortest_error.end());

        std::string shortest_param_error = "Invalid shortest parameter, see help.\n";
        std::vector <char> shortest_param_error_vector(shortest_param_error.begin(), shortest_param_error.end());
        

        std::string print_error = "No valid path to print, see help.\n";
        std::vector <char> print_error_vector(print_error.begin(), print_error.end());

        
        std::string save_error = "No valid path to save, see help.\n";
        std::vector <char> save_error_vector(save_error.begin(), save_error.end());
        
        std::string unknown_error = "Unknown command \"foo\" type help for help.\n";
        std::vector <char> unknown_error_vector(unknown_error.begin(), unknown_error.end());


        while (true) {
            std::string word;
            Doutsink->Put('>');
            Doutsink->Put(' ');
            while (true) {
                
                if (Dcmdsrc->Read(temp_vector, 1) == false) {
                    return true; //everything read 
                }
                if (temp_vector[0] == '\n') {
                    break;
                }
                word = word + temp_vector[0];
            }
            std::string temp_word = "";
            std::vector <std::string> one_command;
            //int i = 0;
            for (char c : word) {
                    if (c == ' ') {
                        if (!temp_word.empty()) {
                            one_command.push_back(temp_word);
                            temp_word.clear();
                        }
                    }
                    else {
                        temp_word += c;
                    }
                }

                if (!temp_word.empty()) {
                    one_command.push_back(temp_word);
                    }



            if (one_command.empty()) {
                continue;
            }
            else if (one_command[0] == "help") {
                Doutsink->Write(help_vector);
            }

            else if (one_command[0] == "exit") {
                break;
            }

            else if (one_command[0] == "count") {
                std::string node_count_str = std::to_string(Dplanner->NodeCount()); // need to store in vector and write 
                std::vector <char> node_count_vector(node_count_str.begin(), node_count_str.end());
                std::string count = " nodes";
                std::vector <char> count_vec(count.begin(), count.end());
                Doutsink->Write(node_count_vector);
                Doutsink->Write(count_vec);
                Doutsink->Put('\n');
            }

            else if (one_command[0] == "node") {
                if (one_command.size() != 2)  {
                    Derrsink->Write(node_error_vector);
                }
                else if (one_command.size() == 2) {
                    int i;
                    try {
                        
                        i = std::stoi(one_command[1]);
                    }
                    catch (...) {
                        Derrsink->Write(node_param_error_vector);
                        continue;
                    }
                    if (i >= Dplanner->NodeCount()|| i < 0) {
                        Derrsink->Write(node_param_error_vector);
                    }
                    else {
                        std::shared_ptr<CStreetMap::SNode> node = Dplanner->SortedNodeByIndex(i);
                        int node_id = node->ID();
                        std::string node_id_str = std::to_string(node_id);
                        CStreetMap::SLocation loc = node->Location();
                        std::string loc_string = SGeographicUtils::ConvertLLToDMS(loc);

                        std::string first_part = "Node " + one_command[1] + ": id = ";
                        std::vector <char> first_part_vector(first_part.begin(), first_part.end());
                        std::vector <char> isat_vector = {' ', 'i','s',' ','a','t', ' '};
                        std::vector <char> node_id_vector(node_id_str.begin(), node_id_str.end());
                        std::vector <char> loc_vector(loc_string.begin(), loc_string.end());
                        Doutsink->Write(first_part_vector);
                        Doutsink->Write(node_id_vector);
                        Doutsink->Write(isat_vector);
                        Doutsink->Write(loc_vector);
                        Doutsink->Put('\n');
                    }
                }
                // should be done 
                
            }

            else if (one_command[0] == "fastest") {
                if (one_command.size() != 3)  {
                    Derrsink->Write(fastest_error_vector);
                }
                else if (one_command.size() == 3) {
                    int k;
                    int i;
                    try {
                        i = std::stoi(one_command[1]);
                        k = std::stoi(one_command[2]);
                    }
                    catch (...) {
                        Derrsink->Write(fastest_param_error_vector);
                        continue; 
                    }
                    std::vector<CTransportationPlanner::TTripStep> path;
                    double fastest_path = Dplanner->FindFastestPath(i, k, path);
                    if (fastest_path == CPathRouter::NoPathExists) {
                        Derrsink->Write(fastest_param_error_vector);
                    }
                    else {
                        int total_seconds = fastest_path * 3600;
                        int hours = total_seconds / 3600;
                        int minutes = (total_seconds % 3600) / 60;
                        int seconds = total_seconds % 60;
                        std::string first_part = "Fastest path takes ";


                        last_src = i;
                        last_dest = k;
                        last_fastest_path = path;
                        last_time_hr = fastest_path;

                        
                        if (hours > 0) {
                            if (minutes != 0 && seconds !=0) {
                                std::string fastest_path_str = std::to_string(hours) + " hr " + std::to_string(minutes) + " min " + std::to_string(seconds) + " sec.";
                                std::vector <char> first_part_vector(first_part.begin(), first_part.end());
                            
                                std::vector <char> fastest_path_vector(fastest_path_str.begin(), fastest_path_str.end());
                                Doutsink->Write(first_part_vector);
                                Doutsink->Write(fastest_path_vector);
                                
                                Doutsink->Put('\n');
                            
                            }
                            else if (minutes == 0 && seconds !=0) {
                                std::string fastest_path_str = std::to_string(hours) + " hr " + std::to_string(minutes) + " min "+ std::to_string(seconds) + " sec.";
                                std::vector <char> first_part_vector(first_part.begin(), first_part.end());
                            
                                std::vector <char> fastest_path_vector(fastest_path_str.begin(), fastest_path_str.end());
                                Doutsink->Write(first_part_vector);
                                Doutsink->Write(fastest_path_vector);
                                
                                Doutsink->Put('\n');
                            }
                            else if (minutes !=0 && seconds == 0) {
                                std::string fastest_path_str = std::to_string(hours) + " hr " + std::to_string(minutes) + " min.";
                                std::vector <char> first_part_vector(first_part.begin(), first_part.end());
                            
                                std::vector <char> fastest_path_vector(fastest_path_str.begin(), fastest_path_str.end());
                                Doutsink->Write(first_part_vector);
                                Doutsink->Write(fastest_path_vector);
                                
                                Doutsink->Put('\n');
                            }
                        }

                        else if (hours == 0 && minutes != 0) {
                            if (seconds != 0) {
                                std::string fastest_path_str = std::to_string(minutes) + " min " + std::to_string(seconds) + " sec.";
                                std::vector <char> first_part_vector(first_part.begin(), first_part.end());
                                std::vector <char> fastest_path_vector(fastest_path_str.begin(), fastest_path_str.end());
                                Doutsink->Write(first_part_vector);
                                Doutsink->Write(fastest_path_vector);
                                Doutsink->Put('\n');
                            }
                            else {
                                std::string fastest_path_str = std::to_string(minutes) + " min.";
                                std::vector <char> first_part_vector(first_part.begin(), first_part.end());
                                std::vector <char> fastest_path_vector(fastest_path_str.begin(), fastest_path_str.end());
                                Doutsink->Write(first_part_vector);
                                Doutsink->Write(fastest_path_vector);
                                Doutsink->Put('\n');
                            }

                            
                        }
                        else {
                            std::string fastest_path_str = std::to_string(minutes) + " min " + std::to_string(seconds) + " sec.";
                            std::vector <char> first_part_vector(first_part.begin(), first_part.end());
                        
                            std::vector <char> fastest_path_vector(fastest_path_str.begin(), fastest_path_str.end());
                            Doutsink->Write(first_part_vector);
                            Doutsink->Write(fastest_path_vector);
                            
                            Doutsink->Put('\n');
                        }
                        
                        
                    }

                }

            }


            else if (one_command[0] == "shortest") {
                if (one_command.size() != 3)  {
                    Derrsink->Write(shortest_error_vector);
                }
                else if (one_command.size() == 3) {
                    int k;
                    int i;
                    try {
                        i = std::stoi(one_command[1]);
                        k = std::stoi(one_command[2]);
                    }
                    catch (...) {
                        Derrsink->Write(shortest_param_error_vector);
                        continue; 
                    }
                    std::vector<CTransportationPlanner::TNodeID> path;
                    double shortest_path = Dplanner->FindShortestPath(i, k, path);
                    if (shortest_path == CPathRouter::NoPathExists) {
                        Derrsink->Write(shortest_param_error_vector);
                    }
                    else {   
                        std::ostringstream trailing_zeroes;
                        trailing_zeroes << shortest_path;
                        std::string shortest_path_str = trailing_zeroes.str();


                        std::string first_part = "Shortest path is ";
                        std::string last_part = " mi.";
                        std::vector <char> first_part_vector(first_part.begin(), first_part.end());
                        std::vector <char> shortest_path_str_vector(shortest_path_str.begin(), shortest_path_str.end());
                        std::vector <char> last_part_vector(last_part.begin(), last_part.end());
                        Doutsink->Write(first_part_vector);
                        Doutsink->Write(shortest_path_str_vector);
                        Doutsink->Write(last_part_vector);

                        Doutsink->Put('\n');
                    }

                }

            }

            else if (one_command[0] == "print") {
                if (last_fastest_path.empty()) {
                    Derrsink->Write(print_error_vector);
                }
                else {
                    std::vector <std::string> path_descrption;
                    Dplanner->GetPathDescription(last_fastest_path, path_descrption);

                    for (auto& element : path_descrption) {
                        std::vector<char> one_line(element.begin(), element.end());
                        Doutsink->Write(one_line);
                        Doutsink->Put('\n');
                    }
                }
            }

            else if (one_command[0] == "save"){
                if (last_fastest_path.empty()) {
                    Derrsink->Write(save_error_vector);
                }
                else {
                    std::string filename = std::to_string(last_src) + "_" + std::to_string(last_dest) + "_" + std::to_string(last_time_hr) + "hr.csv";
                    auto Sink = Dresults->CreateSink(filename);
                    std::string first_line = "mode,node_id\n";
                    std::vector<char> first_line_vector(first_line.begin(),first_line.end());
                    Sink->Write(first_line_vector);

                    for (auto line : last_fastest_path) {
                        std::string trans_mode;
                        if (line.first == CTransportationPlanner::ETransportationMode::Bike) {
                            trans_mode = "Bike";
                        }
                        else if (line.first == CTransportationPlanner::ETransportationMode::Walk) {
                            trans_mode = "Walk";
                        }
                        else if (line.first == CTransportationPlanner::ETransportationMode::Bus) {
                            trans_mode = "Bus";
                        }

                        std::string one_line = trans_mode + "," + std::to_string(line.second) + "\n";
                        std::vector<char> one_line_vector(one_line.begin(),one_line.end());


                        Sink->Write(one_line_vector);
                    }
                    std::string conf =  "Path saved to <results>/" + filename + "\n";
                    std::vector<char> conf_vector(conf.begin(),conf.end());
                    Doutsink->Write(conf_vector);


                }
            }
            else {
                std::string error_message = "Unknown command \"" + one_command[0] + "\" type help for help.\n";
                std::vector<char> error_message_vector(error_message.begin(),error_message.end());
                Derrsink->Write(error_message_vector);
            }


        }
        
        
        return true;
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
