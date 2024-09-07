#include "Graph.h"
#include "PriorityQueue.h"
#include "StringTools.h"

namespace smpl
{
    UndirectedMatrixGraph::UndirectedMatrixGraph(size_t numberOfVerticies)
    {
        matrix = std::vector<std::vector<double>>(numberOfVerticies);
        for(size_t i=0; i<numberOfVerticies; i++)
        {
            matrix[i] = std::vector<double>(numberOfVerticies);
        }
    }
    UndirectedMatrixGraph::~UndirectedMatrixGraph()
    {

    }

    std::vector<double>& UndirectedMatrixGraph::getEdges(size_t vertex)
    {
        return matrix[vertex];
    }
    double UndirectedMatrixGraph::getEdgeWeight(size_t mainVertex, size_t connectedVertex)
    {
        return matrix[mainVertex][connectedVertex];
    }
    void UndirectedMatrixGraph::addVertex()
    {
        matrix.push_back(std::vector<double>(matrix.size()+1));
        for(size_t i=0; i<matrix.size()-1; i++)
        {
            matrix[i].push_back(0);
        }
    }
    void UndirectedMatrixGraph::addEdge(size_t mainVertex, size_t connectedVertex, double weight)
    {
        matrix[mainVertex][connectedVertex] = weight;
    }
    void UndirectedMatrixGraph::removeEdge(size_t mainVertex, size_t connectedVertex)
    {
        matrix[mainVertex][connectedVertex] = 0;
    }

    PathInfo UndirectedMatrixGraph::shortestPath(size_t startVertex, size_t endVertex, std::function<double(size_t,size_t)> heuristic)
    {
        //A*. If no heuristic, dijkstra's algorithm (A* with a heuristic of the same value results in dijkstra's)
        //Need the current vertex and a list of the verticies up to this point (can be put into a single list) along with its cost

        //assumes their exists a path to the end vertex.
        
        PathInfo bestPath;
        if(startVertex == endVertex)
            return bestPath;
        
        if(startVertex >= matrix.size() || endVertex >= matrix.size())
            return bestPath;

        PriorityQueue<PathInfo> pq = PriorityQueue<PathInfo>(PriorityQueue<PathInfo>::TYPE_MIN);
        for(size_t i=0; i<matrix[startVertex].size(); i++)
        {
            double weight = getEdgeWeight(startVertex, i);
            if(i != startVertex && weight != 0)
            {
                PathInfo p;
                p.cost = weight;
                p.path = {startVertex, i};
                double guess = 0;
                if(heuristic != nullptr)
                    guess = heuristic(i, endVertex);

                double priority = p.cost+guess;
                pq.add(priority, p);
            }
        }

        while(!pq.empty())
        {
            //grab path off of the list.
            PathInfo currentPathInfo;
            
            if(pq.get(currentPathInfo))
            {
                //exit early clause. If best path is filled out and its cost is less than what we just got, exit early.
                if(bestPath.path.size() > 0 && bestPath.cost < currentPathInfo.cost)
                {
                    break; //Done. Nothing else we grab can ever be lower cost
                }

                //for each vertex connected to this path that hasn't been included yet, add that as a new potential path.
                size_t currentVertex = currentPathInfo.path.back();
                for(size_t i=0; i<matrix[currentVertex].size(); i++)
                {
                    if(currentVertex == i)
                        continue;
                    
                    double weight = getEdgeWeight(currentVertex, i);
                    if(weight > 0)
                    {
                        //check if this vertex is in the path
                        bool notInPath = true;
                        for(size_t j=0; j<currentPathInfo.path.size(); j++)
                        {
                            if(i == currentPathInfo.path[j])
                            {
                                notInPath = false;
                                break;
                            }
                        }

                        if(notInPath)
                        {
                            //add as a new potential path
                            PathInfo newPathInfo = currentPathInfo;
                            newPathInfo.path.push_back(i);
                            newPathInfo.cost += weight;

                            if(i == endVertex)
                            {
                                //potential answer. No need to add to the priority queue but we do need to maintain this for later
                                if(bestPath.path.size() == 0)
                                    bestPath = newPathInfo;
                                else if(bestPath.cost > newPathInfo.cost)
                                    bestPath = newPathInfo;
                            }
                            else
                            {
                                double guess = 0;
                                if(heuristic != nullptr)
                                    guess = heuristic(i, endVertex);
                                double priority = newPathInfo.cost + guess;
                                pq.add(priority, newPathInfo);
                            }
                        }
                    }
                }
            }
            else
            {
                //problem.
                break;
            }
        }
        return bestPath;
    }


    UndirectedListGraph::UndirectedListGraph(size_t numberOfVerticies)
    {
        adjList = std::vector<std::unordered_map<size_t, double>>(numberOfVerticies);
    }
    UndirectedListGraph::~UndirectedListGraph()
    {
        
    }

    std::unordered_map<size_t, double>& UndirectedListGraph::getEdges(size_t vertex)
    {
        return adjList[vertex];
    }
    double UndirectedListGraph::getEdgeWeight(size_t mainVertex, size_t connectedVertex)
    {
        auto it = adjList[mainVertex].find(connectedVertex);
        if(it != adjList[mainVertex].end())
            return it->second;
        return 0;
    }
    void UndirectedListGraph::addVertex()
    {
        adjList.push_back(std::unordered_map<size_t, double>());
    }
    void UndirectedListGraph::addEdge(size_t mainVertex, size_t connectedVertex, double weight)
    {
        adjList[mainVertex][connectedVertex] = weight;
    }
    void UndirectedListGraph::removeEdge(size_t mainVertex, size_t connectedVertex)
    {
        adjList[mainVertex].erase(connectedVertex);
    }

    PathInfo UndirectedListGraph::shortestPath(size_t startVertex, size_t endVertex, std::function<double(size_t,size_t)> heuristic)
    {
        //A*. If no heuristic, dijkstra's algorithm (A* with a heuristic of the same value results in dijkstra's)
        //Need the current vertex and a list of the verticies up to this point (can be put into a single list) along with its cost

        //assumes their exists a path to the end vertex.
        
        PathInfo bestPath;
        if(startVertex == endVertex)
            return bestPath;
        
        if(startVertex >= adjList.size() || endVertex >= adjList.size())
            return bestPath;

        PriorityQueue<PathInfo> pq = PriorityQueue<PathInfo>(PriorityQueue<PathInfo>::TYPE_MIN);
        for(size_t i=0; i<adjList[startVertex].size(); i++)
        {
            double weight = getEdgeWeight(startVertex, i);
            if(i != startVertex && weight != 0)
            {
                PathInfo p;
                p.cost = weight;
                p.path = {startVertex, i};
                double guess = 0;
                if(heuristic != nullptr)
                    guess = heuristic(i, endVertex);

                double priority = p.cost+guess;
                pq.add(priority, p);
            }
        }

        while(!pq.empty())
        {
            //grab path off of the list.
            PathInfo currentPathInfo;
            
            if(pq.get(currentPathInfo))
            {
                //exit early clause. If best path is filled out and its cost is less than what we just got, exit early.
                if(bestPath.path.size() > 0 && bestPath.cost < currentPathInfo.cost)
                {
                    break; //Done. Nothing else we grab can ever be lower cost
                }

                //for each vertex connected to this path that hasn't been included yet, add that as a new potential path.
                size_t currentVertex = currentPathInfo.path.back();
                for(size_t i=0; i<adjList[currentVertex].size(); i++)
                {
                    if(i == currentVertex)
                        continue;
                    
                    double weight = getEdgeWeight(currentVertex, i);
                    if(weight)
                    {
                        //check if this vertex is in the path
                        bool notInPath = true;
                        for(size_t j=0; j<currentPathInfo.path.size(); j++)
                        {
                            if(i == currentPathInfo.path[j])
                            {
                                notInPath = false;
                                break;
                            }
                        }

                        if(notInPath)
                        {
                            //add as a new potential path
                            PathInfo newPathInfo = currentPathInfo;
                            newPathInfo.path.push_back(i);
                            newPathInfo.cost += weight;

                            if(i == endVertex)
                            {
                                //potential answer. No need to add to the priority queue but we do need to maintain this for later
                                if(bestPath.path.size() == 0)
                                    bestPath = newPathInfo;
                                else if(bestPath.cost > newPathInfo.cost)
                                    bestPath = newPathInfo;
                            }
                            else
                            {
                                double guess = 0;
                                if(heuristic != nullptr)
                                    guess = heuristic(i, endVertex);
                                double priority = newPathInfo.cost + guess;
                                pq.add(priority, newPathInfo);
                            }
                        }
                    }
                }
            }
            else
            {
                //problem.
                break;
            }
        }
        return bestPath;
    }
}