/**
 * File Header
 * Author: Anthony Yang
 * Email: a5yang@ucsd.edu
 * Sources: None
 * Description: This file contains the implementation for pathfinder, which
 * finds the shortest path between two actors
 */

#include <fstream>
#include <iostream>
#include <queue>
#include <sstream>

#include "ActorGraph.hpp"

// argv positions
#define MOVIE_FILE 1
#define UW 2
#define ACTOR_FILE 3
#define OUTPUT_FILE 4

// unweighted vs. weighted flags
#define UNWEIGHTED 'u'
#define WEIGHTED 'w'

// Delimiters in outputing paths
#define ACT_MOV "--"
#define MOV_ACT "-->"
#define LEFT_BRACE "("
#define RIGHT_BRACE ")"
#define LEFT_MOV_BRACE "["
#define RIGHT_MOV_BRACE "]"
#define NAME_YEAR "#@"

using namespace std;

void clearEdges(vector<Edge*> edges);

/**
 * Finds closest weighted path between actor1 and actor2
 * Params:
 *  - graph: graphs of all actors
 *  - actor1: the actor the path starts from
 *  - actor2: the actor the path ends at
 *  Returns: a vector of edges that traces the path from actor1 to actor2
 */
vector<Edge*> findWeightedPath(ActorGraph& graph, string actor1,
                               string actor2) {
    Node* start = graph.getActorNode(actor1);
    Node* end = graph.getActorNode(actor2);
    if (!start || !end) {
        // if start or end not found
        return {};
    }
    priority_queue<pair<Node*, int>, vector<pair<Node*, int>>, NodePairComp>
        djikstra;
    unordered_map<Node*, int> changed;
    start->setStart();
    pair<Node*, int> startPair = {start, start->getPathWeight()};
    djikstra.push(startPair);
    changed[start] = 1;
    while (!djikstra.empty()) {
        pair<Node*, int> currentPair = djikstra.top();
        Node* current = currentPair.first;
        djikstra.pop();
        if (current->isDone()) {
            // current has already been checked
            continue;
        }
        current->check();
        vector<Edge*> neighbors = current->getEdges();
        for (int edge = 0; edge < neighbors.size(); edge++) {
            if (neighbors[edge]->getDest()->isDone()) {
                // Node already checked, can't be shortest distance
                continue;
            }
            if (neighbors[edge]->getDest()->getPathWeight() >
                current->getPathWeight() + neighbors[edge]->getWeight()) {
                // Found a shorter path to the node
                Node* neighbor = neighbors[edge]->getDest();
                neighbor->setPrev(neighbors[edge], current->getPathWeight());
                changed[neighbor] = 1;  // Adds to changed list so Node can be
                                        // reset when path found
                if (neighbor == end) {
                    // Reached the destination
                    vector<Edge*> path;
                    while (neighbor != start) {
                        path.insert(path.begin(), neighbor->getPrev());
                        neighbor = neighbor->getPrev()->getSource();
                    }
                    for (auto iter = changed.begin(); iter != changed.end();
                         iter++) {
                        // Resets all nodes
                        iter->first->uncheck();
                    }
                    return path;
                }
                pair<Node*, int> neighborPair = {neighbor,
                                                 neighbor->getPathWeight()};
                djikstra.push(neighborPair);  // Adds Node to priority queue
            }
        }
    }
    // No path found
    return {};
}

/**
 * Finds the closest unweighted path between actor1 and actor2
 * Params:
 * - graph: the graph of all actors
 * - actor1: the name of first actor
 * - actor2: the name of second actor
 * Returns: a vector of edges representing the path from actor1 to actor2
 */
vector<Edge*> findUnweightedPath(ActorGraph& graph, string actor1,
                                 string actor2) {
    Node* start = graph.getActorNode(actor1);
    Node* end = graph.getActorNode(actor2);
    if (!start || !end) {
        // if start or end not found
        return {};
    }
    /*if (start->getName() == "Bruce Dern") {
        cout << "begin" << endl;
    }*/
    // cout << (start == nullptr);
    // cout << (end == nullptr);
    queue<Node*> bfs;
    bfs.push(start);
    /*cout << endl;
    cout << endl;
    cout << endl;
    cout << endl;
    cout << endl;
    cout << endl;
    cout << "actor1: " << start->getName() << endl;
    cout << "actor2: " << end->getName() << endl;*/

    // vector<Edge*> checked;
    unordered_map<Node*, int> finished;

    while (!bfs.empty()) {
        // Runs BFS to find nearest path using a queue
        Node* current = bfs.front();
        // cout << "current: " << current->getName();
        /*if (current->getPrev() != nullptr) {
            cout << " :: " << current->getPrev()->getName().name << endl;
        } else {
            cout << " previous is null" << endl;
        }*/

        bfs.pop();

        // Marks node as done
        current->check();
        finished[current] = 1;
        // cout << "popped: " << current->getName() << endl;
        if (current == end) {
            // clearEdges(
            //   checked);  // clears all edges so they are reset to unchecked
            // cout << "reached end"
            vector<Edge*> path;
            while (current != start) {
                path.insert(path.begin(), current->getPrev());
                // cout << "current: " << current->getName() << endl;
                current = current->getPrev()->getSource();
            }
            // clears all Nodes when path found
            for (auto iter = finished.begin(); iter != finished.end(); iter++) {
                iter->first->uncheck();
            }
            return path;
        }

        vector<Edge*> neighborEdges = current->getEdges();

        for (int edge = 0; edge < neighborEdges.size(); edge++) {
            // Pushes neighbors onto queue
            Node* neighbor = neighborEdges[edge]->getDest();
            finished[neighbor] = 1;  // adds Node to changed list so it can be
                                     // reset when path found
            /*if (neighbor == current) {
                // Skips directed edges pointed back to itself from neighbor
                neighborEdges[edge]->check();
                checked.push_back(
                    neighborEdges[edge]);  // Store these in array so they can
                                           // be reset after path found
                continue;
            }
            if (neighborEdges[edge]->isChecked()) {
                // Edge will result in a cycle back to a checked node
                continue;
            }*/
            if (neighbor->isDone()) {
                continue;
            }

            /*if (neighbor->getName() == "Kevin Costner") {
                cout << "here" << endl;
            }

            if (neighbor->getName() == "Scott Glenn") {
                cout << "here2" << endl;
            }*/
            /*if (start->getName() == "Djimon Hounsou" &&
                neighbor->getName() == "Suet Lam") {
                cout << "wth, Islan" << endl;
            }*/
            if (neighbor->getPrev() == nullptr) {
                // only changes previous once
                /*if (neighbor == end) {
                    cout << "if statement true" << endl;
                }*/
                neighbor->setPrev(neighborEdges[edge]);
                /*if (neighbor == end) {
                    cout << neighborEdges[edge]->getName().name << endl;
                    cout << neighbor->getPrev()->getName().name << endl;
                }*/
            }
            if (neighbor == end) {
                vector<Edge*> path;
                while (neighbor != start) {
                    path.insert(path.begin(), neighbor->getPrev());
                    // cout << "current: " << current->getName() << endl;
                    /*if (neighbor->getPrev() == nullptr) {
                        cout << "Is null at: " << neighbor->getName() << endl;
                        if (neighborEdges[edge] == nullptr) {
                            cout << "Neighbor's edge is null" << endl;
                        } else {
                            cout << "Neighbor's edge is not null" << endl;
                            if (neighbor->getPrev() == nullptr) {
                                cout << "what the hell" << endl;
                            }
                        }
                    }*/
                    /*if (start->getName() == "Djimon Hounsou" &&
                        neighbor->getName() == "Suet Lam") {
                        cout << "null at " << neighbor->getName() << endl;
                    }*/
                    neighbor = neighbor->getPrev()->getSource();
                }
                // clears all Nodes when path found
                for (auto iter = finished.begin(); iter != finished.end();
                     iter++) {
                    iter->first->uncheck();
                }
                return path;
            }
            bfs.push(neighbor);  // updates neighbor's path
            /*if (current->getName() == "Kevin Kline") {
                cout << neighbor->getName() << " is enqueued" << endl;
            }*/
        }
    }
    // clearEdges(checked);
    // clears all Nodes when path found
    for (auto iter = finished.begin(); iter != finished.end(); iter++) {
        iter->first->uncheck();
    }
    return {};  // Path not found
}

/**
 * Resets all edges so that they are no longer checked; called after path is
 * found
 * Params:
 * - edges: the vector containing all edges that need to be reset
 * Returns: void
 */
void clearEdges(vector<Edge*> edges) {
    for (int edge = 0; edge < edges.size(); edge++) {
        edges[edge]->unCheck();
        edges[edge]->getSource()->uncheck();
    }
}

/**
 * Parses test file to extract actor pairs within it
 * Params:
 * - testfile: name of testfile
 * Returns: a vector of pairs containing actor pairs
 */
vector<pair<string, string>> parsePathFile(string testFile) {
    // Initialize the file stream
    ifstream infile(testFile);
    vector<pair<string, string>> actorList;
    bool have_header = false;

    while (!infile.eof()) {
        string line;

        // Gets the next line in file
        if (!getline(infile, line)) {
            break;
        }

        // cout << "Reading: " << line << endl;

        if (!have_header) {
            // skip the header
            have_header = true;
            continue;
        }

        stringstream ss(line);
        pair<string, string> actors;
        int pos = 1;

        while (ss) {
            string str;

            // get the next string before hitting a tab character and put it in
            // 'str'
            if (!getline(ss, str, '\t')) {
                break;
            }
            if (pos == 1) {  // first actor
                actors.first = str;
                // cout << "Reading1: " << str << " ";
            } else {  // second actor
                actors.second = str;
                // cout << "Reading2: " << str << endl;
            }
            pos++;
        }
        if (actors.first == "") {
            continue;
        }
        actorList.push_back(actors);
        // cout << "Reading1: " << actors.first << " ";
        // cout << "Reading2: " << actors.second << endl;
    }
    infile.close();
    return actorList;
}

/**
 * Runs pathfinder to find the shortest path between two actors
 * Params:
 *  - argc: the number of command line arguments (should be 4)
 *  - argv: contents of the command line arguments: should be movie file name
 *          u or w for weighted/unweighted, file name of file containing actor
 *          pairs, and output file name Returns: 0 if success, otherwise 1
 */
int main(int argc, char* argv[]) {
    ActorGraph graph;
    if (argv[UW][0] == UNWEIGHTED) {
        graph.loadFromFile(argv[MOVIE_FILE], false);
    } else if (argv[UW][0] == WEIGHTED) {
        graph.loadFromFile(argv[MOVIE_FILE], true);
    }
    vector<pair<string, string>> actors = parsePathFile(argv[ACTOR_FILE]);
    // cout << "made it past parsing" << endl;
    ofstream output;
    output.open(argv[OUTPUT_FILE]);
    output << "(actor)--[movie#@year]]-->(actor)--..." << endl;
    // cout << actors.size() << endl;
    for (int test = 0; test < actors.size(); test++) {
        // Runs pathfinder on all actor pairs in file
        // cout << actors[test].first << " " << actors[test].second << endl;
        // cout << "actors " << actors[test].first << " " << actors[test].second
        //     << endl;
        vector<Edge*> path;
        if (argv[UW][0] == UNWEIGHTED) {
            path = findUnweightedPath(graph, actors[test].first,
                                      actors[test].second);
        } else if (argv[UW][0] == WEIGHTED) {
            path = findWeightedPath(graph, actors[test].first,
                                    actors[test].second);
        }
        if (path.size() == 0) {
            output << endl;
            continue;
        }
        for (int edge = 0; edge < path.size(); edge++) {
            if (edge == 0) {  // If printing first actor in path
                output << LEFT_BRACE << path[edge]->getSource()->getName()
                       << RIGHT_BRACE;  // prints first actor
            }
            output << ACT_MOV;  // prints first delimiter
            output << LEFT_MOV_BRACE << path[edge]->getName().name << NAME_YEAR
                   << path[edge]->getName().year
                   << RIGHT_MOV_BRACE;  // prints movie and year
            output << MOV_ACT;          // prints second delimiter
            output << LEFT_BRACE << path[edge]->getDest()->getName()
                   << RIGHT_BRACE;  // prints second actor
        }
        output << endl;  // prints new line
    }
    output.close();
}
