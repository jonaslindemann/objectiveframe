#include <ofai/structure_generator.h>

#include <sstream>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <thread>
#include <ofutil/util_functions.h>

using json = nlohmann::json;

using namespace ofai;

// ============================================================================
// PromptDatabase Implementation
// ============================================================================

void PromptDatabase::parseCategories()
{
    m_categories.clear();
    
    if (!m_data.contains("categories") || !m_data["categories"].is_array())
    {
        return;
    }
    
    for (const auto& catJson : m_data["categories"])
    {
        PromptCategory category;
        category.id = catJson.value("id", "");
        category.name = catJson.value("name", "");
        category.description = catJson.value("description", "");
        
        if (catJson.contains("examples") && catJson["examples"].is_array())
        {
            for (const auto& exJson : catJson["examples"])
            {
                PromptExample example;
                example.id = exJson.value("id", "");
                example.name = exJson.value("name", "");
                example.prompt = exJson.value("prompt", "");
                example.difficulty = exJson.value("difficulty", "");
                
                if (exJson.contains("tags") && exJson["tags"].is_array())
                {
                    for (const auto& tag : exJson["tags"])
                    {
                        if (tag.is_string())
                        {
                            example.tags.push_back(tag.get<std::string>());
                        }
                    }
                }
                
                category.examples.push_back(example);
            }
        }
        
        m_categories.push_back(category);
    }
}

bool PromptDatabase::loadFromFile(const std::string& filename)
{
    try
    {
        std::ifstream file(filename);
        if (!file.is_open())
        {
            std::cerr << "Failed to open file: " << filename << std::endl;
            return false;
        }
        
        file >> m_data;
        file.close();
        
        m_version = m_data.value("version", "");
        m_description = m_data.value("description", "");
        
        parseCategories();
        
        return true;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error loading prompt database: " << e.what() << std::endl;
        return false;
    }
}

bool PromptDatabase::loadFromString(const std::string& jsonString)
{
    try
    {
        m_data = json::parse(jsonString);
        
        m_version = m_data.value("version", "");
        m_description = m_data.value("description", "");
        
        parseCategories();
        
        return true;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error parsing JSON string: " << e.what() << std::endl;
        return false;
    }
}

std::vector<PromptCategory> PromptDatabase::getCategories() const
{
    return m_categories;
}

PromptCategory PromptDatabase::getCategoryById(const std::string& categoryId) const
{
    for (const auto& category : m_categories)
    {
        if (category.id == categoryId)
        {
            return category;
        }
    }
    return PromptCategory(); // Return empty category if not found
}

PromptExample PromptDatabase::getExampleById(const std::string& exampleId) const
{
    for (const auto& category : m_categories)
    {
        for (const auto& example : category.examples)
        {
            if (example.id == exampleId)
            {
                return example;
            }
        }
    }
    return PromptExample(); // Return empty example if not found
}

std::vector<PromptExample> PromptDatabase::getExamplesByCategory(const std::string& categoryId) const
{
    PromptCategory category = getCategoryById(categoryId);
    return category.examples;
}

std::vector<PromptExample> PromptDatabase::getExamplesByTag(const std::string& tag) const
{
    std::vector<PromptExample> results;
    
    for (const auto& category : m_categories)
    {
        for (const auto& example : category.examples)
        {
            for (const auto& exampleTag : example.tags)
            {
                if (exampleTag == tag)
                {
                    results.push_back(example);
                    break; // Don't add the same example multiple times
                }
            }
        }
    }
    
    return results;
}

std::vector<PromptExample> PromptDatabase::getExamplesByDifficulty(const std::string& difficulty) const
{
    std::vector<PromptExample> results;
    
    for (const auto& category : m_categories)
    {
        for (const auto& example : category.examples)
        {
            if (example.difficulty == difficulty)
            {
                results.push_back(example);
            }
        }
    }
    
    return results;
}

std::vector<PromptExample> PromptDatabase::searchExamplesByName(const std::string& searchTerm) const
{
    std::vector<PromptExample> results;
    
    // Convert search term to lowercase for case-insensitive search
    std::string lowerSearchTerm = searchTerm;
    std::transform(lowerSearchTerm.begin(), lowerSearchTerm.end(), lowerSearchTerm.begin(), ::tolower);
    
    for (const auto& category : m_categories)
    {
        for (const auto& example : category.examples)
        {
            // Convert example name to lowercase
            std::string lowerName = example.name;
            std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
            
            // Check if search term is found in the name
            if (lowerName.find(lowerSearchTerm) != std::string::npos)
            {
                results.push_back(example);
            }
        }
    }
    
    return results;
}

std::vector<PromptExample> PromptDatabase::getAllExamples() const
{
    std::vector<PromptExample> allExamples;
    
    for (const auto& category : m_categories)
    {
        for (const auto& example : category.examples)
        {
            allExamples.push_back(example);
        }
    }
    
    return allExamples;
}

// ============================================================================
// StructureGenerator Implementation
// ============================================================================

// Constructor
StructureGenerator::StructureGenerator(const std::string &apiKey)
    : m_apiKey(apiKey), m_apiUrl("https://api.anthropic.com/v1/messages"), m_model("claude-sonnet-4-5"),
      m_systemPrompt(buildSystemPrompt())
{
    curl_global_init(CURL_GLOBAL_DEFAULT);
}

// Destructor
StructureGenerator::~StructureGenerator()
{
    curl_global_cleanup();
}

void ofai::StructureGenerator::setApiKey(const std::string &apiKey)
{
    this->m_apiKey = apiKey;
}

// Static callback function for cURL to store API response
size_t StructureGenerator::WriteCallback(void *contents, size_t size, size_t nmemb, std::string *output)
{
    size_t totalSize = size * nmemb;
    output->append((char *)contents, totalSize);
    return totalSize;
}

// Build the system prompt with preconditions for optimization
// 2. OPTIMIZATION PRINCIPLES :
//    - Use diagonal bracing strategically, no more than one per wall face
// 3. STRUCTURE GENERATION:
//    - Generate nodes only at essential structural points
//    - Connect beams in minimal paths that ensure structural integrity
//    - Use functions to create repeating patterns rather than hardcoding elements
//    - Aim for the absolute minimum number of elements while maintaining stability

std::string StructureGenerator::buildSystemPrompt() const
{
    return R"(
# UNIVERSAL STRUCTURAL ENGINEERING ASSISTANT

You are a structural engineering assistant specialized in generating optimized ChaiScript code for 3D beam/bar structures.
Your task is to convert natural language descriptions into optimized ChaiScript code for any type of structure.

## MANDATORY CODE TEMPLATE (ALWAYS START WITH THIS)
```
// Create new model
newModel();

// Define constants
global PI = 3.14159265358979323846;

// Structure parameters
var width = 0.0;   // Width along X-axis
var depth = 0.0;   // Depth/span along Z-axis
var height = 0.0;  // Height along Y-axis

// Main implementation below
```

## UNIVERSAL COORDINATE SYSTEM (ALWAYS FOLLOW)
- X-axis: Goes from left (-) to right (+)
- Y-axis: Goes from bottom (-) to top (+) [VERTICAL HEIGHT]
- Z-axis: Goes from front (-) to back (+) [DEPTH]
- ORIGIN (0,0,0): Center of structure base
- ALL dimensions in meters
- GROUND SUPPORTS: Members in the XZ plane (y=0) are automatically supported

## STRUCTURE ORIENTATION STANDARDS
- BRIDGES: Span along Z-axis, width along X-axis
- BUILDINGS: Width along X-axis, depth along Z-axis
- ARCHES: Opening along Z-axis, height along Y-axis
- DOMES/SPHERICAL: Centered at origin, peak along Y-axis
- TOWERS: Vertical along Y-axis, base centered at origin

## CRITICAL ERROR PREVENTION
1. ALWAYS define PI globally: global PI = 3.14159265358979323846;
2. ALWAYS start with newModel();
3. NEVER use methods that don't exist in ChaiScript:
   - NO slice() - implement your own getSubset function
   - NO concat() - use loops to combine arrays
   - NO Math.XXX functions - use built-in min, max, etc.
4. ALWAYS initialize all variables before use
5. Use explicit floating point: (1.0 * value) or value.to_double()
6. Properly initialize out parameters for nodePosAt: var x = 0.0; var y = 0.0; var z = 0.0;
7. NEVER add support constraints - ground support is assumed for XZ plane

## CHAISCRIPT-SPECIFIC IMPLEMENTATIONS

### Vector Manipulation (No Standard Library Methods)
```
// Get subset of vector (replaces slice)
def getSubset(vec, startIdx, endIdx) {
    var result = [];
    for (var i = startIdx; i < endIdx && i < vec.size(); ++i) {
        result.push_back(vec[i]);
    }
    return result;
}

// Combine two vectors (replaces concat)
def combineVectors(vec1, vec2) {
    var result = [];
    for (var i = 0; i < vec1.size(); ++i) {
        result.push_back(vec1[i]);
    }
    for (var i = 0; i < vec2.size(); ++i) {
        result.push_back(vec2[i]);
    }
    return result;
}

// Find minimum of two values
def min(a, b) {
    return a < b ? a : b;
}

// Find maximum of two values
def max(a, b) {
    return a > b ? a : b;
}
```

### Parametric Curve Functions (For Various Shapes)
```
// Linear interpolation
def lerp(a, b, t) {
    return a + (b - a) * t;
}

// Semi-circular arch
def archPoint(span, height, t) {
    var z = -span/2 + span * t;
    var y = height * sin(PI * t);
    return [z, y];  // Return [z, y] for arch in XZ plane
}

// Parabolic curve
def parabolicPoint(span, height, t) {
    var z = -span/2 + span * t;
    var y = height * (1.0 - (2.0 * t - 1.0) * (2.0 * t - 1.0));
    return [z, y];
}

// Elliptical curve
def ellipticalPoint(a, b, t) {
    var angle = PI * t;
    var z = a * cos(angle + PI/2) + a;  // Offset to start at left
    var y = b * sin(angle + PI/2) + b;
    return [z, y];
}

// Point on sphere (for domes)
def sphericalPoint(radius, theta, phi) {
    var x = radius * sin(phi) * cos(theta);
    var y = radius * cos(phi);
    var z = radius * sin(phi) * sin(theta);
    return [x, y, z];
}
```

## UNIVERSAL STRUCTURAL COMPONENTS (BUILDING BLOCKS)

### 1. Creating Basic Shapes
```
// Create a rectangular prism frame
def createBoxFrame(width, depth, height) {
    var nodes = [];
    
    // Create 8 corner points
    nodes.push_back(addNodeWithIdx(-width/2, 0, -depth/2));       // 0: Front-left-bottom
    nodes.push_back(addNodeWithIdx(width/2, 0, -depth/2));        // 1: Front-right-bottom
    nodes.push_back(addNodeWithIdx(width/2, 0, depth/2));         // 2: Back-right-bottom
    nodes.push_back(addNodeWithIdx(-width/2, 0, depth/2));        // 3: Back-left-bottom
    nodes.push_back(addNodeWithIdx(-width/2, height, -depth/2));  // 4: Front-left-top
    nodes.push_back(addNodeWithIdx(width/2, height, -depth/2));   // 5: Front-right-top
    nodes.push_back(addNodeWithIdx(width/2, height, depth/2));    // 6: Back-right-top
    nodes.push_back(addNodeWithIdx(-width/2, height, depth/2));   // 7: Back-left-top
    
    // Connect edges (12 beams)
    // Bottom face
    addBeam(nodes[0], nodes[1]); addBeam(nodes[1], nodes[2]);
    addBeam(nodes[2], nodes[3]); addBeam(nodes[3], nodes[0]);
    
    // Top face
    addBeam(nodes[4], nodes[5]); addBeam(nodes[5], nodes[6]);
    addBeam(nodes[6], nodes[7]); addBeam(nodes[7], nodes[4]);
    
    // Vertical edges
    addBeam(nodes[0], nodes[4]); addBeam(nodes[1], nodes[5]);
    addBeam(nodes[2], nodes[6]); addBeam(nodes[3], nodes[7]);
    
    return nodes;
}

// Create a grid of nodes in XZ plane at given height
def createGrid(width, depth, height, numX, numZ) {
    var nodes = [];
    for (var i = 0; i <= numX; ++i) {
        for (var j = 0; j <= numZ; ++j) {
            var x = -width/2 + width * (i / (numX * 1.0));
            var z = -depth/2 + depth * (j / (numZ * 1.0));
            nodes.push_back(addNodeWithIdx(x, height, z));
        }
    }
    return nodes;
}

// Connect a grid of nodes
def connectGrid(nodes, numX, numZ) {
    var numNodesPerRow = numZ + 1;
    
    // Connect along X direction
    for (var i = 0; i < numX; ++i) {
        for (var j = 0; j <= numZ; ++j) {
            var idx1 = i * numNodesPerRow + j;
            var idx2 = (i + 1) * numNodesPerRow + j;
            addBeam(nodes[idx1], nodes[idx2]);
        }
    }
    
    // Connect along Z direction
    for (var i = 0; i <= numX; ++i) {
        for (var j = 0; j < numZ; ++j) {
            var idx1 = i * numNodesPerRow + j;
            var idx2 = i * numNodesPerRow + (j + 1);
            addBeam(nodes[idx1], nodes[idx2]);
        }
    }
}

// Add cross bracing to any rectangular face
def addCrossBracing(node1, node2, node3, node4) {
    addBeam(node1, node3);  // Diagonal 1
    addBeam(node2, node4);  // Diagonal 2
}
```

### 2. Creating Curved Structures
```
// Create an arch structure along Z-axis
def createArch(width, span, height, numSegments) {
    var frontNodes = [];
    var backNodes = [];
    
    for (var i = 0; i <= numSegments; ++i) {
        var t = i / (numSegments * 1.0);
        var point = archPoint(span, height, t);
        
        // point[0] is Z, point[1] is Y
        frontNodes.push_back(addNodeWithIdx(-width/2, point[1], point[0]));
        backNodes.push_back(addNodeWithIdx(width/2, point[1], point[0]));
    }
    
    // Connect along the arch
    for (var i = 0; i < numSegments; ++i) {
        addBeam(frontNodes[i], frontNodes[i+1]);
        addBeam(backNodes[i], backNodes[i+1]);
    }
    
    // Connect front to back
    for (var i = 0; i <= numSegments; ++i) {
        addBeam(frontNodes[i], backNodes[i]);
    }
    
    return [frontNodes, backNodes];
}

// Create a dome structure
def createDome(radius, numSegmentsU, numSegmentsV) {
    var nodes = [];
    
    // Top center node
    var topNodeIdx = addNodeWithIdx(0, radius, 0);
    nodes.push_back(topNodeIdx);
    
    // Create rings of nodes
    for (var v = 1; v <= numSegmentsV; ++v) {
        var phi = (PI/2) * (v / (numSegmentsV * 1.0));
        var ringNodes = [];
        
        for (var u = 0; u < numSegmentsU; ++u) {
            var theta = 2 * PI * (u / (numSegmentsU * 1.0));
            var point = sphericalPoint(radius, theta, phi);
            ringNodes.push_back(addNodeWithIdx(point[0], point[1], point[2]));
        }
        
        // Connect nodes in the same ring
        for (var i = 0; i < ringNodes.size(); ++i) {
            var next = (i + 1) % ringNodes.size();
            addBeam(ringNodes[i], ringNodes[next]);
        }
        
        // Connect with previous ring or top
        if (v == 1) {
            // Connect first ring with top
            for (var i = 0; i < ringNodes.size(); ++i) {
                addBeam(topNodeIdx, ringNodes[i]);
            }
        } else {
            // Get previous ring
            var prevRingStart = nodes.size() - ringNodes.size() - numSegmentsU;
            var prevRingEnd = nodes.size() - ringNodes.size();
            
            // Connect with nodes in previous ring
            for (var i = 0; i < ringNodes.size(); ++i) {
                var prevIdx = i % numSegmentsU;
                addBeam(ringNodes[i], nodes[prevRingStart + prevIdx]);
            }
        }
        
        // Add current ring nodes to all nodes
        for (var i = 0; i < ringNodes.size(); ++i) {
            nodes.push_back(ringNodes[i]);
        }
    }
    
    return nodes;
}
```

### 3. Creating Truss Structures
```
// Create a truss along Z-axis
def createTruss(width, span, height, numSegments) {
    var topNodes = [];
    var bottomNodes = [];
    var segmentLength = span / numSegments;
    
    // Create top and bottom chord nodes
    for (var i = 0; i <= numSegments; ++i) {
        var z = -span/2 + i * segmentLength;
        topNodes.push_back(addNodeWithIdx(-width/2, height, z));
        topNodes.push_back(addNodeWithIdx(width/2, height, z));
        bottomNodes.push_back(addNodeWithIdx(-width/2, 0, z));
        bottomNodes.push_back(addNodeWithIdx(width/2, 0, z));
    }
    
    // Connect top chord
    for (var i = 0; i < numSegments; ++i) {
        // Longitudinal beams
        addBeam(topNodes[i*2], topNodes[(i+1)*2]);        // Left side
        addBeam(topNodes[i*2+1], topNodes[(i+1)*2+1]);    // Right side
        // Cross beams
        addBeam(topNodes[i*2], topNodes[i*2+1]);          // Current segment
        addBeam(topNodes[(i+1)*2], topNodes[(i+1)*2+1]);  // Next segment
    }
    
    // Connect bottom chord
    for (var i = 0; i < numSegments; ++i) {
        // Longitudinal beams
        addBeam(bottomNodes[i*2], bottomNodes[(i+1)*2]);        // Left side
        addBeam(bottomNodes[i*2+1], bottomNodes[(i+1)*2+1]);    // Right side
        // Cross beams
        addBeam(bottomNodes[i*2], bottomNodes[i*2+1]);          // Current segment
        addBeam(bottomNodes[(i+1)*2], bottomNodes[(i+1)*2+1]);  // Next segment
    }
    
    // Connect vertical and diagonal members
    for (var i = 0; i <= numSegments; ++i) {
        // Vertical members at each node
        addBeam(topNodes[i*2], bottomNodes[i*2]);        // Left side
        addBeam(topNodes[i*2+1], bottomNodes[i*2+1]);    // Right side
        
        // Diagonal bracing (alternating pattern)
        if (i < numSegments) {
            if (i % 2 == 0) {
                addBeam(bottomNodes[i*2], topNodes[(i+1)*2]);        // Left side
                addBeam(bottomNodes[i*2+1], topNodes[(i+1)*2+1]);    // Right side
            } else {
                addBeam(topNodes[i*2], bottomNodes[(i+1)*2]);        // Left side
                addBeam(topNodes[i*2+1], bottomNodes[(i+1)*2+1]);    // Right side
            }
        }
    }
    
    return [topNodes, bottomNodes];
}
```

## STRUCTURE-SPECIFIC IMPLEMENTATIONS

### 1. For Bridges
- For ARCH BRIDGES: Use createArch() for the arch, then add deck above
- For TRUSS BRIDGES: Use createTruss() with proper diagonal bracing
- For SUSPENSION BRIDGES: Create towers, deck, and catenary cables
- For BEAM BRIDGES: Use createBoxFrame() with internal reinforcement

### 2. For Buildings and Towers
- For RECTANGULAR BUILDINGS: Use createBoxFrame() plus internal floors
- For TOWERS: Stack rectangular sections with proper vertical alignment
- For SKYSCRAPERS: Use core + perimeter design with proper bracing

### 3. For Domes and Shells
- For DOMES: Use createDome() with proper triangulation
- For GEODESIC DOMES: Use spherical coordinates with triangular faces
- For SHELLS: Use parametric equations with proper discretization

## NODE AND BEAM CONNECTION RULES
1. ALWAYS create nodes first, then connect with beams
2. For stability, each node should connect to at least 3 other nodes
3. Rectangular faces MUST have diagonal bracing
4. NEVER connect a node to itself or duplicate connections
5. Ensure proper triangulation for structural stability
6. Define helper functions for repetitive patterns
7. Remember that XZ-plane nodes (y=0) are automatically supported

## COORDINATE CALCULATION VERIFICATION
1. Check your parametric equations before implementing
2. Verify nodes are properly spaced and aligned
3. For curved structures, verify proper angular distribution
4. ALWAYS use parameters from 0 to 1 for curves: t = i / (numSegments * 1.0)
5. ALWAYS use floating-point division: (numSegments * 1.0)

## VALIDATION CHECKLIST (VERIFY BEFORE FINALIZING CODE)
- PI is defined globally
- newModel() is called at the start
- All functions are defined before they are called
- All variables are properly initialized
- No undefined variables or methods are used
- Structure has proper structural stability (triangulation)
- Structure is properly oriented and centered
- Dimensions match the specification exactly
- No mathematical errors in coordinate calculations
- All array indices are within valid bounds
- File compiles and runs without errors
- NO support constraints have been added - ground support assumed

## API FUNCTIONS
### Node Management
- addNode(double x, double y, double z) - void
- addNodeWithIdx(double x, double y, double z) - size_t
- nodeCount() - size_t
- nodePosAt(int i, double &x, double &y, double &z) - void
- updateNodePosAt(int i, double x, double y, double z) - void
- isNodeSelectedAt(int i) - bool

### Beam Management
- addBeam(int i0, int i1) - void
- addBeamWithIdx(int i0, int i1) - size_t
- beamCount() - size_t
- beamAt(int i, int &i0, int &i1) - void
- updateBeamAt(int i, int i0, int i1) - void

### Selection and Meshing
- selectAllElements() - void
- selectAllNodes() - void
- meshSelectedNodes() - void
- surfaceSelectedNodes(bool groundElements = true) - void

### Utility Functions
- randFloat(double min, double max) - double
- randInt(int min, int max) - int
- randSeed() - void)";
}

// Make a POST request to the Claude API
std::string StructureGenerator::makeClaudeRequest(const std::string &userPrompt)
{
    CURL *curl = curl_easy_init();
    std::string response;

    if (curl)
    {
        // Prepare the request payload
        json requestData = {{"model", m_model},
                            {"max_tokens", 4000},
                            {"messages", json::array({{{"role", "user"}, {"content", userPrompt}}})},
                            {"system", m_systemPrompt}};

        std::string requestBody = requestData.dump(-1, ' ', false, json::error_handler_t::replace);

        // Set up headers
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        std::string authHeader = "x-api-key: " + m_apiKey;
        headers = curl_slist_append(headers, authHeader.c_str());
        headers = curl_slist_append(headers, "anthropic-version: 2023-06-01");

        // Configure the request
        curl_easy_setopt(curl, CURLOPT_URL, m_apiUrl.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, requestBody.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        // Execute the request
        CURLcode res = curl_easy_perform(curl);

        // Check for errors
        if (res != CURLE_OK)
        {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
            response = "";
        }

        // Clean up
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }

    return response;
}

// Extract ChaiScript code from Claude's response
std::string StructureGenerator::extractChaiScript(const std::string &claudeResponse)
{
    try
    {
        json responseJson = json::parse(claudeResponse);

        // Extract the content from Claude's response
        std::string content = responseJson["content"][0]["text"];

        // Extract code between ```chaiscript and ``` markers
        size_t startPos = content.find("```chaiscript");
        if (startPos == std::string::npos)
        {
            startPos = content.find("```");
            if (startPos == std::string::npos)
            {
                // Return the whole content if no code blocks found
                return content;
            }
        }

        startPos = content.find("\n", startPos) + 1;
        size_t endPos = content.find("```", startPos);

        if (endPos == std::string::npos)
        {
            return content.substr(startPos);
        }

        return content.substr(startPos, endPos - startPos);
    } catch (const std::exception &e)
    {
        std::cerr << "Error parsing Claude response: " << e.what() << std::endl;
        return "";
    }
}

void ofai::StructureGenerator::loadSystemPromptFrom(const std::string &filename)
{
    m_systemPrompt = ofutil::read_file(filename);
}

// Public method to generate ChaiScript code from a prompt (synchronous)
std::string StructureGenerator::generateStructure(const std::string &prompt)
{
    // Construct a full prompt with the user's input
    std::stringstream fullPrompt;
    fullPrompt << "Create a ChaiScript implementation for the following structure: " << prompt;
    fullPrompt << "\n\nThe code should define an optimized 3D beam/bar structure with minimal redundant elements. "
                  "Return only the ChaiScript code needed to create this structure.";

    // Make the API request
    std::string claudeResponse = makeClaudeRequest(fullPrompt.str());

    if (claudeResponse.empty())
    {
        return "// Error: Failed to generate structure code";
    }

    // Extract ChaiScript from the response
    return extractChaiScript(claudeResponse);
}

// Asynchronous version with callback
void StructureGenerator::generateStructureAsync(const std::string &prompt, GenerationCallback callback)
{
    // Create a new thread that will execute the request and call the callback when done
    std::thread([this, prompt, callback]() {
        try
        {
            std::string result = generateStructure(prompt);
            callback(result, true);
        } catch (const std::exception &e)
        {
            std::string errorMsg = std::string("Error generating structure: ") + e.what();
            callback(errorMsg, false);
        }
    }).detach(); // Detach the thread so it runs independently
}

// Asynchronous version with future
std::future<std::string> StructureGenerator::generateStructureAsync(const std::string &prompt)
{
    // Use std::async to run the generateStructure function asynchronously
    return std::async(std::launch::async, [this, prompt]() { return generateStructure(prompt); });
}

// Set the model to use for generation
void StructureGenerator::setModel(const std::string &modelName)
{
    m_model = modelName;
}
