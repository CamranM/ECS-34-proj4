#include <gtest/gtest.h>
#include "OpenStreetMap.h"
#include "StringDataSource.h"

TEST(OSMTest, SimpleFiles)
{
    auto OSMDataSource = std::make_shared<CStringDataSource>("<?xml version='1.0' encoding='UTF-8'?>\n"
                                                             "<osm version=\"0.6\" generator=\"osmconvert 0.8.5\">\n"
                                                             "  <node id=\"1\" lat=\"38.5\" lon=\"-121.7\"/>\n"
                                                             "  <node id=\"2\" lat=\"38.5\" lon=\"-121.8\"/>\n"
                                                             "  <way id=\"100\">\n"
                                                             "    <nd ref=\"1\"/>\n"
                                                             "    <nd ref=\"2\"/>\n"
                                                             "  </way>\n"
                                                             "</osm>");
    auto OSMReader = std::make_shared<CXMLReader>(OSMDataSource);
    COpenStreetMap OpenStreetMap(OSMReader);

    EXPECT_EQ(OpenStreetMap.NodeCount(), 2);
    EXPECT_EQ(OpenStreetMap.WayCount(), 1);
    EXPECT_NE(OpenStreetMap.NodeByIndex(0), nullptr);
    EXPECT_NE(OpenStreetMap.NodeByIndex(1), nullptr);
    EXPECT_EQ(OpenStreetMap.NodeByIndex(2), nullptr);
    EXPECT_NE(OpenStreetMap.WayByIndex(0), nullptr);
    EXPECT_EQ(OpenStreetMap.WayByIndex(1), nullptr);
    auto TempNode = OpenStreetMap.NodeByIndex(0);
    ASSERT_NE(TempNode, nullptr);
    EXPECT_EQ(TempNode, OpenStreetMap.NodeByID(TempNode->ID()));
    EXPECT_EQ(TempNode->ID(), 1);
    EXPECT_EQ(TempNode->Location(), std::make_pair(38.5, -121.7));
}

// testing formatting
TEST(OSMTest, EmptyFile)
{
    auto OSMDataSource = std::make_shared<CStringDataSource>("");
    auto OSMReader = std::make_shared<CXMLReader>(OSMDataSource);
    COpenStreetMap OpenStreetMap(OSMReader);

    EXPECT_EQ(OpenStreetMap.NodeCount(), 0);
    EXPECT_EQ(OpenStreetMap.WayCount(), 0);
    EXPECT_EQ(OpenStreetMap.NodeByIndex(0), nullptr);
    EXPECT_EQ(OpenStreetMap.NodeByIndex(1), nullptr);
    EXPECT_EQ(OpenStreetMap.NodeByIndex(2), nullptr);
    EXPECT_EQ(OpenStreetMap.WayByIndex(0), nullptr);
    EXPECT_EQ(OpenStreetMap.WayByIndex(1), nullptr);
    EXPECT_EQ(OpenStreetMap.WayByIndex(2), nullptr);
    // auto TempNode = OpenStreetMap.NodeByIndex(0);
    // ASSERT_NE(TempNode,nullptr);
    // EXPECT_EQ(TempNode, OpenStreetMap.NodeByID(TempNode->ID()));
    // EXPECT_EQ(TempNode->ID(),1);
    // EXPECT_EQ(TempNode->Location(),std::make_pair(38.5,-121.7));
}

TEST(OSMTest, OnlyOSM)
{
    auto OSMDataSource = std::make_shared<CStringDataSource>("<osm version=\"0.6\" generator=\"osmconvert 0.8.5\">\n"
                                                             "</osm>");
    auto OSMReader = std::make_shared<CXMLReader>(OSMDataSource);
    COpenStreetMap OpenStreetMap(OSMReader);

    EXPECT_EQ(OpenStreetMap.NodeCount(), 0);
    EXPECT_EQ(OpenStreetMap.WayCount(), 0);
    EXPECT_EQ(OpenStreetMap.NodeByIndex(0), nullptr);
    EXPECT_EQ(OpenStreetMap.NodeByIndex(1), nullptr);
    EXPECT_EQ(OpenStreetMap.NodeByIndex(2), nullptr);
    EXPECT_EQ(OpenStreetMap.WayByIndex(0), nullptr);
    EXPECT_EQ(OpenStreetMap.WayByIndex(1), nullptr);
    EXPECT_EQ(OpenStreetMap.WayByIndex(2), nullptr);
}

TEST(OSMTest, NoOsm)
{
    auto OSMDataSource = std::make_shared<CStringDataSource>("<node id=\"1\" lat=\"38.5\" lon=\"-121.7\"/>\n");
    auto OSMReader = std::make_shared<CXMLReader>(OSMDataSource);
    COpenStreetMap OpenStreetMap(OSMReader);

    EXPECT_EQ(OpenStreetMap.NodeCount(), 0);
    EXPECT_EQ(OpenStreetMap.WayCount(), 0);
    EXPECT_EQ(OpenStreetMap.NodeByIndex(0), nullptr);
    EXPECT_EQ(OpenStreetMap.NodeByIndex(1), nullptr);
    EXPECT_EQ(OpenStreetMap.NodeByIndex(2), nullptr);
    EXPECT_EQ(OpenStreetMap.WayByIndex(0), nullptr);
    EXPECT_EQ(OpenStreetMap.WayByIndex(1), nullptr);
    EXPECT_EQ(OpenStreetMap.WayByIndex(2), nullptr);
}

TEST(OSMTest, MissingOneOsm)
{
    auto OSMDataSource = std::make_shared<CStringDataSource>("<osm version=\"0.6\" generator=\"osmconvert 0.8.5\">\n"
                                                             "  <node id=\"1\" lat=\"38.5\" lon=\"-121.7\"/>\n"
                                                             "  <node id=\"2\" lat=\"38.5\" lon=\"-121.8\"/>\n"
                                                             "  <way id=\"100\">\n"
                                                             "    <nd ref=\"1\"/>\n"
                                                             "    <nd ref=\"2\"/>\n"
                                                             "  </way>\n");
    auto OSMReader = std::make_shared<CXMLReader>(OSMDataSource);
    COpenStreetMap OpenStreetMap(OSMReader);

    EXPECT_EQ(OpenStreetMap.NodeCount(), 2);
    EXPECT_EQ(OpenStreetMap.WayCount(), 1);
    EXPECT_NE(OpenStreetMap.NodeByIndex(0), nullptr);
    EXPECT_NE(OpenStreetMap.NodeByIndex(1), nullptr);
    EXPECT_EQ(OpenStreetMap.NodeByIndex(2), nullptr);
    EXPECT_NE(OpenStreetMap.WayByIndex(0), nullptr);
    EXPECT_EQ(OpenStreetMap.WayByIndex(1), nullptr);
    auto TempNode = OpenStreetMap.NodeByIndex(0);
    ASSERT_NE(TempNode, nullptr);
    EXPECT_EQ(TempNode, OpenStreetMap.NodeByID(TempNode->ID()));
    EXPECT_EQ(TempNode->ID(), 1);
    EXPECT_EQ(TempNode->Location(), std::make_pair(38.5, -121.7));

    auto OSMDataSource2 = std::make_shared<CStringDataSource>("  <node id=\"1\" lat=\"38.5\" lon=\"-121.7\"/>\n"
                                                              "  <node id=\"2\" lat=\"38.5\" lon=\"-121.8\"/>\n"
                                                              "  <way id=\"100\">\n"
                                                              "    <nd ref=\"258592863\"/>\n"
                                                              "    <nd ref=\"4399281377\"/>\n"
                                                              "  </way>\n"
                                                              "</osm>");
    auto OSMReader2 = std::make_shared<CXMLReader>(OSMDataSource2);
    COpenStreetMap OpenStreetMap2(OSMReader2);

    EXPECT_EQ(OpenStreetMap2.NodeCount(), 0);
    EXPECT_EQ(OpenStreetMap2.WayCount(), 0);
    EXPECT_EQ(OpenStreetMap2.NodeByIndex(0), nullptr);
    EXPECT_EQ(OpenStreetMap2.NodeByIndex(1), nullptr);
    EXPECT_EQ(OpenStreetMap2.NodeByIndex(2), nullptr);
    EXPECT_EQ(OpenStreetMap2.WayByIndex(0), nullptr);
    EXPECT_EQ(OpenStreetMap2.WayByIndex(1), nullptr);
    EXPECT_EQ(OpenStreetMap2.WayByIndex(2), nullptr);
}

TEST(OSMTest, OnlyNodes)
{
    auto OSMDataSource = std::make_shared<CStringDataSource>("<?xml version='1.0' encoding='UTF-8'?>\n"
                                                             "<osm version=\"0.6\" generator=\"osmconvert 0.8.5\">\n"
                                                             "  <node id=\"1\" lat=\"38.5\" lon=\"-121.7\"/>\n"
                                                             "  <node id=\"2\" lat=\"38.5\" lon=\"-121.8\"/>\n"
                                                             "</osm>");
    auto OSMReader = std::make_shared<CXMLReader>(OSMDataSource);
    COpenStreetMap OpenStreetMap(OSMReader);

    EXPECT_EQ(OpenStreetMap.NodeCount(), 2);
    EXPECT_EQ(OpenStreetMap.WayCount(), 0);
    EXPECT_NE(OpenStreetMap.NodeByIndex(0), nullptr);
    EXPECT_NE(OpenStreetMap.NodeByIndex(1), nullptr);
    EXPECT_EQ(OpenStreetMap.NodeByIndex(2), nullptr);
    EXPECT_EQ(OpenStreetMap.WayByIndex(0), nullptr);
    EXPECT_EQ(OpenStreetMap.WayByIndex(1), nullptr);
    auto TempNode = OpenStreetMap.NodeByIndex(0);
    ASSERT_NE(TempNode, nullptr);
    EXPECT_EQ(TempNode, OpenStreetMap.NodeByID(TempNode->ID()));
    EXPECT_EQ(TempNode->ID(), 1);
    EXPECT_EQ(TempNode->Location(), std::make_pair(38.5, -121.7));

    auto TempWay = OpenStreetMap.WayByIndex(0);
    ASSERT_EQ(TempWay, nullptr);
}

TEST(OSMTest, OnlyWays)
{
    auto OSMDataSource = std::make_shared<CStringDataSource>("<?xml version='1.0' encoding='UTF-8'?>\n"
                                                             "<osm version=\"0.6\" generator=\"osmconvert 0.8.5\">\n"
                                                             "  <way id=\"100\">\n"
                                                             "    <nd ref=\"1\"/>\n"
                                                             "    <nd ref=\"2\"/>\n"
                                                             "  </way>\n"
                                                             "</osm>");
    auto OSMReader = std::make_shared<CXMLReader>(OSMDataSource);
    COpenStreetMap OpenStreetMap(OSMReader);

    EXPECT_EQ(OpenStreetMap.NodeCount(), 0);
    EXPECT_EQ(OpenStreetMap.WayCount(), 0);
    EXPECT_EQ(OpenStreetMap.NodeByIndex(0), nullptr);
    EXPECT_EQ(OpenStreetMap.NodeByIndex(1), nullptr);
    EXPECT_EQ(OpenStreetMap.NodeByIndex(2), nullptr);
    EXPECT_EQ(OpenStreetMap.WayByIndex(0), nullptr);
    EXPECT_EQ(OpenStreetMap.WayByIndex(1), nullptr);
    auto TempNode = OpenStreetMap.NodeByIndex(0);
    ASSERT_EQ(TempNode, nullptr);

    auto TempWay = OpenStreetMap.WayByIndex(0);
    ASSERT_EQ(TempWay, nullptr);
}

TEST(OSMTest, WayRefNonExistantNode)
{
    auto OSMDataSource = std::make_shared<CStringDataSource>("<?xml version='1.0' encoding='UTF-8'?>\n"
                                                             "<osm version=\"0.6\" generator=\"osmconvert 0.8.5\">\n"
                                                             "  <node id=\"1\" lat=\"38.5\" lon=\"-121.7\"/>\n"
                                                             "  <node id=\"2\" lat=\"38.5\" lon=\"-121.8\"/>\n"
                                                             "<way id=\"100\">\n"
                                                             "    <nd ref=\"258592863\"/>\n"
                                                             "    <nd ref=\"4399281377\"/>\n"
                                                             "  </way>\n"
                                                             "</osm>");
    auto OSMReader = std::make_shared<CXMLReader>(OSMDataSource);
    COpenStreetMap OpenStreetMap(OSMReader);

    EXPECT_EQ(OpenStreetMap.NodeCount(), 2);
    EXPECT_EQ(OpenStreetMap.WayCount(), 0);
    EXPECT_NE(OpenStreetMap.NodeByIndex(0), nullptr);
    EXPECT_NE(OpenStreetMap.NodeByIndex(1), nullptr);
    EXPECT_EQ(OpenStreetMap.NodeByIndex(2), nullptr);
    EXPECT_EQ(OpenStreetMap.WayByIndex(0), nullptr);
    EXPECT_EQ(OpenStreetMap.WayByIndex(1), nullptr);
    auto TempNode = OpenStreetMap.NodeByIndex(0);
    ASSERT_NE(TempNode, nullptr);
    EXPECT_EQ(TempNode, OpenStreetMap.NodeByID(TempNode->ID()));
    EXPECT_EQ(TempNode->ID(), 1);
    EXPECT_EQ(TempNode->Location(), std::make_pair(38.5, -121.7));

    auto TempWay = OpenStreetMap.WayByIndex(0);
    ASSERT_EQ(TempWay, nullptr);
}

TEST(OSMTest, NodeWithTags)
{
    auto OSMDataSource = std::make_shared<CStringDataSource>("<?xml version='1.0' encoding='UTF-8'?>\n"
                                                             "<osm version=\"0.6\" generator=\"osmconvert 0.8.5\">\n"
                                                             "  <node id=\"1\" lat=\"38.5\" lon=\"-121.7\">\n"
                                                             "      <tag k=\"highway\" v=\"motorway_junction\"/>\n"
                                                             "      <tag k=\"ref\" v=\"71\"/>\n"
                                                             "  </node>\n"
                                                             "  <node id=\"2\" lat=\"38.5\" lon=\"-121.8\"/>\n"
                                                             "<node id=\"3\" lat=\"2323.5\" lon=\"-112312.8\"/>\n"
                                                             "  <way id=\"100\">\n"
                                                             "    <nd ref=\"1\"/>\n"
                                                             "    <nd ref=\"2\"/>\n"
                                                             "  </way>\n"
                                                             "</osm>");
    auto OSMReader = std::make_shared<CXMLReader>(OSMDataSource);
    COpenStreetMap OpenStreetMap(OSMReader);

    EXPECT_EQ(OpenStreetMap.NodeCount(), 3);
    EXPECT_EQ(OpenStreetMap.WayCount(), 1);
    EXPECT_NE(OpenStreetMap.NodeByIndex(0), nullptr);
    EXPECT_NE(OpenStreetMap.NodeByIndex(1), nullptr);
    EXPECT_NE(OpenStreetMap.NodeByIndex(2), nullptr);
    EXPECT_EQ(OpenStreetMap.NodeByIndex(3), nullptr);
    EXPECT_NE(OpenStreetMap.WayByIndex(0), nullptr);
    EXPECT_EQ(OpenStreetMap.WayByIndex(1), nullptr);
    auto TempNode = OpenStreetMap.NodeByIndex(0);
    ASSERT_NE(TempNode, nullptr);
    EXPECT_EQ(TempNode, OpenStreetMap.NodeByID(TempNode->ID()));
    EXPECT_EQ(TempNode->ID(), 1);
    EXPECT_EQ(TempNode->Location(), std::make_pair(38.5, -121.7));

    auto TempNode2 = OpenStreetMap.NodeByIndex(1);
    ASSERT_NE(TempNode2, nullptr);
    EXPECT_EQ(TempNode2->GetAttributeKey(1), "");
    EXPECT_EQ(TempNode2->GetAttribute("ref"), "");
    EXPECT_EQ(TempNode->GetAttributeKey(1), "ref");
    EXPECT_EQ(TempNode->GetAttribute("ref"), "71");
    EXPECT_EQ(TempNode->GetAttributeKey(0), "highway");
    EXPECT_EQ(TempNode->GetAttribute("highway"), "motorway_junction");
}

TEST(OSMTest, WayWithTags)
{
    auto OSMDataSource = std::make_shared<CStringDataSource>("<?xml version='1.0' encoding='UTF-8'?>\n"
                                                             "<osm version=\"0.6\" generator=\"osmconvert 0.8.5\">\n"
                                                             "  <node id=\"1\" lat=\"328.5\" lon=\"-1221.7\">\n"
                                                             "      <tag k=\"highway\" v=\"motorway_junction\"/>\n"
                                                             "      <tag k=\"ref\" v=\"71\"/>\n"
                                                             "  </node>\n"
                                                             "  <node id=\"2\" lat=\"38.5\" lon=\"-121.8\">\n"
                                                             "      <tag k=\"morek\" v=\"morevalue\"/>\n"
                                                             "      <tag k=\"secondtag\" v=\"secondv\"/>\n"
                                                             "      <tag k=\"thirdtagnode\" v=\"thirdvaluenode\"/>\n"
                                                             "  </node>\n"
                                                             "  <node id=\"3\" lat=\"2323.5\" lon=\"-112312.8\"/>\n"
                                                             "  <way id=\"100\">\n"
                                                             "    <nd ref=\"1\"/>\n"
                                                             "    <nd ref=\"2\"/>\n"
                                                             "    <tag k=\"waytagk\" v=\"wayvaluev\"/>\n"
                                                             "    <tag k=\"secondtag\" v=\"secondval\"/>\n"
                                                             "  </way>\n"
                                                             "  <way id=\"101\">\n"
                                                             "    <nd ref=\"3\"/>\n"
                                                             "  </way>\n"
                                                             "  <way id=\"102\">\n"
                                                             "    <nd ref=\"3\"/>\n"
                                                             "    <nd ref=\"2\"/>\n"
                                                             "    <tag k=\"newk\" v=\"newv\"/>\n"
                                                             "  </way>\n"
                                                             "</osm>");
    auto OSMReader = std::make_shared<CXMLReader>(OSMDataSource);
    COpenStreetMap OpenStreetMap(OSMReader);

    EXPECT_EQ(OpenStreetMap.NodeCount(), 3);
    EXPECT_EQ(OpenStreetMap.WayCount(), 3);
    EXPECT_NE(OpenStreetMap.NodeByIndex(0), nullptr);
    EXPECT_NE(OpenStreetMap.NodeByIndex(1), nullptr);
    EXPECT_NE(OpenStreetMap.NodeByIndex(2), nullptr);
    EXPECT_EQ(OpenStreetMap.NodeByIndex(3), nullptr);
    EXPECT_NE(OpenStreetMap.WayByIndex(0), nullptr);
    EXPECT_NE(OpenStreetMap.WayByIndex(1), nullptr);
    EXPECT_NE(OpenStreetMap.WayByIndex(2), nullptr);
    EXPECT_EQ(OpenStreetMap.WayByIndex(3), nullptr);
    auto TempNode = OpenStreetMap.NodeByIndex(1);
    ASSERT_NE(TempNode, nullptr);
    EXPECT_EQ(TempNode, OpenStreetMap.NodeByID(TempNode->ID()));
    EXPECT_EQ(TempNode->ID(), 2);
    EXPECT_EQ(TempNode->Location(), std::make_pair(38.5, -121.8));

    auto TempWay = OpenStreetMap.WayByIndex(0);
    ASSERT_NE(TempWay, nullptr);
    EXPECT_EQ(TempWay, OpenStreetMap.WayByID(TempWay->ID()));
    EXPECT_EQ(TempWay->ID(), 100);

    EXPECT_EQ(TempWay->NodeCount(), 2);
    EXPECT_EQ(TempWay->GetNodeID(0), 1);
    EXPECT_EQ(TempWay->GetNodeID(1), 2);
    EXPECT_EQ(TempWay->GetNodeID(2), -1);
    EXPECT_EQ(TempWay->AttributeCount(), 2);
    EXPECT_EQ(TempWay->GetAttributeKey(0), "waytagk");
    EXPECT_EQ(TempWay->GetAttribute("waytagk"), "wayvaluev");
    EXPECT_EQ(TempWay->GetAttributeKey(1), "secondtag");
    EXPECT_EQ(TempWay->GetAttribute("secondtag"), "secondval");
    EXPECT_EQ(TempWay->GetAttributeKey(2), "");
}

TEST(OSMTest, DupTagsInNode)
{
    auto OSMDataSource = std::make_shared<CStringDataSource>("<?xml version='1.0' encoding='UTF-8'?>\n"
                                                             "<osm version=\"0.6\" generator=\"osmconvert 0.8.5\">\n"
                                                             "  <node id=\"1\" lat=\"38.5\" lon=\"-121.7\">\n"
                                                             "      <tag k=\"highway\" v=\"motorway_junction\"/>\n"
                                                             "  </node>\n"
                                                             "  <node id=\"2\" lat=\"38.5\" lon=\"-121.8\">\n"
                                                             "      <tag k=\"highway\" v=\"71\"/>\n"
                                                             "      <tag k=\"highway\" v=\"71\"/>\n"
                                                             "  </node>\n"
                                                             "<node id=\"3\" lat=\"2323.5\" lon=\"-112312.8\"/>\n"
                                                             "  <way id=\"100\">\n"
                                                             "    <nd ref=\"1\"/>\n"
                                                             "    <nd ref=\"2\"/>\n"
                                                             "  </way>\n"
                                                             "</osm>");
    auto OSMReader = std::make_shared<CXMLReader>(OSMDataSource);
    COpenStreetMap OpenStreetMap(OSMReader);

    EXPECT_EQ(OpenStreetMap.NodeCount(), 1);
    EXPECT_EQ(OpenStreetMap.WayCount(), 0);
    EXPECT_NE(OpenStreetMap.NodeByIndex(0), nullptr);
    EXPECT_EQ(OpenStreetMap.NodeByIndex(1), nullptr);
}

TEST(OSMTest, DupNodes)
{
    auto OSMDataSource = std::make_shared<CStringDataSource>("<?xml version='1.0' encoding='UTF-8'?>\n"
                                                             "<osm version=\"0.6\" generator=\"osmconvert 0.8.5\">\n"
                                                             "  <node id=\"1\" lat=\"38.5\" lon=\"-121.7\">\n"
                                                             "      <tag k=\"highway\" v=\"motorway_junction\"/>\n"
                                                             "  </node>\n"
                                                             "  <node id=\"2\" lat=\"38.5\" lon=\"-121.8\">\n"
                                                             "      <tag k=\"highway\" v=\"71\"/>\n"
                                                             "      <tag k=\"dsfd\" v=\"7sd1\"/>\n"
                                                             "  </node>\n"
                                                             "<node id=\"1\" lat=\"2323.5\" lon=\"-112312.8\"/>\n"
                                                             "  <way id=\"100\">\n"
                                                             "    <nd ref=\"1\"/>\n"
                                                             "    <nd ref=\"2\"/>\n"
                                                             "  </way>\n"
                                                             "</osm>");
    auto OSMReader = std::make_shared<CXMLReader>(OSMDataSource);
    COpenStreetMap OpenStreetMap(OSMReader);

    EXPECT_EQ(OpenStreetMap.NodeCount(), 2);
    EXPECT_EQ(OpenStreetMap.WayCount(), 0);
    EXPECT_NE(OpenStreetMap.NodeByIndex(0), nullptr);
    EXPECT_NE(OpenStreetMap.NodeByIndex(1), nullptr);
    EXPECT_EQ(OpenStreetMap.NodeByIndex(2), nullptr);
}

TEST(OSMTest, DupTagsInWay)
{
    auto OSMDataSource = std::make_shared<CStringDataSource>("<?xml version='1.0' encoding='UTF-8'?>\n"
                                                             "<osm version=\"0.6\" generator=\"osmconvert 0.8.5\">\n"
                                                             "  <node id=\"1\" lat=\"38.5\" lon=\"-121.7\">\n"
                                                             "      <tag k=\"highway\" v=\"motorway_junction\"/>\n"
                                                             "  </node>\n"
                                                             "  <node id=\"2\" lat=\"38.5\" lon=\"-121.8\">\n"
                                                             "      <tag k=\"highway\" v=\"71\"/>\n"
                                                             "      <tag k=\"123\" v=\"71\"/>\n"
                                                             "  </node>\n"
                                                             "<node id=\"3\" lat=\"2323.5\" lon=\"-112312.8\"/>\n"
                                                             "  <way id=\"120\">\n"
                                                             "    <nd ref=\"1\"/>\n"
                                                             "    <nd ref=\"2\"/>\n"
                                                             "  </way>\n"
                                                             "  <way id=\"100\">\n"
                                                             "    <nd ref=\"1\"/>\n"
                                                             "    <nd ref=\"2\"/>\n"
                                                             "    <tag k=\"123\" v=\"71\"/>\n"
                                                             "    <tag k=\"123\" v=\"71\"/>\n"
                                                             "  </way>\n"
                                                             "</osm>");
    auto OSMReader = std::make_shared<CXMLReader>(OSMDataSource);
    COpenStreetMap OpenStreetMap(OSMReader);

    EXPECT_EQ(OpenStreetMap.NodeCount(), 3);
    EXPECT_EQ(OpenStreetMap.WayCount(), 1);
    EXPECT_NE(OpenStreetMap.WayByIndex(0), nullptr);
    EXPECT_EQ(OpenStreetMap.WayByIndex(1), nullptr);
}

TEST(OSMTest, DupWays)
{
    auto OSMDataSource = std::make_shared<CStringDataSource>("<?xml version='1.0' encoding='UTF-8'?>\n"
                                                             "<osm version=\"0.6\" generator=\"osmconvert 0.8.5\">\n"
                                                             "  <node id=\"1\" lat=\"38.5\" lon=\"-121.7\">\n"
                                                             "      <tag k=\"highway\" v=\"motorway_junction\"/>\n"
                                                             "  </node>\n"
                                                             "  <node id=\"2\" lat=\"38.5\" lon=\"-121.8\">\n"
                                                             "      <tag k=\"highway\" v=\"71\"/>\n"
                                                             "      <tag k=\"123\" v=\"71\"/>\n"
                                                             "  </node>\n"
                                                             "<node id=\"3\" lat=\"2323.5\" lon=\"-112312.8\"/>\n"
                                                             "  <way id=\"120\">\n"
                                                             "    <nd ref=\"1\"/>\n"
                                                             "    <nd ref=\"2\"/>\n"
                                                             "  </way>\n"
                                                             "  <way id=\"100\">\n"
                                                             "    <nd ref=\"1\"/>\n"
                                                             "    <nd ref=\"1\"/>\n"
                                                             "    <tag k=\"12123\" v=\"71\"/>\n"
                                                             "    <tag k=\"123\" v=\"71\"/>\n"
                                                             "  </way>\n"
                                                             "  <way id=\"100\">\n"
                                                             "    <nd ref=\"123\"/>\n"
                                                             "    <nd ref=\"11231\"/>\n"
                                                             "    <tag k=\"12312123\" v=\"71\"/>\n"
                                                             "    <tag k=\"11231223\" v=\"71\"/>\n"
                                                             "  </way>\n"
                                                             "</osm>");
    auto OSMReader = std::make_shared<CXMLReader>(OSMDataSource);
    COpenStreetMap OpenStreetMap(OSMReader);

    EXPECT_EQ(OpenStreetMap.NodeCount(), 3);
    EXPECT_EQ(OpenStreetMap.WayCount(), 2);
    EXPECT_NE(OpenStreetMap.WayByIndex(0), nullptr);
    EXPECT_NE(OpenStreetMap.WayByIndex(1), nullptr);
    EXPECT_EQ(OpenStreetMap.WayByIndex(2), nullptr);
}