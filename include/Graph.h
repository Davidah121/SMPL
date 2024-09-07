#pragma once
#include "BuildOptions.h"
#include <iostream>
#include <vector>
#include <unordered_map>
#include <functional>

namespace smpl
{
    struct PathInfo
    {
        std::vector<size_t> path;
        double cost;
    };

    class DLL_OPTION UndirectedMatrixGraph
    {
    public:
        UndirectedMatrixGraph(size_t numberOfVerticies);
        ~UndirectedMatrixGraph();

        std::vector<double>& getEdges(size_t vertex);
        double getEdgeWeight(size_t mainVertex, size_t connectedVertex);
        void addVertex();
        void addEdge(size_t mainVertex, size_t connectedVertex, double weight);
        void removeEdge(size_t mainVertex, size_t connectedVertex);

        PathInfo shortestPath(size_t startVertex, size_t endVertex, std::function<double(size_t, size_t)> heuristic);
    private:
        std::vector<std::vector<double>> matrix;
    };

    class DLL_OPTION UndirectedListGraph
    {
    public:
        UndirectedListGraph(size_t numberOfVerticies);
        ~UndirectedListGraph();

        std::unordered_map<size_t, double>& getEdges(size_t vertex);
        double getEdgeWeight(size_t mainVertex, size_t connectedVertex);
        void addVertex();
        void addEdge(size_t mainVertex, size_t connectedVertex, double weight);
        void removeEdge(size_t mainVertex, size_t connectedVertex);

        PathInfo shortestPath(size_t startVertex, size_t endVertex, std::function<double(size_t, size_t)> heuristic);
    private:
        std::vector<std::unordered_map<size_t, double>> adjList;
    };
}