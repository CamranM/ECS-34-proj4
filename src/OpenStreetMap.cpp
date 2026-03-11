#include "OpenStreetMap.h"
#include <unordered_map>

#include <iostream>
#include <string>

struct COpenStreetMap::SImplementation
{
    const std::string DOSMTag = "osm";
    const std::string DNodeTag = "node";
    const std::string DNodeIDAttr = "id";
    const std::string DNodeLatAttr = "lat";
    const std::string DNodeLonAttr = "lon";

    struct SNode : public CStreetMap::SNode
    {
        TNodeID DID;
        CStreetMap::SLocation DLocation;
        std::vector<std::pair<std::string, std::string>> attributes;
        ~SNode() {};
        TNodeID ID() const noexcept override
        {
            return DID;
        }

        CStreetMap::SLocation Location() const noexcept override
        {
            return DLocation;
        }

        std::size_t AttributeCount() const noexcept override
        {
            return attributes.size();
        }

        std::string GetAttributeKey(std::size_t index) const noexcept override
        {
            if (index >= attributes.size())
            {
                return "";
            }
            return attributes[index].first;
        }

        bool HasAttribute(const std::string &key) const noexcept override
        {
            for (size_t i = 0; i < attributes.size(); i++)
            {
                if (attributes[i].first == key)
                    return true;
            }
            return false;
        }

        std::string GetAttribute(const std::string &key) const noexcept override
        {
            if (HasAttribute(key) == false)
                return "";
            for (size_t i = 0; i < attributes.size(); i++)
            {
                if (attributes[i].first == key)
                    return attributes[i].second;
            }
            return "";
        }
    };

    struct SWay : public CStreetMap::SWay
    {
        TWayID SWayID;
        std::vector<TNodeID> SnodeIDs;
        std::vector<std::pair<std::string, std::string>> Sattributes;

        ~SWay() {};
        TWayID ID() const noexcept override
        {
            return SWayID;
        }

        std::size_t NodeCount() const noexcept override
        {
            return SnodeIDs.size();
        }

        TNodeID GetNodeID(std::size_t index) const noexcept override
        {
            if (index >= SnodeIDs.size())
            {
                return -1;
            }
            return SnodeIDs[index];
        }

        std::size_t AttributeCount() const noexcept override
        {
            return Sattributes.size();
        }

        std::string GetAttributeKey(std::size_t index) const noexcept override
        {
            if (index >= Sattributes.size())
            {
                return "";
            }
            return Sattributes[index].first;
        }

        bool HasAttribute(const std::string &key) const noexcept override
        {
            for (size_t i = 0; i < Sattributes.size(); i++)
            {
                if (Sattributes[i].first == key)
                    return true;
            }
            return false;
        }

        std::string GetAttribute(const std::string &key) const noexcept override
        {
            if (HasAttribute(key) == false)
            {
                return "";
            }
            for (size_t i = 0; i < Sattributes.size(); i++)
            {
                if (Sattributes[i].first == key)
                    return Sattributes[i].second;
            }
            return "";
        }
    };
    std::vector<std::shared_ptr<SNode>> DNodesByIndex;
    std::unordered_map<TNodeID, std::shared_ptr<SNode>> DNodesByID;

    std::vector<std::shared_ptr<SWay>> DWayByIndex;
    std::unordered_map<TWayID, std::shared_ptr<SWay>> DWayByID;

    bool ParseNodes(std::shared_ptr<CXMLReader> src, SXMLEntity &nextentity)
    {
        SXMLEntity TempEntity;
        //  loop throuhg nodes
        while (src->ReadEntity(TempEntity, true))
        {
            // if the entitiy is a node
            if (TempEntity.DType == SXMLEntity::EType::StartElement && TempEntity.DNameData == DNodeTag)
            {
                std::string node = TempEntity.AttributeValue(DNodeIDAttr);
                std::string lat = TempEntity.AttributeValue(DNodeLatAttr);
                std::string lon = TempEntity.AttributeValue(DNodeLonAttr);
                if (node.empty() || lat.empty() || lon.empty())
                {
                    return false;
                }
                try // checking if its valid
                {
                    auto NodeID = std::stoull(TempEntity.AttributeValue(DNodeIDAttr));
                    auto NodeLat = std::stod(TempEntity.AttributeValue(DNodeLatAttr));
                    auto NodeLon = std::stod(TempEntity.AttributeValue(DNodeLonAttr));
                }
                catch (const std::exception &e)
                {
                    return false;
                }

                auto NodeID = std::stoull(TempEntity.AttributeValue(DNodeIDAttr));
                auto NodeLat = std::stod(TempEntity.AttributeValue(DNodeLatAttr));
                auto NodeLon = std::stod(TempEntity.AttributeValue(DNodeLonAttr));
                auto NewNode = std::make_shared<SNode>();

                NewNode->DID = NodeID;
                CStreetMap::SLocation Location = CStreetMap::SLocation(NodeLat, NodeLon);
                NewNode->DLocation = Location;
                if (NodeByID(NodeID) != nullptr)
                {
                    return false;
                }

                // looping through the tags
                while (src->ReadEntity(TempEntity, true))
                {
                    // start of new tag, add that to attributes
                    if (TempEntity.DType == SXMLEntity::EType::StartElement && TempEntity.DNameData == "tag")
                    {

                        auto k = std::string(TempEntity.AttributeValue("k"));
                        auto v = std::string(TempEntity.AttributeValue("v"));

                        if (k.empty() || v.empty())
                        {
                            k = std::string(TempEntity.AttributeValue("key"));
                            v = std::string(TempEntity.AttributeValue("value"));
                            if (k.empty() || v.empty())
                            {
                                return false;
                            }
                        }
                        if (NewNode->HasAttribute(k))
                        {
                            return false;
                        }
                        NewNode->attributes.push_back(std::make_pair(k, v));
                        // reads the next element, expect it to be an end tag element
                        if (src->ReadEntity(TempEntity, true))
                        {
                            // return false if not end tag element
                            if (TempEntity.DType != SXMLEntity::EType::EndElement || TempEntity.DNameData != "tag")
                            {
                                return false;
                            }
                        }
                        // if reading the next element fails, return false, should be properly closed
                        else
                        {
                            return false;
                        }
                    }
                    // element is a end node element, that means no more tags
                    else if (TempEntity.DType == SXMLEntity::EType::EndElement && TempEntity.DNameData == DNodeTag)
                    {
                        break;
                    }
                    // anything other than tag or end node element, format is incorrect
                    else
                    {
                        return false;
                    }
                }
                DNodesByIndex.push_back(NewNode);
                DNodesByID[NodeID] = NewNode;
            }
            // if it is not a node element, return the element
            else
            {
                nextentity = TempEntity;
                return true;
            }
        }
        // entity reading fails, check if because EOF
        if (!src->End())
        {
            return false;
        }
        // if because of EOF, just return true
        return true;
    }

    bool ParseWays(std::shared_ptr<CXMLReader> src, SXMLEntity &firstentity)
    {
        SXMLEntity TempEntity;
        bool first = true;
        // loop the ways
        while (true)
        {
            // if first element parsing, get the firstentity and set it to temp
            if (first)
            {
                TempEntity = firstentity;
                first = false;
            }
            // if not the first parse, just proceed normally by reading src
            else
            {
                // if reading fails, break out of loop
                if (!src->ReadEntity(TempEntity, true))
                {
                    break;
                }
            }
            // if the element is a start element way tag
            if (TempEntity.DType == SXMLEntity::EType::StartElement && TempEntity.DNameData == "way")
            {
                std::string check = TempEntity.AttributeValue(DNodeIDAttr);
                if (check.empty())
                {
                    return false;
                }
                try // checking if its valid
                {
                    auto wayID = std::stoull(TempEntity.AttributeValue(DNodeIDAttr));
                }
                catch (const std::exception &e)
                {
                    return false;
                }
                auto wayID = std::stoull(TempEntity.AttributeValue(DNodeIDAttr));
                auto NewWay = std::make_shared<SWay>();
                NewWay->SWayID = wayID;
                if (WayByID(wayID) != nullptr)
                {
                    return false;
                }
                // looping through the inner elements (nodes and tags)
                while (src->ReadEntity(TempEntity, true))
                {
                    // if the element is a nd
                    if (TempEntity.DType == SXMLEntity::EType::StartElement && TempEntity.DNameData == "nd")
                    {
                        std::string check = TempEntity.AttributeValue("ref");
                        if (check.empty())
                        {
                            return false;
                        }
                        try // checking if its valid
                        {
                            auto r = std::stoull(TempEntity.AttributeValue("ref"));
                        }
                        catch (const std::exception &e)
                        {
                            return false;
                        }
                        auto r = std::stoull(TempEntity.AttributeValue("ref"));
                        if (NodeByID(r) == nullptr)
                        {
                            return false;
                        }
                        NewWay->SnodeIDs.push_back(r);
                        // checking if the next element closes the nd
                        if (src->ReadEntity(TempEntity, true))
                        {
                            if (TempEntity.DType != SXMLEntity::EType::EndElement || TempEntity.DNameData != "nd")
                            {

                                return false;
                            }
                        }
                        else
                        {
                            return false;
                        }
                    }
                    // the element is a start tag
                    else if (TempEntity.DType == SXMLEntity::EType::StartElement && TempEntity.DNameData == "tag")
                    {
                        auto k = std::string(TempEntity.AttributeValue("k"));
                        auto v = std::string(TempEntity.AttributeValue("v"));
                        if (k.empty() || v.empty())
                        {
                            k = std::string(TempEntity.AttributeValue("key"));
                            v = std::string(TempEntity.AttributeValue("value"));
                            if (k.empty() || v.empty())
                            {
                                return false;
                            }
                        }

                        if (!(NewWay->HasAttribute(k)))
                        {
                            NewWay->Sattributes.push_back(std::make_pair(k, v));
                        }

                        // check that the next element is a closing tag
                        if (src->ReadEntity(TempEntity, true))
                        {
                            if (TempEntity.DType != SXMLEntity::EType::EndElement || TempEntity.DNameData != "tag")
                            {
                                return false;
                            }
                        }
                        else
                        {
                            return false;
                        }
                    }
                    // if the element is an ending of the way, break out of this loop and continue looping ways
                    else if (TempEntity.DType == SXMLEntity::EType::EndElement && TempEntity.DNameData == "way")
                    {
                        break;
                    }

                    // something other than tag start, nd start, or way end was found, unable to parse
                    else
                    {
                        return false;
                    }
                }

                DWayByIndex.push_back(NewWay);
                DWayByID[wayID] = NewWay;
            }

            // if it is not a way, return true
            else
            {
                firstentity = TempEntity;
                return true;
            }
        }
        // if we broke out of loop but not at end of file, error occurred
        if (!src->End())
        {
            return false;
        }
        return true;
    }

    bool ParseOpenStreetMap(std::shared_ptr<CXMLReader> src)
    {
        SXMLEntity TempEntity;
        bool osmfound = false;

        // loop through to find the first osm
        while (src->ReadEntity(TempEntity, true))
        {
            if (TempEntity.DType == SXMLEntity::EType::StartElement && TempEntity.DNameData == DOSMTag)
            {
                osmfound = true;
                break;
            }
        }

        // if you reach the end of the file without finding the osm, return false
        if (src->End() && osmfound == false)
        {
            return false;
        }
        // parse throuhg nodes
        if (!ParseNodes(src, TempEntity))
        {
            return false;
        }

        if (!ParseWays(src, TempEntity))
        {
            return false;
        }

        if (TempEntity.DType == SXMLEntity::EType::EndElement && TempEntity.DNameData == "osm")
        {
            return true;
        }

        return false;
    }

    SImplementation(std::shared_ptr<CXMLReader> src)
    {
        if (!ParseOpenStreetMap(src))
        {
        }
    }

    std::size_t NodeCount() const noexcept
    {
        return DNodesByIndex.size();
    }

    std::size_t WayCount() const noexcept
    {
        return DWayByIndex.size();
    }

    std::shared_ptr<CStreetMap::SNode> NodeByIndex(std::size_t index) const noexcept
    {
        if (index >= DNodesByIndex.size())
            return nullptr;

        return DNodesByIndex[index];
    }

    std::shared_ptr<CStreetMap::SNode> NodeByID(TNodeID id) const noexcept
    {
        auto it = DNodesByID.find(id);
        if (it != DNodesByID.end())
        {
            return it->second;
        }

        return nullptr;
    }

    std::shared_ptr<CStreetMap::SWay> WayByIndex(std::size_t index) const noexcept
    {
        if (index >= DWayByIndex.size())
            return nullptr;

        return DWayByIndex[index];
    }

    std::shared_ptr<CStreetMap::SWay> WayByID(TWayID id) const noexcept
    {
        auto it = DWayByID.find(id);
        if (it != DWayByID.end())
        {
            return it->second;
        }

        return nullptr;
    }
};

COpenStreetMap::COpenStreetMap(std::shared_ptr<CXMLReader> src)
{
    DImplementation = std::make_unique<SImplementation>(src);
}

COpenStreetMap::~COpenStreetMap()
{
}

std::size_t COpenStreetMap::NodeCount() const noexcept
{
    return DImplementation->NodeCount();
}
std::size_t COpenStreetMap::WayCount() const noexcept
{
    return DImplementation->WayCount();
}

std::shared_ptr<CStreetMap::SNode> COpenStreetMap::NodeByIndex(std::size_t index) const noexcept
{
    return DImplementation->NodeByIndex(index);
}

std::shared_ptr<CStreetMap::SNode> COpenStreetMap::NodeByID(TNodeID id) const noexcept
{
    return DImplementation->NodeByID(id);
}
std::shared_ptr<CStreetMap::SWay> COpenStreetMap::WayByIndex(std::size_t index) const noexcept
{
    return DImplementation->WayByIndex(index);
}

std::shared_ptr<CStreetMap::SWay> COpenStreetMap::WayByID(TWayID id) const noexcept
{
    return DImplementation->WayByID(id);
}
