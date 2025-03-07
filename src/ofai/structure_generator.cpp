#include <ofai/structure_generator.h>

#include <sstream>
#include <iostream>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

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

// Static callback function for cURL to store API response
size_t StructureGenerator::WriteCallback(void *contents, size_t size, size_t nmemb, std::string *output)
{
    size_t totalSize = size * nmemb;
    output->append((char *)contents, totalSize);
    return totalSize;
}

// Build the system prompt with preconditions for optimization
std::string StructureGenerator::buildSystemPrompt() const
{
    return R"(
You are a structural engineering assistant specialized in generating optimized ChaiScript code for 3D beam/bar structures.

Your task is to convert natural language descriptions into optimized ChaiScript code.

The code you generate should follow these guidelines:

1. COORDINATE SYSTEM:
   - XZ is the ground plane, Y is height
   - Origin (0,0,0) is at center of structure base unless specified otherwise

2. OPTIMIZATION PRINCIPLES:
   - Minimize redundant elements
   - Each node must serve a structural purpose
   - Use diagonal bracing strategically, no more than one per wall face
   - Create triangulated structures where possible
   - Avoid duplicate beams and unnecessary nodes

3. STRUCTURE GENERATION:
   - Generate nodes only at essential structural points
   - Connect beams in minimal paths that ensure structural integrity
   - Use functions to create repeating patterns rather than hardcoding elements
   - Aim for the absolute minimum number of elements while maintaining stability

4. AVAILABLE FUNCTIONS:
   - vfem::Node *addNode(double x, double y, double z);
   - vfem::Beam *addBeam(int i0, int i1);
   - size_t nodeCount();
   - void nodePos(vfem::Node *node, double &x, double &y, double &z);
   - void nodePosAt(int i, double &x, double &y, double &z);
   - void updateNodePos(vfem::Node *node, double x, double y, double z);
   - void updateNodePosAt(int i, double x, double y, double z);
   - vfem::Node *nodeAt(int i);
   - bool isNodeSelected(vfem::Node *node);
   - bool isNodeSelectedAt(int i);
   - double randFloat(double min, double max);
   - int randInt(int min, int max);
   - void randSeed();

Provide only ChaiScript code with minimal comments explaining the structure. Do not include explanations outside the code.
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

// Public method to generate ChaiScript code from a prompt
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

// Set the model to use for generation
void StructureGenerator::setModel(const std::string &modelName)
{
    model = modelName;
}
