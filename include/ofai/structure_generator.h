#pragma once

#include <string>
#include <vector>
#include <future>
#include <functional>

namespace ofai {

/**
 * A class that interfaces with the Claude API to generate optimized ChaiScript code
 * for structural modeling based on natural language prompts.
 */
class StructureGenerator {
public:
    // Define a callback type for asynchronous operations
    using GenerationCallback = std::function<void(const std::string &, bool)>;

private:
    std::string m_apiKey;
    std::string m_apiUrl;
    std::string m_model;
    std::string m_systemPrompt;

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

    void setApiKey(const std::string &apiKey);
    void loadSystemPromptFrom(const std::string &filename);

    /**
     * Generates ChaiScript code for a structure based on a natural language prompt.
     * This is the synchronous version.
     *
     * @param prompt The natural language description of the structure
     * @return A string containing the generated ChaiScript code
     */
    std::string generateStructure(const std::string &prompt);

    /**
     * Generates ChaiScript code asynchronously for a structure based on a natural language prompt.
     * Returns immediately and executes the callback when complete.
     *
     * @param prompt The natural language description of the structure
     * @param callback Function to call when generation is complete, with result and success flag
     */
    void generateStructureAsync(const std::string &prompt, GenerationCallback callback);

    /**
     * Generates ChaiScript code asynchronously and returns a future.
     *
     * @param prompt The natural language description of the structure
     * @return A future containing the generated ChaiScript code
     */
    std::future<std::string> generateStructureAsync(const std::string &prompt);

    /**
     * Sets the model to use for generation.
     *
     * @param modelName The name of the Claude model to use
     */
    void setModel(const std::string &modelName);
};

} // namespace ofai
