# AI / LLM integration

ObjectiveFrame includes an AI-assisted workflow for generating and modifying beam models from natural language. The assistant sends your prompt to an LLM, receives ChaiScript code, and can run that code inside ObjectiveFrame to create or update the current model.

## What the AI creates

The LLM integration does not edit the model directly. Instead it generates ChaiScript using the same scripting functions available in the script editor. This makes the result inspectable before it is executed.

Typical prompts include:

- Create a 10 m by 20 m truss bridge with diagonal bracing.
- Add fixed supports to the ground nodes of the current model.
- Add a downward load to the highest node.
- Modify the current model by adding cross bracing to each wall.
- Repeat the current frame four times along X and three times along Z, 5 m apart.

The prompt system distinguishes between two important modes:

- **Creation**: use this for a new structure. The generated script normally starts with `newModel()`.
- **Modification**: use this for changes to the current structure. The generated script should not call `newModel()`.

When you want to preserve the current model, say so explicitly in the prompt.

## Opening the prompt window

You can open the AI prompt window from:

- the start popup, using **Open AI prompt...**
- the main menu, using **Tools/Create using AI**

The prompt window has two tabs:

- **Generate**: write a prompt, generate ChaiScript, review the response, and run it.
- **Prompts**: browse built-in prompt examples by category, difficulty, or search term.

## API key setup

The LLM service requires an API key. Open **View/Settings...** and enter the key in **AI API key**. The key can also be pasted from the clipboard using the button in the settings window.

ObjectiveFrame stores the key in the local application configuration so it can be reused in later sessions.

!!! note

    The current implementation uses the Anthropic Messages API and defaults to the `claude-sonnet-4-5` model in the application code.

## Generating and running scripts

1. Open the prompt window.
2. Write a clear model request in the **Generate** tab.
3. Click **Generate**.
4. Review the generated ChaiScript in the response area.
5. Leave **Auto run** enabled to run successful responses automatically, or disable it and click **Run** after reviewing the script.

If the generated script fails, the error area shows the ChaiScript evaluation message. You can edit the generated script in the response area and run it again.

## Prompting tips

Be explicit about units, dimensions, and intent. Mention whether the prompt should create a new model or modify the current one.

Good prompts:

```text
Create a new 12 m span truss bridge, 4 m wide and 3 m high, with fixed supports at both ends.
```

```text
Modify the current model by adding diagonal bracing to each rectangular bay. Do not clear the model.
```

```text
Add a 1000 N downward nodal load to each top chord node. Keep the existing supports.
```

Use the **Prompts** tab when you want a starting point. The examples are loaded from the local prompt database in the application package.

## Privacy and network use

Prompt text is sent to the configured LLM provider. Generated code is returned to ObjectiveFrame and executed locally. Avoid sending confidential project information unless the configured provider and your usage policy allow it.
