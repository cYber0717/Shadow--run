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

    // Animation settings
    int columns = 4;
    int rows = 6;
    int totalFrames = 22;

    int frame = 0;
    float timer = 0.f;
    float frameTime = 0.067f;

    bool playing = false;
    bool loop = true;

    // Previous keyboard states
    bool rightWasPressed = false;
    bool spaceWasPressed = false;
    bool upWasPressed = false;
    bool downWasPressed = false;

    Clock clock;

    while (window.isOpen())
    {
        // Window events
        while (const optional event = window.pollEvent())
        {
            if (event.value().is<Event::Closed>())
                window.close();
        }

        // Check keyboard
        bool right = Keyboard::isKeyPressed(Keyboard::Key::Right);
        bool space = Keyboard::isKeyPressed(Keyboard::Key::Space);
        bool up = Keyboard::isKeyPressed(Keyboard::Key::Up);
        bool down = Keyboard::isKeyPressed(Keyboard::Key::Down);

        // Running
        if (right && !rightWasPressed)
        {
            player.setTexture(runTexture);
            columns = 4;
            rows = 6;
            totalFrames = 22;
            frameTime = 0.067f;
            loop = true;
            frame = 0;
            timer = 0.f;
            playing = true;
        }

        // Jumping
        else if (space && !spaceWasPressed)
        {
            player.setTexture(jumpTexture);
            columns = 4;
            rows = 4;
            totalFrames = 14;
            frameTime = 0.067f;
            loop = false;
            frame = 0;
            timer = 0.f;
            playing = true;
        }

        // Climbing
        else if (up && !upWasPressed)
        {
            player.setTexture(climbTexture);
            columns = 4;
            rows = 7;
            totalFrames = 28;
            frameTime = 0.04f;
            loop = false;
            frame = 0;
            timer = 0.f;
            playing = true;
        }

        // Sliding
        else if (down && !downWasPressed)
        {
            player.setTexture(slideTexture);
            columns = 4;
            rows = 12;
            totalFrames = 24;
            frameTime = 0.067f;
            loop = false;
            frame = 0;
            timer = 0.f;
            playing = true;
        }

        // Remember previous key states
        rightWasPressed = right;
        spaceWasPressed = space;
        upWasPressed = up;
        downWasPressed = down;

        // Animation timing
        float dt = clock.restart().asSeconds();

        if (playing)
        {
            timer += dt;

            if (timer >= frameTime)
            {
                timer -= frameTime;
                frame++;

                if (frame >= totalFrames)
                {
                    if (loop)
                        frame = 0;
                    else
                    {
                        frame = totalFrames - 1;
                        playing = false;
                    }
                }
            }
        }

        // Calculate frame position
        Vector2u size = player.getTexture().getSize();

        int frameWidth = size.x / columns;
        int frameHeight = size.y / rows;

        int x = (frame % columns) * frameWidth;
        int y = (frame / columns) * frameHeight;

        player.setTextureRect(IntRect({ x, y },
                                      { frameWidth, frameHeight }));

        // Position character
        player.setOrigin({ frameWidth / 2.f, frameHeight * 0.9f });
        player.setPosition({ 400.f, 500.f });

        // Draw everything
        window.clear(Color(110, 140, 190));

        window.draw(ground);
        window.draw(player);

        window.display();
    }

    return 0;
}
