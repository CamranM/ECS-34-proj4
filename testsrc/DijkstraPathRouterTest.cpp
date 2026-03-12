#include <gtest/gtest.h>
#include "DijkstraPathRouter.h"

TEST(DijkstraPathRouter, SimpleTest)
{
    CDijkstraPathRouter PathRouter;

    EXPECT_EQ(PathRouter.VertexCount(), 0);
    auto VertexID = PathRouter.AddVertex(std::string("foo"));
    EXPECT_EQ(PathRouter.VertexCount(), 1);
    auto VetextTag = std::any_cast<std::string>(PathRouter.GetVertexTag(VertexID));
    EXPECT_EQ(VetextTag, "foo");
}

TEST(DijkstraPathRouter, ShortestPath)
{
    CDijkstraPathRouter PathRouter;
    /*
                5
       A ---> B --->C
          4  2|  ___^
              V / 1
              D

    */
    auto VertexA = PathRouter.AddVertex(std::string("A"));
    auto VertexB = PathRouter.AddVertex(std::string("B"));
    auto VertexC = PathRouter.AddVertex(std::string("C"));
    auto VertexD = PathRouter.AddVertex(std::string("D"));
    EXPECT_EQ(PathRouter.VertexCount(), 4);
    EXPECT_TRUE(PathRouter.AddEdge(VertexA, VertexB, 4.0));
    EXPECT_TRUE(PathRouter.AddEdge(VertexB, VertexC, 5.0));
    EXPECT_TRUE(PathRouter.AddEdge(VertexB, VertexD, 2.0));
    EXPECT_TRUE(PathRouter.AddEdge(VertexD, VertexC, 1.0));
    std::vector<CPathRouter::TVertexID> Path;
    EXPECT_EQ(PathRouter.FindShortestPath(VertexA, VertexC, Path), 7.0);
    std::vector<CPathRouter::TVertexID> ExpectedPath{VertexA, VertexB, VertexD, VertexC};
    EXPECT_EQ(Path, ExpectedPath);
}

// TEST(DijkstraPathRouter, NoPath)
// {
//     CDijkstraPathRouter PathRouter;

//     auto VertexA = PathRouter.AddVertex(std::string("A"));
//     auto VertexB = PathRouter.AddVertex(std::string("B"));
//     auto VertexC = PathRouter.AddVertex(std::string("C"));
//     auto VertexD = PathRouter.AddVertex(std::string("D"));
//     EXPECT_EQ(PathRouter.VertexCount(), 4);
//     EXPECT_TRUE(PathRouter.AddEdge(VertexA, VertexB, 4.0));

//     EXPECT_TRUE(PathRouter.AddEdge(VertexA, VertexD, 1.0));
//     std::vector<CPathRouter::TVertexID> Path;
//     EXPECT_EQ(PathRouter.FindShortestPath(VertexA, VertexC, Path), CPathRouter::NoPathExists);
//     std::vector<CPathRouter::TVertexID> ExpectedPath{};
//     EXPECT_EQ(Path, ExpectedPath);
// }

TEST(DijkstraPathRouter, SameSrcDest)
{
    CDijkstraPathRouter PathRouter;

    auto VertexA = PathRouter.AddVertex(std::string("A"));
    auto VertexB = PathRouter.AddVertex(std::string("B"));
    auto VertexC = PathRouter.AddVertex(std::string("C"));
    auto VertexD = PathRouter.AddVertex(std::string("D"));
    EXPECT_EQ(PathRouter.VertexCount(), 4);
    EXPECT_TRUE(PathRouter.AddEdge(VertexA, VertexB, 4.0));

    EXPECT_TRUE(PathRouter.AddEdge(VertexA, VertexD, 1.0));
    std::vector<CPathRouter::TVertexID> Path;
    EXPECT_EQ(PathRouter.FindShortestPath(VertexA, VertexA, Path), 0.0);
    std::vector<CPathRouter::TVertexID> ExpectedPath{VertexA};
    EXPECT_EQ(Path, ExpectedPath);
}

TEST(DijkstraPathRouter, Circle)
{
    CDijkstraPathRouter PathRouter;

    auto VertexA = PathRouter.AddVertex(std::string("A"));
    auto VertexB = PathRouter.AddVertex(std::string("B"));
    auto VertexC = PathRouter.AddVertex(std::string("C"));
    auto VertexD = PathRouter.AddVertex(std::string("D"));
    EXPECT_EQ(PathRouter.VertexCount(), 4);
    EXPECT_TRUE(PathRouter.AddEdge(VertexA, VertexB, 1.0));
    EXPECT_TRUE(PathRouter.AddEdge(VertexB, VertexC, 1.0));
    EXPECT_TRUE(PathRouter.AddEdge(VertexC, VertexD, 1.0));
    EXPECT_TRUE(PathRouter.AddEdge(VertexD, VertexA, 1.0));
    std::vector<CPathRouter::TVertexID> Path;
    EXPECT_EQ(PathRouter.FindShortestPath(VertexA, VertexD, Path), 3.0);
    std::vector<CPathRouter::TVertexID> ExpectedPath{VertexA, VertexB, VertexC, VertexD};
    EXPECT_EQ(Path, ExpectedPath);

    EXPECT_TRUE(PathRouter.AddEdge(VertexA, VertexD, 2.0));
    std::vector<CPathRouter::TVertexID> newPath;
    EXPECT_EQ(PathRouter.FindShortestPath(VertexA, VertexD, newPath), 2.0);
    std::vector<CPathRouter::TVertexID> ExpectedPathnew{VertexA, VertexD};
    EXPECT_EQ(newPath, ExpectedPathnew);
}

TEST(DijkstraPathRouter, LargerGraph)
{
    CDijkstraPathRouter PathRouter;

    auto VertexA = PathRouter.AddVertex(std::string("A"));
    auto VertexB = PathRouter.AddVertex(std::string("B"));
    auto VertexC = PathRouter.AddVertex(std::string("C"));
    auto VertexD = PathRouter.AddVertex(std::string("D"));
    auto VertexE = PathRouter.AddVertex(std::string("E"));
    auto VertexF = PathRouter.AddVertex(std::string("F"));

    EXPECT_EQ(PathRouter.VertexCount(), 6);
    EXPECT_TRUE(PathRouter.AddEdge(VertexA, VertexB, 2.0));
    EXPECT_TRUE(PathRouter.AddEdge(VertexB, VertexC, 3.0));
    EXPECT_TRUE(PathRouter.AddEdge(VertexC, VertexD, 3.0));
    EXPECT_TRUE(PathRouter.AddEdge(VertexC, VertexF, 5.0));
    EXPECT_TRUE(PathRouter.AddEdge(VertexF, VertexC, 5.0));
    EXPECT_TRUE(PathRouter.AddEdge(VertexD, VertexF, 7.0));
    EXPECT_TRUE(PathRouter.AddEdge(VertexE, VertexD, 10.0));
    EXPECT_TRUE(PathRouter.AddEdge(VertexF, VertexE, 2.0));
    EXPECT_TRUE(PathRouter.AddEdge(VertexF, VertexA, 4.0));
    EXPECT_TRUE(PathRouter.AddEdge(VertexE, VertexA, 2.0));
    EXPECT_TRUE(PathRouter.AddEdge(VertexD, VertexA, 1.0));

    std::vector<CPathRouter::TVertexID> ogPath;
    EXPECT_EQ(PathRouter.FindShortestPath(VertexA, VertexA, ogPath), 0.0);
    std::vector<CPathRouter::TVertexID> ogExpectedPath{VertexA};
    EXPECT_EQ(ogPath, ogExpectedPath);

    std::vector<CPathRouter::TVertexID> Path;
    EXPECT_EQ(PathRouter.FindShortestPath(VertexA, VertexD, Path), 8.0);
    std::vector<CPathRouter::TVertexID> ExpectedPath{VertexA, VertexB, VertexC, VertexD};
    EXPECT_EQ(Path, ExpectedPath);

    std::vector<CPathRouter::TVertexID> newPath;
    EXPECT_EQ(PathRouter.FindShortestPath(VertexA, VertexF, newPath), 10.0);
    std::vector<CPathRouter::TVertexID> ExpectedPath1{VertexA, VertexB, VertexC, VertexF};
    EXPECT_EQ(newPath, ExpectedPath1);

    std::vector<CPathRouter::TVertexID> newPath2;
    EXPECT_EQ(PathRouter.FindShortestPath(VertexA, VertexC, newPath2), 5.0);
    std::vector<CPathRouter::TVertexID> ExpectedPath2{VertexA, VertexB, VertexC};
    EXPECT_EQ(newPath2, ExpectedPath2);

    std::vector<CPathRouter::TVertexID> newPath3;
    EXPECT_EQ(PathRouter.FindShortestPath(VertexC, VertexA, newPath3), 4.0);
    std::vector<CPathRouter::TVertexID> ExpectedPath3{VertexC, VertexD, VertexA};
    EXPECT_EQ(newPath3, ExpectedPath3);
}

TEST(DijkstraPathRouter, MultipleCorrect)
{
    CDijkstraPathRouter PathRouter;

    auto VertexA = PathRouter.AddVertex(std::string("A"));
    auto VertexB = PathRouter.AddVertex(std::string("B"));
    auto VertexC = PathRouter.AddVertex(std::string("C"));
    auto VertexD = PathRouter.AddVertex(std::string("D"));
    EXPECT_EQ(PathRouter.VertexCount(), 4);
    EXPECT_TRUE(PathRouter.AddEdge(VertexA, VertexB, 1.0));
    EXPECT_TRUE(PathRouter.AddEdge(VertexB, VertexC, 1.0));
    EXPECT_TRUE(PathRouter.AddEdge(VertexC, VertexD, 1.0));
    EXPECT_TRUE(PathRouter.AddEdge(VertexD, VertexA, 1.0));
    EXPECT_TRUE(PathRouter.AddEdge(VertexA, VertexD, 3.0));
    std::vector<CPathRouter::TVertexID> Path;
    EXPECT_EQ(PathRouter.FindShortestPath(VertexA, VertexD, Path), 3.0);
    std::vector<CPathRouter::TVertexID> ExpectedPath{VertexA, VertexB, VertexC, VertexD};
    std::vector<CPathRouter::TVertexID> ExpectedPath2{VertexA, VertexD};

    // hopefully this works as expected
    EXPECT_TRUE(Path == ExpectedPath || Path == ExpectedPath2);
}
