#include <gtest/gtest.h>
#include "CSVBusSystem.h"
#include "StringDataSource.h"
#include "CSVBusSystem.h"

TEST(CSVBusSystem, SimpleFilesTest){
    auto StopDataSource = std::make_shared< CStringDataSource >("stop_id,node_id\n"
                                                                "1,123\n"
                                                                "2,124");
    auto StopReader = std::make_shared< CDSVReader >(StopDataSource,',');
    auto RouteDataSource = std::make_shared< CStringDataSource >("route,stop_id\n"
                                                                "A,1\n"
                                                                "A,2");
    auto RouteReader = std::make_shared< CDSVReader >(RouteDataSource,',');
    
    CCSVBusSystem BusSystem(StopReader,RouteReader);
    
    EXPECT_EQ(BusSystem.StopCount(),2);
    auto StopObj = BusSystem.StopByIndex(0);
    EXPECT_NE(StopObj, nullptr);
    StopObj = BusSystem.StopByIndex(1);
    EXPECT_NE(StopObj, nullptr);
    StopObj = BusSystem.StopByID(1);
    ASSERT_NE(StopObj,nullptr);
    EXPECT_EQ(StopObj->ID(),1);
    EXPECT_EQ(StopObj->NodeID(),123);
    StopObj = BusSystem.StopByID(2);
    ASSERT_NE(StopObj,nullptr);
    EXPECT_EQ(StopObj->ID(),2);
    EXPECT_EQ(StopObj->NodeID(),124);

}


TEST(CSVBusSystem, SimpleFilesSwappedTest){
    auto StopDataSource = std::make_shared< CStringDataSource >("node_id,stop_id\n"
                                                                "123,1\n"
                                                                "124,2");
    auto StopReader = std::make_shared< CDSVReader >(StopDataSource,',');
    auto RouteDataSource = std::make_shared< CStringDataSource >("stop_id,route\n"
                                                                "1,A\n"
                                                                "2,A");
    auto RouteReader = std::make_shared< CDSVReader >(RouteDataSource,',');
    
    CCSVBusSystem BusSystem(StopReader,RouteReader);
    
    EXPECT_EQ(BusSystem.StopCount(),2);
    auto StopObj = BusSystem.StopByIndex(0);
    EXPECT_NE(StopObj,nullptr);
    StopObj = BusSystem.StopByIndex(1);
    EXPECT_NE(StopObj,nullptr);
    StopObj = BusSystem.StopByID(1);
    ASSERT_NE(StopObj,nullptr);
    EXPECT_EQ(StopObj->ID(),1);
    EXPECT_EQ(StopObj->NodeID(),123);
    StopObj = BusSystem.StopByID(2);
    ASSERT_NE(StopObj,nullptr);
    EXPECT_EQ(StopObj->ID(),2);
    EXPECT_EQ(StopObj->NodeID(),124);

}



// multiple routes test 
// route with no stops
// basic routes
// out of bounds stuff 

TEST(CSVBusSystem, MutipleRoutesTest){
    auto StopDataSource = std::make_shared< CStringDataSource >("node_id,stop_id\n"
                                                                "123,1\n"
                                                                "124,2\n"
                                                                "139,3");
    auto StopReader = std::make_shared< CDSVReader >(StopDataSource,',');
    auto RouteDataSource = std::make_shared< CStringDataSource >("stop_id,route\n"
                                                                "1,A\n"
                                                                "2,A\n"
                                                                "1,B\n"    
                                                                "3,B");
    auto RouteReader = std::make_shared< CDSVReader >(RouteDataSource,',');
    
    CCSVBusSystem BusSystem(StopReader,RouteReader);
    
    EXPECT_EQ(BusSystem.StopCount(),3);
    auto StopObj = BusSystem.StopByIndex(0);
    EXPECT_NE(StopObj,nullptr);
    StopObj = BusSystem.StopByIndex(1);
    EXPECT_NE(StopObj,nullptr);
    StopObj = BusSystem.StopByID(1);
    ASSERT_NE(StopObj,nullptr);
    EXPECT_EQ(StopObj->ID(),1);
    EXPECT_EQ(StopObj->NodeID(),123);
    StopObj = BusSystem.StopByID(2);
    ASSERT_NE(StopObj,nullptr);
    EXPECT_EQ(StopObj->ID(),2);
    EXPECT_EQ(StopObj->NodeID(),124);
    StopObj = BusSystem.StopByID(3);
    ASSERT_NE(StopObj,nullptr);
    EXPECT_EQ(StopObj->ID(),3);
    EXPECT_EQ(StopObj->NodeID(),139);
    EXPECT_EQ(BusSystem.RouteCount(),2);
    auto RouteA = BusSystem.RouteByName("A");
    EXPECT_EQ(RouteA->Name(), "A");
    EXPECT_EQ(RouteA->StopCount(), 2);
    EXPECT_EQ(RouteA->GetStopID(0), 1);
    EXPECT_EQ(RouteA->GetStopID(1), 2);
    auto RouteB = BusSystem.RouteByName("B");
    EXPECT_EQ(RouteB->Name(), "B");
    EXPECT_EQ(RouteB->StopCount(), 2);
    EXPECT_EQ(RouteB->GetStopID(0), 1);
    EXPECT_EQ(RouteB->GetStopID(1), 3);

}

// stopbyid
// route by index done
// if index is higher than stop count or route count 

TEST(CSVBusSystem, RootByIndexTest){
    auto StopDataSource = std::make_shared< CStringDataSource >("stop_id,node_id\n"
                                                                "1,123\n"
                                                                "2,124");
    auto StopReader = std::make_shared< CDSVReader >(StopDataSource,',');
    auto RouteDataSource = std::make_shared< CStringDataSource >("route,stop_id\n"
                                                                "A,1\n"
                                                                "A,2");
    auto RouteReader = std::make_shared< CDSVReader >(RouteDataSource,',');
    
    CCSVBusSystem BusSystem(StopReader,RouteReader);
    
    EXPECT_EQ(BusSystem.StopCount(),2);
    auto StopObj = BusSystem.StopByIndex(0);
    EXPECT_NE(StopObj,nullptr);
    StopObj = BusSystem.StopByIndex(1);
    EXPECT_NE(StopObj,nullptr);
    StopObj = BusSystem.StopByID(1);
    ASSERT_NE(StopObj,nullptr);
    EXPECT_EQ(StopObj->ID(),1);
    EXPECT_EQ(StopObj->NodeID(),123);
    StopObj = BusSystem.StopByID(2);
    ASSERT_NE(StopObj,nullptr);
    EXPECT_EQ(StopObj->ID(),2);
    EXPECT_EQ(StopObj->NodeID(),124);
    auto RouteA = BusSystem.RouteByIndex(0);
    EXPECT_EQ(RouteA->Name(), "A");
    EXPECT_EQ(RouteA->StopCount(), 2);
    EXPECT_EQ(RouteA->GetStopID(0), 1);
    EXPECT_EQ(RouteA->GetStopID(1), 2);

}

TEST(CSVBusSystem, IndexErrorTest){
    auto StopDataSource = std::make_shared< CStringDataSource >("stop_id,node_id\n"
                                                                "1,123\n"
                                                                "2,124");
    auto StopReader = std::make_shared< CDSVReader >(StopDataSource,',');
    auto RouteDataSource = std::make_shared< CStringDataSource >("route,stop_id\n"
                                                                "A,1\n"
                                                                "A,2");
    auto RouteReader = std::make_shared< CDSVReader >(RouteDataSource,',');
    
    CCSVBusSystem BusSystem(StopReader,RouteReader);
    
    
    auto RouteA = BusSystem.RouteByIndex(2);
    EXPECT_EQ(RouteA, nullptr);
    RouteA = BusSystem.RouteByIndex(0);
    EXPECT_EQ(RouteA->GetStopID(3), CBusSystem::InvalidStopID);
    auto RouteB = BusSystem.RouteByName("B");
    EXPECT_EQ(RouteB, nullptr);
    auto StopObj = BusSystem.StopByID(4);
    EXPECT_EQ(StopObj, nullptr);
    StopObj = BusSystem.StopByIndex(4);
    EXPECT_EQ(StopObj, nullptr);


}
