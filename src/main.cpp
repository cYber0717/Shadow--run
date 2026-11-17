#include <SFML/Graphics.hpp>
#include <iostream>
                      
using namespace std;
using namespace sf;      

int main()
{
    // Create window
    RenderWindow window(VideoMode({1280u, 720u}), "Shadow Run");
    window.setFramerateLimit(60);

    // Load images
    Texture runTexture, jumpTexture, climbTexture, slideTexture;

    if (!runTexture.loadFromFile("assets/characters/run_sheet.png") ||
        !jumpTexture.loadFromFile("assets/characters/running_jump.png") ||
        !climbTexture.loadFromFile("assets/characters/climb_sheet.png") ||
        !slideTexture.loadFromFile("assets/characters/slide_sheet.png"))
    {
        cout << "Image loading failed!";
        return 1;
    }

    // Create character
    Sprite player(runTexture);
    player.setScale({2.f, 2.f});

    // Create ground
    RectangleShape ground({1280.f, 4.f});
    ground.setPosition({0.f, 500.f});
    ground.setFillColor(Color(40, 60, 90));
}