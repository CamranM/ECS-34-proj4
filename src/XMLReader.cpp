#include "XMLReader.h"
#include <expat.h>
#include <queue>

// <tag attr="val">CharData</tag>

struct CXMLReader::SImplementation
{
    std::shared_ptr<CDataSource> DSource;
    XML_Parser DParser;
    std::queue<SXMLEntity> DQueue;

    static void StartElementHandler(void *userData, const XML_Char *name, const XML_Char **atts)
    {
        SImplementation *This = (SImplementation *)userData;
        SXMLEntity NewEntity;

        NewEntity.DType = SXMLEntity::EType::StartElement;
        NewEntity.DNameData = name;
        for (int i = 0; atts[i]; i += 2)
        {
            NewEntity.DAttributes.push_back({atts[i], atts[i + 1]});
        }
        This->DQueue.push(NewEntity);
    }

    static void EndElementHandler(void *userData, const XML_Char *name)
    {
        SImplementation *This = (SImplementation *)userData;
        SXMLEntity NewEntity;

        NewEntity.DType = SXMLEntity::EType::EndElement;
        NewEntity.DNameData = name;
        This->DQueue.push(NewEntity);
    }

    static void CharacterDataHandler(void *userData, const XML_Char *s, int len)
    {
        std::vector<std::pair<char, std::string>> escape_char = {{'<', "&lt"}, {'>', "&gt"}, {'&', "&amp"}, {'\"', "&quot"}, {'\'', "&apos"}};
        int i;
        int j;
        int k;
        std::string substr;
        SImplementation *This = (SImplementation *)userData;
        SXMLEntity NewEntity;
        std::string new_s;

        NewEntity.DType = SXMLEntity::EType::CharData;

        // checks characters to see if any of them are special/escape chars that need to be converted due to XML syntax through looping thruogh the entire char s. If a & is detected, if finds the
        // entire special character, goes to the espape_char vector to find the corresponding special character, and converts it.
        for (i = 0; i < len; ++i)
        {
            j = i + 1;
            if (s[i] == '&')
            {
                for (j; j < len; ++j)
                {
                    if (s[j] == ';')
                    {
                        break;
                    }
                }
                substr = std::string(s + i, j - i);
                for (k = 0; k < 5; ++k)
                {
                    if (escape_char[k].second == substr)
                    {
                        new_s = new_s + escape_char[k].first;
                    }
                    else if (substr == "&")
                    {
                        new_s = new_s + '&';
                        break;
                    }
                }
                i = j;
            }
            else
            {
                new_s = new_s + s[i];
            }
        }
        NewEntity.DNameData = new_s;
        This->DQueue.push(NewEntity);
    }

    SImplementation(std::shared_ptr<CDataSource> src)
    {
        DSource = src;
        DParser = XML_ParserCreate(nullptr);
        XML_SetUserData(DParser, (void *)this);
        XML_SetStartElementHandler(DParser, StartElementHandler);
        XML_SetEndElementHandler(DParser, EndElementHandler);
        XML_SetCharacterDataHandler(DParser, CharacterDataHandler);
    }

    bool End() const
    {
        if (DQueue.empty() && DSource->End())
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    bool ReadEntity(SXMLEntity &entity, bool skipcdata)
    {

        while (true)
        {
            while (DQueue.empty() && !DSource->End())
            {
                std::vector<char> Buffer(512);
                DSource->Read(Buffer, Buffer.size());
                // size_t bytes = DSource->Read(Buffer, Buffer.size());
                XML_Parse(DParser, Buffer.data(), Buffer.size(), DSource->End());
            }

            if (!DQueue.empty())
            {
                // addresses the skipcdata parameter. Uses a while loop to pop all char data, since char data is not read when skipcdata is true. Then, entity becomes the first non char (if the queue is not empty at that point)
                if (skipcdata == true)
                {
                    while (!DQueue.empty() && DQueue.front().DType == SXMLEntity::EType::CharData)
                    {
                        DQueue.pop();
                    }
                    // if the queue is still filled and there is no more chars, have entity equal the front of queue
                    if (!DQueue.empty())
                    {
                        entity = DQueue.front();
                        DQueue.pop();
                        return true;
                    }
                    // continue;
                }

                else
                {
                    entity = DQueue.front();
                    DQueue.pop();
                    // since expat reads chars that are special characters in chunks, add all of the characters in a row and return the entity instead of individually
                    if (entity.DType == SXMLEntity::EType::CharData)
                    {
                        while (!DQueue.empty() && DQueue.front().DType == SXMLEntity::EType::CharData)
                        {
                            entity.DNameData += DQueue.front().DNameData;
                            DQueue.pop();
                        }
                        return true;
                    }

                    return true;
                }
            }
            if (DQueue.empty() && DSource->End())
            {
                return false;
            }
        }
    }
};

// public interface, since simplementation is not visible to the public
CXMLReader::CXMLReader(std::shared_ptr<CDataSource> src)
{
    DImplementation = std::make_unique<SImplementation>(src);
}

CXMLReader::~CXMLReader()
{
}

bool CXMLReader::End() const
{
    return DImplementation->End();
}

bool CXMLReader::ReadEntity(SXMLEntity &entity, bool skipcdata)
{
    return DImplementation->ReadEntity(entity, skipcdata);
}
