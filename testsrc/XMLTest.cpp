#include <gtest/gtest.h>
#include "StringDataSink.h"
#include "StringDataSource.h"
#include "XMLWriter.h"
#include "XMLReader.h"

// reader tests
TEST(XMLReader, SimpleTest)
{
    std::string XMLString = "<tag></tag>";
    std::shared_ptr<CStringDataSource> DataSource = std::make_shared<CStringDataSource>(XMLString);
    CXMLReader Reader(DataSource);
    SXMLEntity TempEntity;
    EXPECT_TRUE(Reader.ReadEntity(TempEntity));
    EXPECT_EQ(TempEntity.DType, SXMLEntity::EType::StartElement);
    EXPECT_EQ(TempEntity.DNameData, "tag");
    EXPECT_TRUE(TempEntity.DAttributes.empty());
    EXPECT_TRUE(Reader.ReadEntity(TempEntity));
    EXPECT_EQ(TempEntity.DType, SXMLEntity::EType::EndElement);
    EXPECT_EQ(TempEntity.DNameData, "tag");
    EXPECT_TRUE(TempEntity.DAttributes.empty());

    std::string XMLString2 = "<song>";
    std::shared_ptr<CStringDataSource> DataSource2 = std::make_shared<CStringDataSource>(XMLString2);
    CXMLReader Reader2(DataSource2);
    SXMLEntity TempEntity2;

    EXPECT_TRUE(Reader2.ReadEntity(TempEntity2));
    EXPECT_EQ(TempEntity2.DType, SXMLEntity::EType::StartElement);
    EXPECT_EQ(TempEntity2.DNameData, "song");
    EXPECT_TRUE(TempEntity2.DAttributes.empty());

    EXPECT_FALSE(Reader2.ReadEntity(TempEntity2));
}

TEST(XMLReader, CharDataTest)
{
    std::string XMLString = "<tag attr1=\"val1\">Some Data</tag>";
    std::shared_ptr<CStringDataSource> DataSource = std::make_shared<CStringDataSource>(XMLString);
    CXMLReader Reader(DataSource);
    SXMLEntity TempEntity;
    EXPECT_TRUE(Reader.ReadEntity(TempEntity));
    EXPECT_EQ(TempEntity.DType, SXMLEntity::EType::StartElement);
    EXPECT_EQ(TempEntity.DNameData, "tag");
    ASSERT_EQ(TempEntity.DAttributes.size(), 1);
    EXPECT_EQ(TempEntity.AttributeValue("attr1"), "val1");
    EXPECT_TRUE(Reader.ReadEntity(TempEntity));
    EXPECT_EQ(TempEntity.DType, SXMLEntity::EType::CharData);
    EXPECT_EQ(TempEntity.DNameData, "Some Data");
    EXPECT_TRUE(Reader.ReadEntity(TempEntity));
    EXPECT_EQ(TempEntity.DType, SXMLEntity::EType::EndElement);
    EXPECT_EQ(TempEntity.DNameData, "tag");
    EXPECT_TRUE(TempEntity.DAttributes.empty());

    std::string XMLString2 = "<music><food>eating food and listening to music</food></music>";
    std::shared_ptr<CStringDataSource> DataSource2 = std::make_shared<CStringDataSource>(XMLString2);
    CXMLReader Reader2(DataSource2);
    SXMLEntity TempEntity2;
    EXPECT_TRUE(Reader2.ReadEntity(TempEntity2));
    EXPECT_EQ(TempEntity2.DType, SXMLEntity::EType::StartElement);
    EXPECT_EQ(TempEntity2.DNameData, "music");
    EXPECT_TRUE(TempEntity2.DAttributes.empty());

    EXPECT_TRUE(Reader2.ReadEntity(TempEntity2));
    EXPECT_EQ(TempEntity2.DType, SXMLEntity::EType::StartElement);
    EXPECT_EQ(TempEntity2.DNameData, "food");
    EXPECT_TRUE(TempEntity2.DAttributes.empty());

    EXPECT_TRUE(Reader2.ReadEntity(TempEntity2));
    EXPECT_EQ(TempEntity2.DType, SXMLEntity::EType::CharData);
    EXPECT_EQ(TempEntity2.DNameData, "eating food and listening to music");
    EXPECT_TRUE(TempEntity2.DAttributes.empty());

    EXPECT_TRUE(Reader2.ReadEntity(TempEntity2));
    EXPECT_EQ(TempEntity2.DType, SXMLEntity::EType::EndElement);
    EXPECT_EQ(TempEntity2.DNameData, "food");
    EXPECT_TRUE(TempEntity2.DAttributes.empty());

    EXPECT_TRUE(Reader2.ReadEntity(TempEntity2));
    EXPECT_EQ(TempEntity2.DType, SXMLEntity::EType::EndElement);
    EXPECT_EQ(TempEntity2.DNameData, "music");
    EXPECT_TRUE(TempEntity2.DAttributes.empty());

    EXPECT_FALSE(Reader2.ReadEntity(TempEntity2));
}

TEST(XMLReader, AttributeTest)
{

    std::string XMLString = "<music song=\"hello\" song2=\"hey jude\">";
    std::shared_ptr<CStringDataSource> DataSource = std::make_shared<CStringDataSource>(XMLString);
    CXMLReader Reader(DataSource);
    SXMLEntity TempEntity;

    EXPECT_TRUE(Reader.ReadEntity(TempEntity));
    EXPECT_EQ(TempEntity.DType, SXMLEntity::EType::StartElement);
    EXPECT_EQ(TempEntity.DNameData, "music");
    ASSERT_EQ(TempEntity.DAttributes.size(), 2);
    EXPECT_EQ(TempEntity.AttributeValue("song"), "hello");
    EXPECT_EQ(TempEntity.AttributeValue("song2"), "hey jude");
}

TEST(XMLReader, StartEndTest)
{

    std::string XMLString = "<music song=\"hello\" song2=\"hey jude\"></music>";
    std::shared_ptr<CStringDataSource> DataSource = std::make_shared<CStringDataSource>(XMLString);
    CXMLReader Reader(DataSource);
    SXMLEntity TempEntity;

    EXPECT_TRUE(Reader.ReadEntity(TempEntity));
    EXPECT_EQ(TempEntity.DType, SXMLEntity::EType::StartElement);
    EXPECT_EQ(TempEntity.DNameData, "music");
    ASSERT_EQ(TempEntity.DAttributes.size(), 2);
    EXPECT_EQ(TempEntity.AttributeValue("song"), "hello");
    EXPECT_EQ(TempEntity.AttributeValue("song2"), "hey jude");

    EXPECT_TRUE(Reader.ReadEntity(TempEntity));
    EXPECT_EQ(TempEntity.DType, SXMLEntity::EType::EndElement);
    EXPECT_EQ(TempEntity.DNameData, "music");
    EXPECT_TRUE(TempEntity.DAttributes.empty());
}

TEST(XMLReader, SkipData)
{
    std::string XMLString = "<music><food>eating food and listening to music</food></music>";
    std::shared_ptr<CStringDataSource> DataSource = std::make_shared<CStringDataSource>(XMLString);
    CXMLReader Reader(DataSource);
    SXMLEntity TempEntity;

    EXPECT_TRUE(Reader.ReadEntity(TempEntity, true));
    EXPECT_EQ(TempEntity.DType, SXMLEntity::EType::StartElement);
    EXPECT_EQ(TempEntity.DNameData, "music");
    EXPECT_TRUE(TempEntity.DAttributes.empty());

    EXPECT_TRUE(Reader.ReadEntity(TempEntity, true));
    EXPECT_EQ(TempEntity.DType, SXMLEntity::EType::StartElement);
    EXPECT_EQ(TempEntity.DNameData, "food");
    EXPECT_TRUE(TempEntity.DAttributes.empty());

    EXPECT_TRUE(Reader.ReadEntity(TempEntity, true));
    EXPECT_EQ(TempEntity.DType, SXMLEntity::EType::EndElement);
    EXPECT_EQ(TempEntity.DNameData, "food");
    EXPECT_TRUE(TempEntity.DAttributes.empty());

    EXPECT_TRUE(Reader.ReadEntity(TempEntity, true));
    EXPECT_EQ(TempEntity.DType, SXMLEntity::EType::EndElement);
    EXPECT_EQ(TempEntity.DNameData, "music");
    EXPECT_TRUE(TempEntity.DAttributes.empty());

    EXPECT_FALSE(Reader.ReadEntity(TempEntity, true));
}


TEST(XMLReader, SpecialChar)
{
    std::string XMLString = "<music><food>eating &lt; food &gt; &amp; listening &apos;to&apos; &quot;music&quot;</food></music>";
    std::shared_ptr<CStringDataSource> DataSource = std::make_shared<CStringDataSource>(XMLString);
    CXMLReader Reader(DataSource);
    SXMLEntity TempEntity;

    EXPECT_TRUE(Reader.ReadEntity(TempEntity));
    EXPECT_EQ(TempEntity.DType, SXMLEntity::EType::StartElement);
    EXPECT_EQ(TempEntity.DNameData, "music");
    EXPECT_TRUE(TempEntity.DAttributes.empty());

    EXPECT_TRUE(Reader.ReadEntity(TempEntity));
    EXPECT_EQ(TempEntity.DType, SXMLEntity::EType::StartElement);
    EXPECT_EQ(TempEntity.DNameData, "food");
    EXPECT_TRUE(TempEntity.DAttributes.empty());

    EXPECT_TRUE(Reader.ReadEntity(TempEntity));
    EXPECT_EQ(TempEntity.DType, SXMLEntity::EType::CharData);
    EXPECT_EQ(TempEntity.DNameData, "eating < food > & listening \'to\' \"music\"");
    EXPECT_TRUE(TempEntity.DAttributes.empty());

    EXPECT_TRUE(Reader.ReadEntity(TempEntity));
    EXPECT_EQ(TempEntity.DType, SXMLEntity::EType::EndElement);
    EXPECT_EQ(TempEntity.DNameData, "food");
    EXPECT_TRUE(TempEntity.DAttributes.empty());

    EXPECT_TRUE(Reader.ReadEntity(TempEntity));
    EXPECT_EQ(TempEntity.DType, SXMLEntity::EType::EndElement);
    EXPECT_EQ(TempEntity.DNameData, "music");
    EXPECT_TRUE(TempEntity.DAttributes.empty());

    EXPECT_FALSE(Reader.ReadEntity(TempEntity));
}


// XML Writer
TEST(XMLWriter, SimpleTest)
{
    std::shared_ptr<CStringDataSink> DataSink = std::make_shared<CStringDataSink>();
    CXMLWriter Writer(DataSink);
    SXMLEntity sxml;
    sxml.DType = SXMLEntity::EType::StartElement;
    sxml.DNameData = "song";
    EXPECT_TRUE(Writer.WriteEntity(sxml));
    EXPECT_EQ(DataSink->String(), "<song>");
}

TEST(XMLWriter, AttributeTest)
{
    std::shared_ptr<CStringDataSink> DataSink = std::make_shared<CStringDataSink>();
    CXMLWriter Writer(DataSink);
    SXMLEntity sxml;
    sxml.DType = SXMLEntity::EType::StartElement;
    sxml.DNameData = "music";
    sxml.DAttributes = {{"song", "hello"}, {"song2", "hey jude"}};
    EXPECT_TRUE(Writer.WriteEntity(sxml));
    EXPECT_EQ(DataSink->String(), "<music song=\"hello\" song2=\"hey jude\">");
}

TEST(XMLWriter, StartEndTest)
{
    std::shared_ptr<CStringDataSink> DataSink = std::make_shared<CStringDataSink>();
    CXMLWriter Writer(DataSink);
    SXMLEntity sxml;
    sxml.DType = SXMLEntity::EType::StartElement;
    sxml.DNameData = "music";
    sxml.DAttributes = {{"song", "hello"}, {"song", "hey jude"}};
    EXPECT_TRUE(Writer.WriteEntity(sxml));
    EXPECT_EQ(DataSink->String(), "<music song=\"hello\" song=\"hey jude\">");
    sxml.DType = SXMLEntity::EType::EndElement;
    EXPECT_TRUE(Writer.WriteEntity(sxml));
    EXPECT_EQ(DataSink->String(), "<music song=\"hello\" song=\"hey jude\"></music>");
}

TEST(XMLWriter, CompleteTest)
{
    std::shared_ptr<CStringDataSink> DataSink = std::make_shared<CStringDataSink>();
    CXMLWriter Writer(DataSink);
    SXMLEntity sxml;
    sxml.DType = SXMLEntity::EType::CompleteElement;
    sxml.DNameData = "music";
    sxml.DAttributes = {{"song", "hello"}, {"song", "hey jude"}};
    EXPECT_TRUE(Writer.WriteEntity(sxml));
    EXPECT_EQ(DataSink->String(), "<music song=\"hello\" song=\"hey jude\"/>");
}

TEST(XMLWriter, FlushTest)
{
    std::shared_ptr<CStringDataSink> DataSink = std::make_shared<CStringDataSink>();
    CXMLWriter Writer(DataSink);
    SXMLEntity sxml;
    sxml.DType = SXMLEntity::EType::StartElement;
    sxml.DNameData = "music";
    EXPECT_TRUE(Writer.WriteEntity(sxml));
    EXPECT_EQ(DataSink->String(), "<music>");
    sxml.DNameData = "food";
    EXPECT_TRUE(Writer.WriteEntity(sxml));
    EXPECT_EQ(DataSink->String(), "<music><food>");
    EXPECT_TRUE(Writer.Flush());
    EXPECT_EQ(DataSink->String(), "<music><food></food></music>");
}

TEST(XMLWriter, CharTest)
{
    std::shared_ptr<CStringDataSink> DataSink = std::make_shared<CStringDataSink>();
    CXMLWriter Writer(DataSink);
    SXMLEntity sxml;
    sxml.DType = SXMLEntity::EType::StartElement;
    sxml.DNameData = "music";
    EXPECT_TRUE(Writer.WriteEntity(sxml));
    EXPECT_EQ(DataSink->String(), "<music>");
    sxml.DNameData = "food";
    EXPECT_TRUE(Writer.WriteEntity(sxml));
    EXPECT_EQ(DataSink->String(), "<music><food>");
    sxml.DType = SXMLEntity::EType::CharData;
    sxml.DNameData = "eating food and listening to music";
    EXPECT_TRUE(Writer.WriteEntity(sxml));
    EXPECT_EQ(DataSink->String(), "<music><food>eating food and listening to music");
    EXPECT_TRUE(Writer.Flush());
    EXPECT_EQ(DataSink->String(), "<music><food>eating food and listening to music</food></music>");
}

TEST(XMLWriter, SingleFlushTest)
{
    std::shared_ptr<CStringDataSink> DataSink = std::make_shared<CStringDataSink>();
    CXMLWriter Writer(DataSink);
    SXMLEntity sxml;
    sxml.DType = SXMLEntity::EType::StartElement;
    sxml.DNameData = "music";
    EXPECT_TRUE(Writer.WriteEntity(sxml));
    EXPECT_EQ(DataSink->String(), "<music>");
    sxml.DNameData = "food";
    EXPECT_TRUE(Writer.WriteEntity(sxml));
    EXPECT_EQ(DataSink->String(), "<music><food>");
    sxml.DType = SXMLEntity::EType::EndElement;
    EXPECT_TRUE(Writer.WriteEntity(sxml));
    EXPECT_EQ(DataSink->String(), "<music><food></food>");
    EXPECT_TRUE(Writer.Flush());
    EXPECT_EQ(DataSink->String(), "<music><food></food></music>");
}

TEST(XMLWriter, SpecialChar_AttributeTest){
    std::shared_ptr<CStringDataSink> DataSink = std::make_shared<CStringDataSink>();
    CXMLWriter Writer(DataSink);
    SXMLEntity sxml;
    sxml.DType = SXMLEntity::EType::StartElement;
    sxml.DNameData = "music";
    sxml.DAttributes = {{"song<>","hello<>"}, {"song","hey jude"}};
    EXPECT_TRUE(Writer.WriteEntity(sxml));
    EXPECT_EQ(DataSink->String(), "<music song&lt&gt=\"hello&lt&gt\" song=\"hey jude\">");
}

TEST(XMLWriter, SpecialChar_CharDataTest){
    std::shared_ptr<CStringDataSink> DataSink = std::make_shared<CStringDataSink>();
    CXMLWriter Writer(DataSink);
    SXMLEntity sxml;
    sxml.DType = SXMLEntity::EType::StartElement;
    sxml.DNameData = "music";
    EXPECT_TRUE(Writer.WriteEntity(sxml));
    EXPECT_EQ(DataSink->String(), "<music>");
    sxml.DNameData = "food";
    EXPECT_TRUE(Writer.WriteEntity(sxml));
    EXPECT_EQ(DataSink->String(), "<music><food>");
    sxml.DType = SXMLEntity::EType::CharData;
    sxml.DNameData = "eating food and listening to music&\"\'";
    EXPECT_TRUE(Writer.WriteEntity(sxml));
    EXPECT_EQ(DataSink->String(), "<music><food>eating food and listening to music&amp&quot&apos");
}
