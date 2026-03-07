#include "DSVReader.h"

#include <iostream>
using std::cout;
using std::endl;

struct CDSVReader::SImplementation{
    std::shared_ptr<CDataSource> DSource;
    char DDelimiter;

    SImplementation(std::shared_ptr< CDataSource > src, char delimiter){
        DSource = src;
        DDelimiter = delimiter;
    }

    bool ParseValue(std::string &val){
        bool InQuotes = false;
        val.clear();
cout<<"IN "<<__FILE__<<" @ "<<__LINE__<<endl;
        while(!DSource->End()){
cout<<"IN "<<__FILE__<<" @ "<<__LINE__<<endl;
            char NextChar;
            DSource->Peek(NextChar);
            if(!InQuotes &&((NextChar == DDelimiter)||(NextChar == '\n'))){
                DSource->Get(NextChar);
cout<<"IN "<<__FILE__<<" @ "<<__LINE__<<endl;
                return true;
            }
            if(NextChar == '\"' && !InQuotes){
                InQuotes = true;
            }
            else{
                val += std::string(1,NextChar);
            }
            DSource->Get(NextChar);
        }
        return false;
    }

    bool End() const{
        return false;
    }

    bool ReadRow(std::vector<std::string> &row){
        cout<<"IN "<<__FILE__<<" @ "<<__LINE__<<endl;
        while(!DSource->End()){
            cout<<"IN "<<__FILE__<<" @ "<<__LINE__<<endl;
            std::string NextValue;
            if(ParseValue(NextValue)){
                row.push_back(NextValue);
                char NextChar;
                DSource->Peek(NextChar);
                if(NextChar == '\n'){
                    // consume
                    return true;
                }
            }
            else{
                break;
            }
        }
        return false;
    }

};
        
CDSVReader::CDSVReader(std::shared_ptr< CDataSource > src, char delimiter){
    DImplementation = std::make_unique<SImplementation>(src,delimiter);
}

CDSVReader::~CDSVReader(){

}

bool CDSVReader::End() const{
    return DImplementation->End();
}

bool CDSVReader::ReadRow(std::vector<std::string> &row){
    return DImplementation->ReadRow(row);
}
