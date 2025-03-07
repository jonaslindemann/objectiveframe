#pragma once

#include <string>
#include <vector>

namespace ofai {

/**
 * A class that interfaces with the Claude API to generate optimized ChaiScript code
 * for structural modeling based on natural language prompts.
 */
class StructureGenerator {
private:
    std::string apiKey;
    std::string apiUrl;
    std::string model;

    static size_t WriteCallback(void *contents, size_t size, size_t nmemb, std::string *output);
    std::string buildSystemPrompt() const;
    std::string makeClaudeRequest(const std::string &userPrompt);
    std::string extractChaiScript(const std::string &claudeResponse);

public:
    /**
     * Constructs a StructureGenerator with the specified API key.
     *
     * @param apiKey The Claude API key
     */
    StructureGenerator(const std::string &apiKey);

    ~StructureGenerator();

    /**
     * Generates ChaiScript code for a structure based on a natural language prompt.
     *
     * @param prompt The natural language description of the structure
     * @return A string containing the generated ChaiScript code
     */
    std::string generateStructure(const std::string &prompt);

    /**
     * Sets the model to use for generation.
     *
     * @param modelName The name of the Claude model to use
     */
    void setModel(const std::string &modelName);
};

} // namespace ofai
