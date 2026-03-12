#include "DijkstraPathRouter.h"
#include <algorithm>

struct CDijkstraPathRouter::SImplementation
{
    struct SVertex;
    using TEdge = std::pair<double, std::shared_ptr<SVertex>>; // contains the weight and the vertex
    struct SVertex
    { // one vertex
        std::vector<TEdge> DEdges;
        std::any DTag;
    };
    std::vector<std::shared_ptr<SVertex>> DVertices; // all vertices in the graph

    SImplementation()
    {
    }
    ~SImplementation()
    {
    }

    // each vertex contains a list of edges and a tag, and an edge is in a vector
    // where the edge is associated with a weight and a pointer to the vertex destintion

    std::size_t VertexCount() const noexcept
    { // how many vertices exist
        return DVertices.size();
    }

    TVertexID AddVertex(std::any tag) noexcept
    { // creates a new vertex and adds the new vertex to dvertices
        auto NewVertex = std::make_shared<SVertex>();
        NewVertex->DTag = tag;
        TVertexID NewID = DVertices.size();
        DVertices.push_back(NewVertex);
        return NewID;
    }

    std::any GetVertexTag(TVertexID id) const noexcept
    { // gets the tag of a given vertex
        if (id < DVertices.size())
        {
            return DVertices[id]->DTag;
        }
        return std::any();
    }

    bool AddEdge(TVertexID src, TVertexID dest, double weight, bool bidir = false) noexcept
    {
        if (weight < 0)
        {
            return false;
        }
        if (src < DVertices.size() && dest < DVertices.size())
        {
            DVertices[src]->DEdges.push_back(std::make_pair(weight, DVertices[dest]));
            if (bidir)
            {
                DVertices[dest]->DEdges.push_back(std::make_pair(weight, DVertices[src]));
            }
            return true;
        }
        return false;
        // not done magnitude check
        // adds an edge to the
    }

    bool Precompute(std::chrono::steady_clock::time_point deadline) noexcept
    {
        return true; // for extra credit
    }

    double FindShortestPath(TVertexID src, TVertexID dest, std::vector<TVertexID> &path) noexcept
    {
        // building the graph
        if (src == dest)
        {
            path.push_back(src);
            return 0.0; // no path for sae src dest
        }
        // running the algorithm
        std::vector<double> Weights;
        Weights.resize(DVertices.size(), std::numeric_limits<double>::max());
        std::vector<TVertexID> Previous;
        Previous.resize(DVertices.size(), std::numeric_limits<TVertexID>::max());

        Weights[src] = 0; // since the weight between src and src is 0
        std::vector<std::shared_ptr<SVertex>> visited_set;

        int dest_index;
        while (true)
        {

            /*
            int i;
            auto p = min_element(Weights.begin(), Weights.end());
            i = p - Weights.begin();
            */

            int min_index = -1;
            for (int i = 0; i < VertexCount(); ++i)
            {
                if ((min_index == -1 && std::find(visited_set.begin(), visited_set.end(), DVertices[i]) == visited_set.end()) || (Weights[i] < Weights[min_index] && std::find(visited_set.begin(), visited_set.end(), DVertices[i]) == visited_set.end()))
                {
                    min_index = i;
                }
            }
            if (dest == min_index)
            {
                dest_index = min_index;
                break; // found the lowest cost path
            }
            else if (Weights[min_index] == std::numeric_limits<double>::max())
            {
                // break; // no path to dest exists
                return NoPathExists;
            }
            else if (min_index == -1)
            {
                return NoPathExists; // nothing left
            }

            for (int j = 0; j < DVertices[min_index]->DEdges.size(); ++j)
            {
                double new_weight = Weights[min_index] + DVertices[min_index]->DEdges[j].first;
                std::shared_ptr<SVertex> curr_vertex = DVertices[min_index]->DEdges[j].second;
                int vertex_index = -1;
                for (int k = 0; k < VertexCount(); ++k)
                {
                    if (DVertices[k] == curr_vertex)
                    {
                        vertex_index = k; // error if its not found
                        break;
                    }
                }
                if (vertex_index != -1)
                {
                    if (new_weight < Weights[vertex_index])
                    {
                        Weights[vertex_index] = new_weight;
                        Previous[vertex_index] = min_index;
                    }
                }
            }
            visited_set.push_back(DVertices[min_index]);
        }

        // path part
        // path.insert(path.begin(), dest_index);
        int curr_index = dest_index;
        while (curr_index != src)
        {
            path.insert(path.begin(), curr_index);
            curr_index = Previous[curr_index];
        }
        path.insert(path.begin(), src);
        // weight part

        return Weights[dest_index];
    }
};

CDijkstraPathRouter::CDijkstraPathRouter()
{
    DImplementation = std::make_unique<SImplementation>();
}

CDijkstraPathRouter::~CDijkstraPathRouter()
{
}

std::size_t CDijkstraPathRouter::VertexCount() const noexcept
{
    return DImplementation->VertexCount();
}

CPathRouter::TVertexID CDijkstraPathRouter::AddVertex(std::any tag) noexcept
{
    return DImplementation->AddVertex(tag);
}

std::any CDijkstraPathRouter::GetVertexTag(TVertexID id) const noexcept
{
    return DImplementation->GetVertexTag(id);
}

bool CDijkstraPathRouter::AddEdge(TVertexID src, TVertexID dest, double weight, bool bidir) noexcept
{
    return DImplementation->AddEdge(src, dest, weight, bidir);
}

bool CDijkstraPathRouter::Precompute(std::chrono::steady_clock::time_point deadline) noexcept
{
    return DImplementation->Precompute(deadline);
}

double CDijkstraPathRouter::FindShortestPath(TVertexID src, TVertexID dest, std::vector<TVertexID> &path) noexcept
{
    return DImplementation->FindShortestPath(src, dest, path);
}