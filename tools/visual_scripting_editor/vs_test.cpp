#include <iostream>
#include <fstream>
#include "../include/VisualScripting.h"
int main(){
    std::ifstream f("demo/visual_graph.json");
    if(!f){ std::cout<<"No demo graph for tests. Run vs_editor to create one.\n"; return 0; }
    std::string s((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
    auto g = VisualScripting::Graph::deserialize(s);
    VisualScripting::ExecutionContext ctx; ctx.log = [](const std::string &m){ std::cout<<"[VS TEST] "<<m<<std::endl; };
    VisualScripting::executeGraph(g, ctx);
    return 0;
}
