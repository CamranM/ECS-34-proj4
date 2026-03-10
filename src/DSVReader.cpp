#include "DSVReader.h"

struct CDSVReader::SImplementation
{
    std::shared_ptr<CDataSource> Dsrc;
    char DDelimiter;
    char ch;
    bool first;

    SImplementation(std::shared_ptr<CDataSource> src, char delimiter)
    {
        DDelimiter = delimiter;
        Dsrc = src;
        ch = 0;
        first = true;
    }

    ~SImplementation()
    {
    }

    bool ReadRow(std::vector<std::string> &row)
    {
        // read, peek, get(?), end
        // src

        // hello,how,are,\"you\n\"\n\ngood,thank,\"you,\"
        // hello""\n
        row.clear();
        std::string Temp = "";
        if (Dsrc->End())
        {
            if (ch == '\n' || first)
            {
                ch = 0;
                row.push_back(Temp);
                first = false;
                return true;
            }
            else
            {
                return false;
            }
        }
        first = false;
        char delim = DDelimiter;
        if (DDelimiter == '"')
        {
            delim = ',';
        }

        while (!Dsrc->End())
        {
            Dsrc->Peek(ch);
            bool quoted = (ch == '"');
            Dsrc->Get(ch);

            if (quoted)
            {
                // Dsrc->Get(ch);       // get past first surrounding "
                while (!Dsrc->End()) // looks ahead of ch
                {
                    Dsrc->Get(ch);
                    if (ch == '"')
                    { // if current char is "
                        if (Dsrc->End())
                        {
                            row.push_back(Temp);
                            Temp = "";
                            return true;
                        }
                        Dsrc->Get(ch);
                        // Dsrc->Peek(ch);
                        if (ch == '"') // ch == '"'
                        {              // if current char is " and next char is ", push only 1 " //Dsrc->Peek(dQuote)
                            Temp.push_back(ch);
                            // Dsrc->Get(ch);
                        }
                        else if (ch == '\n') // ch == '\n'
                        {                    // if current char is " and next char is \n, thats the end of the row //Dsrc->Peek(newLine)
                            row.push_back(Temp);
                            // Dsrc->Get(ch);
                            Temp = "";
                            return true;
                        }
                        else
                        { // if current char " and next char delim, thats end of string //Dsrc->Peek(delim) //if (ch == delim)
                            // Dsrc->Get(ch);
                            row.push_back(Temp);
                            Temp = "";
                            break;
                        }
                    }
                    else
                    {
                        Temp.push_back(ch);
                    }
                }
            }
            else
            {
                while (ch != delim && ch != '\n' && !Dsrc->End())
                {
                    Temp.push_back(ch);
                    Dsrc->Get(ch);
                }

                if (ch == '\n' || Dsrc->End())
                {
                    if (ch == '\n')
                    {
                        row.push_back(Temp);
                        Temp = "";
                    }
                    else
                    {
                        Temp.push_back(ch);
                        row.push_back(Temp);
                        Temp = "";
                    }

                    return true;
                }
                row.push_back(Temp);
                Temp = "";
            }
        }
        row.push_back(Temp);
        Temp = "";
        return true;
    }

    bool End() const
    {
        return Dsrc->End();
    }
};

CDSVReader::CDSVReader(std::shared_ptr<CDataSource> src, char delimiter)
{
    DImplementation = std::make_unique<SImplementation>(src, delimiter);
}

CDSVReader::~CDSVReader()
{
}

bool CDSVReader::ReadRow(std::vector<std::string> &row)
{
    return DImplementation->ReadRow(row);
}

bool CDSVReader::End() const
{
    return DImplementation->End();
}