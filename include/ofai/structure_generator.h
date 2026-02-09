#pragma once

#include <string>
#include <vector>
#include <future>
#include <functional>
#include <nlohmann/json.hpp>

namespace ofai {

struct PromptExample {
    std::string id;
    std::string name;
    std::string prompt;
    std::string difficulty;
    std::vector<std::string> tags;
};

struct PromptCategory {
    std::string id;
    std::string name;
    std::string description;
    std::vector<PromptExample> examples;
};

class PromptDatabase {
private:
    nlohmann::json m_data;
    std::vector<PromptCategory> m_categories;
    std::string m_version;
    std::string m_description;

    void parseCategories();

public:
    PromptDatabase() = default;

    /**
     * @brief Load the prompt database from a JSON file
     * @param filename Path to the JSON file
     * @return true if loaded successfully, false otherwise
     */
    bool loadFromFile(const std::string& filename);

    /**
     * @brief Load the prompt database from a JSON string
     * @param jsonString JSON string containing the database
     * @return true if loaded successfully, false otherwise
     */
    bool loadFromString(const std::string& jsonString);

    /**
     * @brief Get all categories in the database
     * @return Vector of categories
     */
    std::vector<PromptCategory> getCategories() const;

    /**
     * @brief Get a category by its ID
     * @param categoryId The category ID to search for
     * @return Category if found, otherwise an empty category
     */
    PromptCategory getCategoryById(const std::string& categoryId) const;

    /**
     * @brief Get an example by its ID
     * @param exampleId The example ID to search for
     * @return Example if found, otherwise an empty example
     */
    PromptExample getExampleById(const std::string& exampleId) const;

    /**
     * @brief Get all examples from a specific category
     * @param categoryId The category ID
     * @return Vector of examples from that category
     */
    std::vector<PromptExample> getExamplesByCategory(const std::string& categoryId) const;

    /**
     * @brief Search for examples by tag
     * @param tag The tag to search for
     * @return Vector of examples matching the tag
     */
    std::vector<PromptExample> getExamplesByTag(const std::string& tag) const;

    /**
     * @brief Search for examples by difficulty level
     * @param difficulty The difficulty level (beginner, intermediate, advanced, expert)
     * @return Vector of examples matching the difficulty
     */
    std::vector<PromptExample> getExamplesByDifficulty(const std::string& difficulty) const;

    /**
     * @brief Search for examples by name (case-insensitive partial match)
     * @param searchTerm The search term
     * @return Vector of examples matching the search term
     */
    std::vector<PromptExample> searchExamplesByName(const std::string& searchTerm) const;

    /**
     * @brief Get all examples from all categories
     * @return Vector of all examples
     */
    std::vector<PromptExample> getAllExamples() const;

    /**
     * @brief Get the database version
     * @return Version string
     */
    std::string getVersion() const { return m_version; }

    /**
     * @brief Get the database description
     * @return Description string
     */
    std::string getDescription() const { return m_description; }

    /**
     * @brief Check if the database is loaded
     * @return true if database contains data
     */
    bool isLoaded() const { return !m_categories.empty(); }
};

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
