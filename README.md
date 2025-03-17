# Text Adventure Game

A colorful command-line text adventure game written in C++. Journey through a mystical realm called Eldara, where you'll explore forests, ruins, caves, and mountains while searching for legendary treasure.

## Features

- Rich, colorful text-based interface using ANSI color codes
- Multiple interconnected locations to explore
- Interactive NPCs (villagers and monsters)
- Items to collect (sword, key, treasure)
- Simple combat system
- Detailed environment descriptions
- Movement tracking

## How to Play

1. Compile the game:
```bash
g++ -std=c++11 -o AdventureGame capstone.cpp
```

2. Run the game:
```bash
./adventure
```

## Commands

- `n`, `s`, `e`, `w`: Move in different directions
- `look`: Get a detailed description of your current location
- `talk`: Speak with characters
- `fight`: Battle monsters (requires sword)
- `take`: Pick up items
- `help`: Show available commands
- `quit`: Exit the game

## Game World

The game world consists of several interconnected locations:
- Village: Starting point with helpful NPCs
- Forest: Dark and mysterious woods
- Ruins: Ancient civilization remains
- Cave: Home to fearsome monsters
- Mountain: Elevated vantage point
- Valley: Peaceful transition area
- Lake: Reflective water body
- Hidden Room: Secret location with treasure 