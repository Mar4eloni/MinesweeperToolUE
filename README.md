# MinesweeperToolUE
Creating custom tool for the unreal engine 5.5.4

Setup: 
- Unreal engine 5.5 is nedded. Download it trough the Epic Games Launcher
- Right click on the MineSweep.uproject and generate visual studio files
- Open the generated .sln file with Visual Studio or Rider
- Wait for the headers to load 
- Build and start the project 
- The tool is in Window menu of the main menu of the editor

<img width="616" alt="Screenshot 2025-04-30 054132" src="https://github.com/user-attachments/assets/5ae0dd99-ffb5-4232-9959-4b69cb6de8cf" />





The code: 
- The code for the tool is in the plugins folder \Plugins\MinesweeperTool\Source\MinesweeperTool

AI assited : 

- the slate logic in the construction script was done with the help of AI
- the grid creating was checked for errors by AI
- the reveal adjecent tiles was checked for optimal solution and checked for errors by AI

Added the MCP protocol to work with Claudia AI 
- working on adding the logic for opening the new tool
