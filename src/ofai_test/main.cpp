#include <iostream>

#include <ofai/structure_generator.h>

using namespace ofai;

int main()
{
    // Initialize the generator with your API key
    StructureGenerator generator("your-api-key-here");

    // Generate ChaiScript code for a structure
    std::string chaiScript = generator.generateStructure(
        "Create a building structure 10m × 10m with 4m wall height and an angled roof peaking at 6m. "
        "Add stabilization with single diagonals on each wall and direct roof supports.");

    // Print or use the generated code
    std::cout << chaiScript << std::endl;

    return 0;
}
