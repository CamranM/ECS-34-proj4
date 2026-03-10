#include "DSVWriter.h"
#include <expat.h>

struct CDSVWriter::SImplementation
{
    std::shared_ptr<CDataSink> DSink;
    char DDelimiter;
    bool DQuoteAll;
    bool firstLine;

    SImplementation(std::shared_ptr<CDataSink> sink, char delimiter, bool quoteall)
    {
        DSink = sink;
        DDelimiter = delimiter;
        DQuoteAll = quoteall;
        firstLine = true;
    }

    ~SImplementation()
    {
    }

    bool WriteRow(const std::vector<std::string> &row)
    {
        // have to make it work with delimiters inside the columns
        // quoteall just adds quotes around the columns done
        //
        bool First = true;
        char delim = DDelimiter;
        if (!firstLine)
        {
            DSink->Put('\n');
        }
        if (DDelimiter == '"')
        {
            delim = ',';
        }
        for (auto &Column : row)
        {
            // type of return from find()
            bool double_quote = (Column.find('"') != std::string::npos);
            bool quotes = ((Column.find(delim) != std::string::npos) || double_quote || (Column.find('\n') != std::string::npos));
            // std::vector<char> Buffer(Column.begin(), Column.end());
            if (!First)
            {
                DSink->Put(delim);
            }
            First = false;
            // puts quotes in front when quoteAll = true
            if (DQuoteAll || quotes)
            {
                DSink->Put('"');
            }
            std::vector<char> Buffer;
            for (char c : Column)
            {
                if (c == '"')
                {
                    Buffer.push_back('"');
                }
                Buffer.push_back(c);
            }
            DSink->Write(Buffer);
            // puts quotes at end when quoteAll = true

            if (DQuoteAll || quotes)
            {
                DSink->Put('"');
            }
        }
        firstLine = false;

        return true;
    }
};

CDSVWriter::CDSVWriter(std::shared_ptr<CDataSink> sink, char delimiter, bool quoteall)
{
    DImplementation = std::make_unique<SImplementation>(sink, delimiter, quoteall);
}

CDSVWriter::~CDSVWriter()
{
}

bool CDSVWriter::WriteRow(const std::vector<std::string> &row)
{
    return DImplementation->WriteRow(row);
}
