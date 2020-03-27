#include <iostream>

#include "args.hpp"

// https://github.com/Taywee/args
// ./arguments/run -i 5 -c f

int main(int argc, char **argv)
{
    args::ArgumentParser parser("DREVO Ray Tracer", "This goes after the options.");
    args::HelpFlag help(parser, "help", "Display this help menu", {"help"});
    
    args::ValueFlag<int> width(parser, "width", "Height of our image", {'w'});
    args::ValueFlag<int> height(parser, "height", "Width of our image", {'h'});
    args::ValueFlag<int> depth(parser, "depth", "How many bounces to allow for each ray of light", {'d'});
    args::ValueFlag<int> sampling(parser, "sampling", "Number of rays to sample per pixel, for antialiasing purposes", {'s'});
    args::ValueFlag<std::string> output(parser, "output", "Output PPM filepath", {'o'});

    try {
        parser.ParseCLI(argc, argv);
    } catch (args::Help) {
        std::cout << parser;
        return 0;
    } catch (args::ParseError e) {
        std::cerr << e.what() << std::endl;
        std::cerr << parser;
        return 1;
    } catch (args::ValidationError e) {
        std::cerr << e.what() << std::endl;
        std::cerr << parser;
        return 1;
    }

    if (width) { std::cout << "width: " << args::get(width) << std::endl; }
    if (height) { std::cout << "height: " << args::get(height) << std::endl; }
    if (depth) { std::cout << "depth: " << args::get(depth) << std::endl; }
    if (sampling) { std::cout << "sampling: " << args::get(sampling) << std::endl; }
    if (output) { std::cout << "output: " << args::get(output) << std::endl; }
    
    return 0;
}