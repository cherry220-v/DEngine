#pragma once

#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <stdexcept>

#include "passes/IRenderPass.hpp"
#include "RenderGraphStorage.hpp"

class RenderGraph
{
public:
    void addPass(std::unique_ptr<IRenderPass> pass)
    {
        passes.push_back(std::move(pass));
    }

    void init(IRenderPlatform* platform, IRenderDevice* device)
    {
        for (auto& p : passes)
            p->init(platform, device);

        buildGraph();
        sortPasses();
    }

    void build()
    {
        for (auto& p : passes)
            p->setup(storage);

        buildGraph();
        sortPasses();
    }

    void execute(const RenderContext& ctx)
    {
        for (auto* p : sorted)
            p->execute(ctx, storage);
    }

private:
    void buildGraph()
    {
        adjacency.clear();
        indegree.clear();
        passToIndex.clear();

        for (size_t i = 0; i < passes.size(); i++)
        {
            indegree[i] = 0;
            adjacency[i] = {};
            passToIndex[passes[i].get()] = i;
        }

        for (size_t i = 0; i < passes.size(); i++)
        {
            auto& a = passes[i];

            for (size_t j = 0; j < passes.size(); j++)
            {
                if (i == j) continue;

                auto& b = passes[j];

                if (dependsOnResource(a.get(), b.get()))
                    addEdge(j, i);
            }

            for (auto* dependency : a->afterPasses)
            {
                auto dependencyIt = passToIndex.find(dependency);
                if (dependencyIt != passToIndex.end())
                    addEdge(dependencyIt->second, i);
            }
        }
    }

    bool dependsOnResource(IRenderPass* reader, IRenderPass* writer)
    {
        for (auto read : reader->reads)
        {
            for (auto write : writer->writes)
            {
                if (read == write)
                    return true;
            }
        }
        return false;
    }

    void addEdge(size_t before, size_t after)
    {
        auto& edges = adjacency[before];
        for (auto edge : edges)
        {
            if (edge == after)
                return;
        }

        edges.push_back(after);
        indegree[after]++;
    }

    void sortPasses()
    {
        sorted.clear();

        std::queue<size_t> q;
        auto indegreeCopy = indegree;

        for (size_t i = 0; i < passes.size(); i++)
        {
            if (indegreeCopy[i] == 0)
                q.push(i);
        }

        while (!q.empty())
        {
            size_t n = q.front();
            q.pop();

            sorted.push_back(passes[n].get());

            for (auto next : adjacency[n])
            {
                if (--indegreeCopy[next] == 0)
                    q.push(next);
            }
        }

        if (sorted.size() != passes.size())
            throw std::runtime_error("RenderGraph: cycle detected!");
    }

private:
    RenderGraphStorage storage;

    std::vector<std::unique_ptr<IRenderPass>> passes;
    std::vector<IRenderPass*> sorted;

    std::unordered_map<IRenderPass*, size_t> passToIndex;
    std::unordered_map<size_t, std::vector<size_t>> adjacency;
    std::unordered_map<size_t, int> indegree;
};
