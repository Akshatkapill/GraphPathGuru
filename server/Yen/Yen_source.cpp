#include <iostream>
#include <vector>
#include <utility>
#include <string>
#include <fstream>
#include <queue>
#include <algorithm>
#include <Windows.h>
#include <climits>

std::string epath;

using std::cout;
using std::endl;
using std::greater;
using std::pair;
using std::priority_queue;
using std::string;
using std::vector;

vector<int> dijkstra(int V, vector<vector<pair<int, int>>> &adj, int S, int D, std::string &output)
{
    // Create a priority queue for storing the nodes as a pair {dist,node}
    // where dist is the distance from source to the node.
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;

    // Initialising distTo list with a large number to
    // indicate the nodes are unvisited initially.
    // This list contains distance from source to the nodes.
    vector<int> distTo(V, INT_MAX);

    // Predecessor list is initialized to -1. A -1 in the final pred list means that that node is unreachable.
    vector<int> pred(V, -1);

    // Source initialised with dist=0.
    distTo[S] = 0;
    pq.push({0, S});

    // Now, pop the minimum distance node first from the min-heap
    // and traverse for all its adjacent nodes.
    while (!pq.empty())
    {
        auto copy = pq;
        output += "<ds>\n\t"; // begin <ds>
        while (copy.size())
        { // putting PQ to output
            output += std::to_string(copy.top().first) + "," + std::to_string(copy.top().second) + " ";
            copy.pop();
        }
        output += "\n\t";
        for (auto &d : distTo)
        { // Putting dist to output
            if (d == INT_MAX)
                output += "INF ";
            else
                output += std::to_string(d) + " ";
        }
        output += "\n\t";
        for (auto &p : pred)
        { // putting pred to output
            output += std::to_string(p) + " ";
        }
        output += "\n</ds>\n"; // <ds> stops here

        int node = pq.top().second;
        int dis = pq.top().first;
        pq.pop();

        output += "<adj>\n\t"; // Adj begins here
        output += std::to_string(node) + ", " + std::to_string(dis) + ":";
        // Check for all adjacent nodes of the popped out
        // element whether the prev dist is larger than current or not.
        for (auto it : adj[node])
        {
            int v = it.first;
            int w = it.second;
            output += "\n\t" + std::to_string(v) + ", " + std::to_string(w) + ", ";
            bool f = 0;
            if (dis + w < distTo[v])
            {
                distTo[v] = dis + w;
                // If current distance is smaller, push it into the queue.
                pq.push({dis + w, v});
                // Change the predecessor from the old node to the newer, more optimal node
                pred[v] = node;
                f = 1;
            }
            // This is for output gen logic
            if (f) // outputing different things depending on comparision result
                output += "1, " + std::to_string(pred[v]) + ", " + std::to_string(distTo[v]);
            else
                output += "0, -1, -1";
        }
        output += "\n</adj>\n"; // adj ends here
    }

    std::vector<int> path;
    for (int v = D; v != -1; v = pred[v])
    {
        path.push_back(v);
    }

    std::reverse(path.begin(), path.end());

    return (path[0] == S) ? path : std::vector<int>();
}

std::vector<std::vector<int>> Yen(int V, vector<vector<pair<int, int>>> &adj, int S, int D, std::string &output, int k)
{
    std::vector<std::vector<int>> kShortestPaths;

    // we are trying to find k shortest paths
    for (int i = 0; i < k; ++i)
    {
        // apply djikstra to find the shortest path between source and destination
        std::vector<int> path = dijkstra(V, adj, S, D, output);

        if (path.empty())
            break;

        kShortestPaths.push_back(path);

        // it is used to remove the edges that have been used in the path we got so that we can find a new path.
        output += "<path>";
        for (int j = 0; j < path.size() - 1; ++j)
        {
            int u = path[j];
            int v = path[j + 1];
            for (int idx = 0; idx < adj[u].size(); ++idx)
            {
                if (adj[u][idx].first == v)
                {
                    output += "\n\t" + std::to_string(u) + ", " + std::to_string(adj[u][idx].first) + ":";
                    adj[u].erase(adj[u].begin() + idx);
                    break;
                }
            }
        }
        output += "\n</path>\n";
    }
    return kShortestPaths;
}

vector<int> restore_path(int s, int t, vector<int> const &p)
{
    // This function makes out the path from any node to source, and then reverse it to be a path from source to the node.
    vector<int> path;

    for (int v = t; v != s; v = p[v])
    {
        path.push_back(v);
    }
    path.push_back(s);

    std::reverse(path.begin(), path.end());
    return path;
}

std::string goBackDir(std::string path, int levels)
{
    std::string res;
    int count = 0;
    for (int i = path.size() - 1; i >= 0; i--)
    {
        if (count == levels)
        {
            res = path.substr(0, i + 1);
        }
        if (path[i] == '\\')
            count++;
    }
    return res;
}

pair<int, vector<vector<pair<int, int>>>> make_graph()
{

    std::ifstream inputFile;
    std::string path = std::string(epath);
    inputFile.open(goBackDir(path, 1) + "\\file io\\input.txt");
    vector<std::string> lines;
    if (inputFile.is_open())
    {
        std::string line;
        while (std::getline(inputFile, line))
        {
            lines.push_back(line);
        }

        inputFile.close();
    }
    else
    {
        std::cerr << "Error: Unable to open the file." << std::endl;
    }

    pair<int, vector<vector<pair<int, int>>>> res;
    int V = lines.size();
    vector<vector<pair<int, int>>> adj(V, vector<pair<int, int>>());
    for (auto line : lines)
    {
        int pointer = 0;
        string temp;
        // extract the node value
        while (line[pointer] != ' ')
        {
            temp += line[pointer];
            pointer++;
        }

        int node = std::stoi(temp);
        temp = "";

        // skip past extra info
        while (pointer < line.size() && line[pointer] != ':')
            pointer++;

        // if the node has no neighbours, continue
        if (pointer >= line.size())
            continue;
        pointer += 2; // skip past the ":" and the " "

        int neighbour = 0, weight = 0;
        while (pointer < line.size())
        {
            // get num until ","
            while (pointer < line.size() && line[pointer] != ',')
            {
                temp += line[pointer];
                pointer++;
            }
            if (pointer >= line.size())
                continue;

            pointer++; // skip the ','
            neighbour = std::stoi(temp);
            temp = "";
            // get the number until ' '
            while (line[pointer] != ' ')
            {
                temp += line[pointer];
                pointer++;
            }
            weight = std::stoi(temp);
            temp = "";
            adj[node].push_back({neighbour, weight});
        }
    }
    res = {V, adj};
    return res;
}

void storeOutput(string output)
{

    std::string path = std::string(epath);
    std::ofstream outputFile(goBackDir(path, 1) + "\\file io\\output.txt");

    if (outputFile.is_open())
    {
        outputFile << output << std::endl
                   << std::endl;
        outputFile.close(); // Close the file
    }
    else
    {
        std::cerr << "Failed to open the output file." << std::endl;
    }

    return;
}

std::string getExecutablePath()
{
    char buffer[MAX_PATH];
    // @ts-ignore
    GetModuleFileName(NULL, buffer, MAX_PATH); // ignore this error it doesn't matter
    return std::string(buffer);
}

int main()
{
    epath = getExecutablePath();
    std::cout << "Current File: " << epath << std::endl;
    auto g = make_graph();
    int V = g.first, S = 0, D = V - 1;
    int t = 0;
    auto adj = g.second;
    std::string output;

    int k = 2;
    vector<vector<int>> kShortestPaths = Yen(V, adj, S, D, output, k);

    output += "<result>";
    cout << "K shortest paths are :";
    for (const auto &path : kShortestPaths)
    {
        output += "\n\t";
        for (int vertex : path)
        {
            output += std::to_string(vertex) + " ";
            cout << vertex << " ";
        }
        cout << endl;
    }

    output += "\n</result>\n";

    // Storing the result for front end to read
    storeOutput(output);

    return 0;
}
