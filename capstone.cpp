#include <iostream>     
#include <string>      
#include <vector>     
#include <map>        
#include <utility>    
#include <algorithm>   

using std::string;
using std::cout;
using std::endl;
using std::cin;
using std::getline;

// GameColors namespace containing ANSI escape codes for text coloring and styling
namespace GameColors {
    // Basic text styling
    const std::string reset = "\033[0m";      // Resets all formatting to default
    const std::string bold = "\033[1m";       // Makes text bold/bright
    
    // Standard ANSI colors
    const std::string blue = "\033[34m";      // Basic blue color
    const std::string red = "\033[31m";       // Basic red color
    const std::string green = "\033[32m";     // Basic green color
    const std::string yellow = "\033[33m";    // Basic yellow color
    const std::string cyan = "\033[36m";      // Basic cyan color
    const std::string magenta = "\033[35m";   // Basic magenta color
    
    // Custom 256-color codes for specific game locations
    const std::string forestColor = "\033[38;5;28m";    // Dark green for forest areas
    const std::string ruinsColor = "\033[38;5;137m";    // Sandy brown for ancient ruins
    const std::string caveColor = "\033[38;5;240m";     // Dark gray for cave systems
    const std::string mountainColor = "\033[38;5;248m";  // Light gray for mountain peaks
    const std::string lakeColor = "\033[38;5;39m";      // Light blue for water bodies
    const std::string villageColor = "\033[38;5;180m";   // Light brown for village areas
    const std::string hiddenRoomColor = "\033[38;5;141m"; // Light purple for secret areas
}

using namespace std;  // Add this to resolve std namespace issues

class Room;    // Main class for game locations
class NPC;     // Non-Player Character class
class Player;  // Player character class

// Enumeration for cardinal directions, used for room connections
enum Direction { NORTH = 0, EAST, SOUTH, WEST };  // Values auto-increment from 0

// Enumeration for different types of items player can collect
enum ItemType { NONE, SWORD, KEY, TREASURE };  // Values auto-increment from 0

// Enumeration for different types of NPCs in the game
enum NPCType { NO_NPC, VILLAGER, MONSTER };    // Values auto-increment from 0

// NPC (Non-Player Character) class definition
class NPC {
public:
    NPCType type;        // Type of NPC (villager or monster)
    bool isDefeated;     // Tracks if monster has been defeated
    std::string dialogue;// Text displayed when player talks to NPC

    // Constructor initializes NPC with type and dialogue
    NPC(NPCType t, const std::string& d) : type(t), dialogue(d), isDefeated(false) {}
};

// Room class represents a location in the game world
class Room {
private:
    Room* connections[4];         // Array of pointers to connected rooms (NORTH, EAST, SOUTH, WEST)
    std::string availablePaths[4]; // Array of direction names for available exits
    int numPaths;                 // Number of available exits from this room
    
public:
    std::string name;             // Display name of the room
    std::string description;      // Short description shown on entry
    std::string detailedDescription; // Longer description shown with 'look' command
    ItemType item;                // Item present in the room (if any)
    NPC* npc;                     // NPC present in the room (if any)
    bool isLocked;                // Whether the room is currently accessible

    // Constructor initializes room with name and basic description
    Room(const std::string& n, const std::string& desc) 
        : name(n), description(desc), detailedDescription(""), item(NONE), npc(nullptr), isLocked(false), numPaths(0) {
        // Initialize all connections to nullptr
        for(int i = 0; i < 4; i++) {
            connections[i] = nullptr;
        }
    }

    // Getter methods for private members
    Room** getConnections() { return connections; }
    const Room* const* getConnections() const { return connections; }
    std::string* getAvailablePaths() { return availablePaths; }
    const std::string* getAvailablePaths() const { return availablePaths; }
    int getNumPaths() const { return numPaths; }

    // Adds a new available direction to the room
    void addPath(const std::string& path) {
        if (numPaths < 4) {
            availablePaths[numPaths++] = path;
        }
    }

    // Removes all available paths from the room
    void clearPaths() {
        numPaths = 0;
    }

    // Converts Direction enum to string representation
    std::string directionToString(Direction dir) {
        switch(dir) {
            case NORTH: return std::string("north");
            case SOUTH: return std::string("south");
            case EAST: return std::string("east");
            case WEST: return std::string("west");
            default: return std::string("unknown");
        }
    }

    // Establishes a two-way connection between this room and another
    void setConnection(Direction dir, Room* room) {
        if (room == nullptr) {
            return;
        }
        
        // Set connection in specified direction
        connections[dir] = room;
        addPath(directionToString(dir));
        
        // Set reverse connection automatically
        Direction opposite;
        switch(dir) {
            case NORTH: opposite = SOUTH; break;
            case SOUTH: opposite = NORTH; break;
            case EAST: opposite = WEST; break;
            case WEST: opposite = EAST; break;
        }
        
        room->connections[opposite] = this;
        room->addPath(directionToString(opposite));
    }

    // Displays basic room information on entry
    void describe() {
        std::cout << "\nYou are in " << name << ". " << description << std::endl;
        showAvailablePathsAndItems();
    }

    // Displays detailed room information when looking
    void describeLook() {
        std::cout << "\nYou carefully examine " << name << ".\n" << detailedDescription << std::endl;
        showAvailablePathsAndItems();
    }

    // Helper function to display paths, NPCs, and items
    void showAvailablePathsAndItems() {
        // Show available exits
        if (getNumPaths() > 0) {
            std::cout << GameColors::cyan << "Available paths lead: ";
            for (int i = 0; i < getNumPaths(); ++i) {
                std::cout << GameColors::yellow << getAvailablePaths()[i] << GameColors::cyan;
                if (i < getNumPaths() - 1) {
                    std::cout << ", ";
                }
            }
            std::cout << "." << GameColors::reset << std::endl;
        }

        // Show NPCs if present
        if (npc != nullptr && !npc->isDefeated) {
            if (npc->type == NPCType::VILLAGER) {
                std::cout << GameColors::bold << GameColors::cyan << "There is a villager here you can talk to." << GameColors::reset << std::endl;
            } else if (npc->type == NPCType::MONSTER) {
                std::cout << GameColors::bold << GameColors::red << "A fearsome monster blocks your path!" << GameColors::reset << std::endl;
            }
        }

        // Show items if present
        if (item != ItemType::NONE) {
            switch(item) {
                case ItemType::SWORD:
                    std::cout << GameColors::bold << GameColors::green << "There is a sword here that you can take." << GameColors::reset << std::endl;
                    break;
                case ItemType::KEY:
                    std::cout << GameColors::bold << GameColors::yellow << "There is a key here that you can take." << GameColors::reset << std::endl;
                    break;
                case ItemType::TREASURE:
                    std::cout << GameColors::bold << GameColors::yellow << "There is a treasure chest here!" << GameColors::reset << std::endl;
                    break;
                default:
                    break;
            }
        }
    }

    // Checks if movement in a given direction is possible
    bool canMove(Direction dir) {
        return connections[dir] != nullptr;
    }
};

// Player class manages the player's state and inventory
class Player {
private:
    bool itemInventory[3];  // Fixed-size array tracking collected items (SWORD, KEY, TREASURE)
    
public:
    Room* currentRoom;      // Pointer to the room player is currently in
    bool hasTreasure;       // Flag indicating if player has found the treasure
    int moveCount;          // Number of moves player has made

    // Constructor initializes player at starting room with empty inventory
    Player(Room* startingRoom) : currentRoom(startingRoom), hasTreasure(false), moveCount(0) {
        itemInventory[SWORD-1] = false;
        itemInventory[KEY-1] = false;
        itemInventory[TREASURE-1] = false;
    }

    // Checks if player has a specific item
    bool hasItem(ItemType item) {
        return item != NONE && itemInventory[item-1];
    }

    // Adds an item to player's inventory
    void addItem(ItemType item) {
        if (item != NONE) {
            itemInventory[item-1] = true;
            if (item == TREASURE) {
                hasTreasure = true;
            }
        }
    }

    // Checks if player can engage in combat
    bool canFight() {
        return hasItem(SWORD);
    }
};

int main() {
    // Create styled room names with appropriate colors and formatting
    std::string forestName = std::string(GameColors::forestColor) + GameColors::bold + "Forest" + GameColors::reset;  
    std::string ruinsName = std::string(GameColors::ruinsColor) + GameColors::bold + "Ruins" + GameColors::reset;   
    std::string caveName = std::string(GameColors::caveColor) + GameColors::bold + "Cave" + GameColors::reset;   
    std::string mountainName = std::string(GameColors::mountainColor) + GameColors::bold + "\033[3m" + "Mountain" + GameColors::reset;  // Italicized
    std::string valleyName = std::string("\033[38;5;106m") + GameColors::bold + "Valley" + GameColors::reset;  // Light green
    std::string lakeName = std::string(GameColors::lakeColor) + GameColors::bold + "\033[3m" + "Lake" + GameColors::reset;  // Italicized
    std::string villageName = std::string(GameColors::villageColor) + GameColors::bold + "Village" + GameColors::reset;
    std::string hiddenRoomName = std::string(GameColors::hiddenRoomColor) + GameColors::bold + "Hidden Room" + GameColors::reset;

    // Create and initialize all rooms with their descriptions
    // Forest - Starting Room
    Room forest(forestName, std::string(GameColors::forestColor) + "A dark forest surrounds you. Ancient trees tower overhead, and the air is thick with the scent of pine." + GameColors::reset);
    forest.detailedDescription = std::string(GameColors::forestColor) + "Ancient trees tower overhead, their branches swaying in the breeze. The air is thick with the scent of pine and wild mushrooms. Fallen leaves crunch beneath your feet, and somewhere in the distance, an owl hoots softly. The dense canopy above allows only occasional shafts of light to penetrate to the forest floor. You notice some old footprints leading east." + GameColors::reset;
    
    // Ruins - Ancient civilization remains
    Room ruins(ruinsName, std::string(GameColors::ruinsColor).append("Crumbling stone walls and weathered pillars tell tales of an ancient civilization.").append(GameColors::reset));
    ruins.detailedDescription = std::string(GameColors::ruinsColor).append("Crumbling stone walls and weathered pillars tell tales of an ancient civilization. Intricate carvings, though worn by time, still adorn the weathered stones. Vines and moss have claimed much of the architecture. Among the broken pottery shards, you spot what appears to be a map fragment showing a path leading south.").append(GameColors::reset);
    
    // Cave - Dark and mysterious location
    Room cave(caveName, std::string(GameColors::caveColor).append("The damp cave walls glisten with moisture. Strange echoes bounce off the rocky surfaces.").append(GameColors::reset));
    cave.detailedDescription = std::string(GameColors::caveColor).append("The damp cave walls glisten with moisture. Strange echoes bounce off the rocky surfaces, making it impossible to tell their source. Mineral formations catch what little light there is, creating an otherworldly atmosphere. The monster's presence makes it difficult to explore further, but you sense something valuable might be hidden here.").append(GameColors::reset);
    
    // Mountain - Elevated vantage point
    Room mountain(mountainName, std::string(GameColors::mountainColor).append("The majestic mountain peak pierces the clouds above. The air is thin but crisp.").append(GameColors::reset));
    mountain.detailedDescription = std::string(GameColors::mountainColor).append("The air is thin but crisp, and the view from here is breathtaking. Snow-capped peaks stretch into the distance, and the wind whistles through the rocky crags. Ancient runes are carved into some of the larger boulders. The eastern rock face seems unusually smooth compared to the rest.").append(GameColors::reset);
    
    // Valley - Peaceful transition area
    Room valley(valleyName, std::string("\033[38;5;106m").append("A serene valley stretches between the mountains.").append(GameColors::reset));
    valley.detailedDescription = std::string("\033[38;5;106m").append("Wildflowers dot the gentle slopes, creating a carpet of vibrant colors. A gentle breeze carries the sweet scent of mountain blooms, and butterflies dance among the flowers. Small streams trickle down from the heights, creating a peaceful melody. The path continues east towards what appears to be a large body of water.").append(GameColors::reset);
    
    // Lake - Reflective water body
    Room lake(lakeName, std::string(GameColors::lakeColor).append("Crystal clear waters stretch before you, reflecting the sky like a mirror.").append(GameColors::reset));
    lake.detailedDescription = std::string(GameColors::lakeColor).append("Crystal clear waters stretch before you, reflecting the sky like a mirror. The surface occasionally ripples as fish jump, creating expanding circles that distort the perfect reflection. The shoreline is dotted with smooth pebbles and tall reeds. Through the clear water, you can make out what looks like an old path leading south.").append(GameColors::reset);
    
    // Village - Inhabited settlement
    Room village(villageName, std::string(GameColors::villageColor).append("A peaceful village with thatched-roof houses and cobblestone streets.").append(GameColors::reset));
    village.detailedDescription = std::string(GameColors::villageColor).append("Thatched-roof houses line the cobblestone streets, smoke rising from their chimneys. The scent of hearth fires and cooking meals fills the air. Children play between the buildings while adults go about their daily tasks. You overhear villagers discussing local legends about hidden treasures and secret passages in the mountains.").append(GameColors::reset);
    
    // Hidden Room - Secret treasure location
    Room hiddenRoom(hiddenRoomName, std::string(GameColors::hiddenRoomColor).append("This dusty chamber seems untouched for centuries. An ornate chest catches your eye.").append(GameColors::reset));
    hiddenRoom.detailedDescription = std::string(GameColors::hiddenRoomColor).append("This dusty chamber seems untouched for centuries. An ornate chest catches your eye, its metalwork still gleaming despite its age. The walls are covered in elaborate tapestries depicting ancient battles and mystical creatures. Precious gems and metals are worked into the very structure of the room, creating a subtle sparkle in the dim light.").append(GameColors::reset);

    // Add NPCs and items to specific rooms
    village.npc = new NPC(VILLAGER, "Greetings traveler! Let me show you a map of the area:\n\n" 
        + GameColors::bold + GameColors::blue + R"(
    Village ─── Valley ─── Lake
        │                    │
        │                    │
    Forest ─── Ruins         │
                  │          │
                  │          │
                Cave         │
                             │
                         Mountain
)" + GameColors::reset + "\n" + GameColors::cyan + "There are many interesting places to explore. I've heard whispers of ancient treasures hidden somewhere in these lands, but their location remains a mystery..." + GameColors::reset);
    cave.npc = new NPC(MONSTER, "A fearsome monster guards a mysterious key!");
    mountain.item = SWORD;
    hiddenRoom.item = TREASURE;
    hiddenRoom.isLocked = true;

    // Setup room connections to create the game world layout
    // Village area connections
    village.setConnection(EAST, &valley);
    village.setConnection(SOUTH, &forest);

    // Valley connections
    valley.setConnection(EAST, &lake);

    // Forest connections
    forest.setConnection(EAST, &ruins);

    // Lake connections
    lake.setConnection(SOUTH, &mountain);

    // Ruins connections
    ruins.setConnection(SOUTH, &cave);

    // Mountain connections (including hidden path)
    mountain.setConnection(EAST, &hiddenRoom);

    // Hide the east path from mountain to hidden room
    mountain.clearPaths();  // Clear existing paths
    if (mountain.getConnections()[NORTH] != nullptr) {
        mountain.addPath(mountain.directionToString(NORTH));
    }
    if (mountain.getConnections()[SOUTH] != nullptr) {
        mountain.addPath(mountain.directionToString(SOUTH));
    }
    if (mountain.getConnections()[WEST] != nullptr) {
        mountain.addPath(mountain.directionToString(WEST));
    }

    // Initialize player at the starting location (forest)
    Player player(&forest);

    // Display welcome banner using Unicode block characters
    std::cout << GameColors::bold << GameColors::cyan;
    std::cout << R"(
█████╗    ██████╗  ██╗   ██╗ ███████╗ ███╗   ██╗ ████████╗ ██╗   ██╗ ██████╗  ███████╗
██╔══██╗  ██╔══██╗ ██║   ██║ ██╔════╝ ████╗  ██║ ╚══██╔══╝ ██║   ██║ ██╔══██╗ ██╔════╝
███████║  ██║  ██║ ██║   ██║ █████╗   ██╔██╗ ██║    ██║    ██║   ██║ ██████╔╝ █████╗  
██╔══██║  ██║  ██║ ╚██╗ ██╔╝ ██╔══╝   ██║╚██╗██║    ██║    ██║   ██║ ██╔══██╗ ██╔══╝  
██║  ██║  ██████╔╝  ╚████╔╝  ███████╗ ██║ ╚████║    ██║    ╚██████╔╝ ██║  ██║ ███████╗
╚═╝  ╚═╝  ╚═════╝    ╚═══╝   ╚══════╝ ╚═╝  ╚═══╝    ╚═╝     ╚═════╝  ╚═╝  ╚═╝ ╚══════╝
)" << GameColors::reset << std::endl;

    // Display welcome message and game introduction
    std::cout << GameColors::bold << GameColors::magenta << R"(
╔═══════════════════════════════════════════╗
║           Welcome to the Adventure!       ║
╚═══════════════════════════════════════════╝
)" << GameColors::reset << std::endl;

    // Display story introduction with colored text
    std::cout << GameColors::bold << GameColors::forestColor 
         << "Welcome to the mystical realm of " << GameColors::cyan << "Eldara" << GameColors::forestColor 
         << ", where ancient magic flows through emerald forests" << std::endl
         << "and " << GameColors::lakeColor << "crystalline lakes shimmer with otherworldly light" << GameColors::forestColor << ". Hidden within this enchanted land lies a" << std::endl
         << GameColors::yellow << "legendary treasure" << GameColors::forestColor << ", sought after by brave adventurers for centuries." << std::endl << std::endl
         << "As you journey through the " << GameColors::forestColor << "whispering woods" << " and " << GameColors::ruinsColor << "crumbling ruins" << GameColors::forestColor << ", you'll encounter " << GameColors::villageColor << "friendly villagers" << std::endl 
         << GameColors::forestColor << "who hold age-old secrets, and " << GameColors::red << "fearsome creatures" << GameColors::forestColor << " who guard sacred places. The very air tingles with" << std::endl
         << GameColors::magenta << "arcane energy" << GameColors::forestColor << ", while " << GameColors::caveColor << "mysterious caves" << GameColors::forestColor << " and " << GameColors::mountainColor << "towering mountains" << GameColors::forestColor << " beckon you to explore their depths." << std::endl << std::endl
         << "Your quest will test both your " << GameColors::red << "courage" << GameColors::forestColor << " and " << GameColors::blue << "wisdom" << GameColors::forestColor << " as you unravel the mysteries of this magical realm." << std::endl
         << "The " << GameColors::yellow << "treasure" << GameColors::forestColor << " awaits those pure of heart and sharp of mind - will you be the one to discover its" << std::endl
         << "resting place?" << GameColors::reset << std::endl;
    
    // Display command menu with colored formatting
    std::cout << GameColors::bold 
              << "┌─────────────── " 
              << GameColors::yellow << "Commands" 
              << GameColors::cyan << " ───────────────┐\n"
              << "│ " << GameColors::green << "▶ n, s, e, w" 
              << GameColors::cyan << ": Movement                │\n"
              << "│ " << GameColors::yellow << "▶ take" 
              << GameColors::cyan << ": Pick up items                 │\n"
              << "│ " << GameColors::blue << "▶ talk" 
              << GameColors::cyan << ": Speak with characters         │\n"
              << "│ " << GameColors::red << "▶ fight" 
              << GameColors::cyan << ": Battle monsters              │\n"
              << "│ " << GameColors::magenta << "▶ help" 
              << GameColors::cyan << ": Show commands                 │\n"
              << "│ " << GameColors::yellow << "▶ quit" 
              << GameColors::cyan << ": Exit game                     │\n"
              << "└────────────────────────────────────────┘" 
              << GameColors::reset << std::endl;

    // Main game loop
    std::string command;
    while (true) {
        // Display current room description unless using look command
        if (command != "look") {
            player.currentRoom->describe();
        }
        
        // Display command prompt
        std::cout << "\n" << GameColors::bold << GameColors::blue << "┌─────────────────────┐" << std::endl
                 << "│  Enter a command:   │" << std::endl
                 << "└─────────────────────┘" << GameColors::reset << std::endl;
        std::getline(std::cin, command);

        // Handle look command - shows detailed room description
        if (command == "look") {
            // Special case for Mountain room when player has the key
            if (player.currentRoom->name.find("Mountain") != std::string::npos && player.hasItem(KEY)) {
                std::cout << GameColors::bold << GameColors::cyan 
                         << "\nAs you examine the area more carefully with your key in hand, you notice that some of the runes on the eastern rock face seem to form the outline of a doorway. Perhaps there's more here than meets the eye..." 
                         << GameColors::reset << std::endl;
            } else {
                player.currentRoom->describeLook();
            }
            continue;
        }

        // Handle help command - displays available commands
        if (command == "help") {
            std::cout << GameColors::bold 
                      << "┌─────────────── " 
                      << GameColors::yellow << "Commands" 
                      << GameColors::cyan << " ───────────────┐\n"
                      << "│ " << GameColors::green << "▶ n, s, e, w" 
                      << GameColors::cyan << ": Movement                │\n"
                      << "│ " << GameColors::yellow << "▶ take" 
                      << GameColors::cyan << ": Pick up items                 │\n"
                      << "│ " << GameColors::blue << "▶ talk" 
                      << GameColors::cyan << ": Speak with characters         │\n"
                      << "│ " << GameColors::red << "▶ fight" 
                      << GameColors::cyan << ": Battle monsters              │\n"
                      << "│ " << GameColors::magenta << "▶ help" 
                      << GameColors::cyan << ": Show commands                 │\n"
                      << "│ " << GameColors::yellow << "▶ quit" 
                      << GameColors::cyan << ": Exit game                     │\n"
                      << "└────────────────────────────────────────┘" 
                      << GameColors::reset << std::endl;
            continue;
        }

        // Handle quit command - exits the game
        if (command == "quit") {
            std::cout << GameColors::bold << GameColors::blue << R"(
╔═══════════════════════════════════════════╗
║          Thanks for playing!              ║
╚═══════════════════════════════════════════╝
)" << GameColors::reset << std::endl;
            break;
        }

        // Check for victory condition (found treasure)
        if (player.hasTreasure) {
            std::cout << "\n" << GameColors::bold << GameColors::green << R"(
╔════════════════════════════════════════════════════╗
║     🎉 Congratulations! You found the treasure! 🎉  ║
║        You completed the game in )" << player.moveCount << " moves!        ║\n╚════════════════════════════════════════════════════╝" << GameColors::reset << std::endl;
            break;
        }

        // Handle talk command - interact with NPCs
        if (command == "talk") {
            if (player.currentRoom->npc != nullptr && !player.currentRoom->npc->isDefeated) {
                std::cout << GameColors::bold << GameColors::cyan << player.currentRoom->npc->dialogue << GameColors::reset << std::endl;
            } else {
                std::cout << GameColors::bold << GameColors::red << "There is no one here to talk to." << GameColors::reset << std::endl;
            }
            continue;
        }

        // Handle fight command - battle monsters
        if (command == "fight") {
            if (player.currentRoom->npc != nullptr && 
                player.currentRoom->npc->type == MONSTER && 
                !player.currentRoom->npc->isDefeated) {
                if (player.canFight()) {
                    std::cout << GameColors::bold << GameColors::green << "You defeat the monster with your sword!" << GameColors::reset << std::endl;
                    player.currentRoom->npc->isDefeated = true;
                    player.addItem(KEY);
                    std::cout << GameColors::bold << GameColors::yellow << "You found a key!" << GameColors::reset << std::endl;
                } else {
                    std::cout << GameColors::bold << GameColors::red << "You need a sword to fight the monster!" << GameColors::reset << std::endl;
                }
            } else {
                std::cout << GameColors::bold << GameColors::red << "There is nothing to fight here." << GameColors::reset << std::endl;
            }
            continue;
        }

        // Handle take command - collect items
        if (command == "take") {
            if (player.currentRoom->item != NONE) {
                if (player.currentRoom->item == TREASURE && !player.hasItem(KEY)) {
                    std::cout << GameColors::bold << GameColors::red << "The chest is locked! You need a key." << GameColors::reset << std::endl;
                } else {   
                    ItemType item = player.currentRoom->item;
                    player.addItem(item);
                    player.currentRoom->item = NONE;
                    
                    // Display appropriate message based on item type
                    switch(item) {
                        case SWORD:
                            std::cout << GameColors::bold << GameColors::green << "You take the sword. Now you can fight monsters!" << GameColors::reset << std::endl;
                            break;
                        case KEY:
                            std::cout << GameColors::bold << GameColors::yellow << "You take the key." << GameColors::reset << std::endl;
                            break;
                        case TREASURE:
                            std::cout << GameColors::bold << GameColors::green << "You have taken the treasure!" << GameColors::reset << std::endl;
                            break;
                        default:
                            break;
                    }
                }
            } else {
                std::cout << GameColors::bold << GameColors::red << "There is nothing to take here." << GameColors::reset << std::endl;
            }
            continue;
        }

        // Handle movement commands
        Direction direction;
        bool validDirection = true;
        
        // Parse movement command
        if (command == "n" || command == "north") {
            direction = NORTH;
        } else if (command == "e" || command == "east") {
            direction = EAST;
        } else if (command == "s" || command == "south") {
            direction = SOUTH;
        } else if (command == "w" || command == "west") {
            direction = WEST;
        } else {
            std::cout << GameColors::bold << GameColors::red << "Unknown command. Try 'n', 'e', 's', or 'w'." << GameColors::reset << std::endl;
            validDirection = false;
        }

        // Execute movement if direction is valid
        if (validDirection) {
            if (player.currentRoom->canMove(direction)) {
                std::cout << GameColors::green << "You move " << player.currentRoom->directionToString(direction) << "." << GameColors::reset << std::endl;
                player.currentRoom = player.currentRoom->getConnections()[direction];
                player.moveCount++;  // Increment move counter when movement is successful
            } else {
                std::cout << GameColors::bold << GameColors::red << "You cannot go that way. Try another direction." << GameColors::reset << std::endl;
            }
        }
    }

    return 0;
}