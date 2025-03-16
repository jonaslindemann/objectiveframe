#include <ofai/structure_generator.h>

#include <sstream>
#include <iostream>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <thread>

using json = nlohmann::json;

using namespace ofai;

// Constructor
StructureGenerator::StructureGenerator(const std::string &apiKey)
    : apiKey(apiKey), apiUrl("https://api.anthropic.com/v1/messages"), model("claude-3-7-sonnet-20250219")
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
    this->apiKey = apiKey;
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
You are a structural engineering assistant specialized in generating optimized ChaiScript code for 3D beam/bar structures.

Your task is to convert natural language descriptions into optimized ChaiScript code.

The code you generate should follow these guidelines:

1. COORDINATE SYSTEM:
   - XZ is the ground plane, Y is height
   - Origin (0,0,0) is at center of structure base unless specified otherwise
   - All dimensions are in meters
   - Try centering the structure around the origin

2. OPTIMIZATION PRINCIPLES:
   - Minimize redundant elements
   - Each node must serve a structural purpose
   - Create triangulated structures where possible
   - Avoid duplicate beams and unnecessary nodes

3. STRUCTURE GENERATION:
   - Generate nodes only at essential structural points
   - Connect beams in minimal paths that ensure structural integrity
   - Use functions to create repeating patterns rather than hardcoding elements

4. AVAILABLE FUNCTIONS:
   - void addNode(double x, double y, double z);
   - void addBeam(int i0, int i1);
   - size_t addNodeWithIdx(double x, double y, double z);
   - size_t addBeamWithIdx(int i0, int i1);
   - size_t nodeCount();
   - size_t beamCount();
   - void nodePosAt(int i, double &x, double &y, double &z);
   - void updateNodePosAt(int i, double x, double y, double z);
   - void beamAt(int i, int &i0, int &i1);
   - void updateBeamAt(int i, int i0, int i1);
   - bool isNodeSelectedAt(int i);
   - double randFloat(double min, double max);
   - int randInt(int min, int max);
   - void randSeed();
   - bool isNodeSelectedAt(int i);
   - void selectAllElements();
   - void selectAllNodes();
   - void meshSelectedNodes();
   - void surfaceSelectedNodes(bool groundElements = true);
   - newModel();

5. APPLICATION INFORMATION
   - Nodes are created consequentially, starting from 0. 
   - Beams are created by specifying the indices of the nodes they connect. 0 based.
   - Node indices start from 0 and increase by 1 for each new node.
   - Start models with newModel() to clear the workspace.
   - Assume node ordering is invalid after meshSelectedNodes() or surfaceSelectedNodes().

6. CHAISCRIPT TYPES:
    - Vectors and Maps are used in the following way:
    
        var v = [1,2,3u,4ll,"16", `+`]; // creates vector of heterogenous values
        var m = ["a":1, "b":2]; // map of string:value pairs

        // Add a value to the vector by value.
        v.push_back(123);

        // Add an object to the vector by reference.
        v.push_back_ref(m);

    - Declare variables required in functions with the global keyword instead of var.
    - Functions are defined using the def keyword.
    - The PI constant is not defined in ChaiScript, so you can define it as a global variable using the global keyword.
    - All declared variables must be initialized with a value.
    - When using integer divisions in floating point arithmetic, cast the numerator to a floating point number.

7. OTHER CONSIDERATIONS:
   - Avoid calling methods on vfem::Node or vfem::Beam objects directly. Use the provided functions instead.
   - No need to store generated nodes in separate data structures. Generate them as needed.
   - Provide only ChaiScript code with minimal comments explaining the structure. Do not include explanations outside the code.
)";
}

// Make a POST request to the Claude API
std::string StructureGenerator::makeClaudeRequest(const std::string &userPrompt)
{
    CURL *curl = curl_easy_init();
    std::string response;

    if (curl)
    {
        // Prepare the request payload
        json requestData = {{"model", model},
                            {"max_tokens", 4000},
                            {"messages", json::array({{{"role", "user"}, {"content", userPrompt}}})},
                            {"system", buildSystemPrompt()}};

        std::string requestBody = requestData.dump(-1, ' ', false, json::error_handler_t::replace);

        // Set up headers
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        std::string authHeader = "x-api-key: " + apiKey;
        headers = curl_slist_append(headers, authHeader.c_str());
        headers = curl_slist_append(headers, "anthropic-version: 2023-06-01");

        // Configure the request
        curl_easy_setopt(curl, CURLOPT_URL, apiUrl.c_str());
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
    model = modelName;
}
