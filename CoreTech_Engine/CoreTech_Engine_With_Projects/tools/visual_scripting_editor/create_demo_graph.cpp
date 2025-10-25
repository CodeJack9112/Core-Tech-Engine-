#include <iostream>
#include <fstream>
#include "../../include/VisualScripting.h"
using namespace VisualScripting;
int main(){
    Graph g; g.name = "DemoGraph";
    auto add = createAddNode(3.5f, 2.5f);
    auto print = createPrintNode("Result:");
    // link add result to print text
    Link l; l.fromNode = add->id; l.fromOutput = "Result"; l.toNode = print->id; l.toInput = "Text";
    g.nodes.push_back(add); g.nodes.push_back(print); g.links.push_back(l);
    std::string out = g.serialize();
    std::ofstream f("demo/visual_graph.json"); f << out; f.close();
    std::cout << "Wrote demo/visual_graph.json\n";
    return 0;
}
