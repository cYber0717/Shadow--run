#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>

using namespace std;
using namespace sf;

int main(){    

    // Create window
    RenderWindow window(VideoMode({1280u, 720u}), "Shadow Run");
    window.setFramerateLimit(60);
    RenderTexture gameScene({1280u, 720u});

    Music gameMusic;
    if (!gameMusic.openFromFile("assets/Full game.mp3"))
    {
        cerr << "Music loading failed!" << endl;
        return 1;    //unsuccessful exit
    }
    gameMusic.setLooping(true);
    gameMusic.play();

    //level timer and settings!!
    constexpr float levelDuration = 55.f;
    constexpr float levelExitX = 8750.f;
    constexpr float expectedClimbPause = 0.75f;
    constexpr float levelWorldSpeed =
        (levelExitX - 300.f) / (levelDuration - 2.f * expectedClimbPause - 0.25f);


    // Load images!!
    Texture runTexture, jumpTexture, climbTexture, slideTexture, idleTexture;
    Texture backgroundTexture;
    if (!runTexture.loadFromFile("assets/characters/run_sheet.png") ||
        !jumpTexture.loadFromFile("assets/characters/running_jump.png") ||
        !climbTexture.loadFromFile("assets/characters/climb_sheet.png") ||
        !slideTexture.loadFromFile("assets/characters/slide_sheet.png") ||
        !idleTexture.loadFromFile("assets/characters/idle_sheet.png") ||
        !backgroundTexture.loadFromFile("assets/background.png"))
    {
        cerr << "Image loading failed!" << endl;
        return 1;
    }

    //loads font bro....
    Font uiFont;
    if (!uiFont.openFromFile("assets/arial.ttf"))
    {
        cerr << "Font loading failed!" << endl;
        return 1;
    }

    //drone message text(font.message, size ) okay
    Text droneText(uiFont, "Drone is approaching", 24);
    droneText.setPosition({24.f, 19.f});
    droneText.setFillColor(Color::White);
    RectangleShape droneTextBackground({300.f, 40.f});
    droneTextBackground.setPosition({16.f, 16.f});
    droneTextBackground.setFillColor(Color(0, 0, 0, 180));


    auto centerText = [](Text& text, Vector2f center)
    {
        FloatRect bounds = text.getLocalBounds();
        text.setOrigin({
            bounds.position.x + bounds.size.x / 2.f,
            bounds.position.y + bounds.size.y / 2.f
        });
        text.setPosition(center);
    };

    RectangleShape menuBackground({1280.f, 720.f});
    menuBackground.setFillColor(Color(35, 18, 55));
    Text menuTitle(uiFont, "SHADOW RUN", 68);
    menuTitle.setFillColor(Color::White);
    centerText(menuTitle, {640.f, 220.f});
    Text menuSubtitle(uiFont, "DODGE THE OBSTACLES. ESCAPE THE DRONE.", 22);
    menuSubtitle.setFillColor(Color::White);
    centerText(menuSubtitle, {640.f, 282.f});

    RectangleShape startButton({320.f, 72.f});
    startButton.setPosition({480.f, 342.f});
    startButton.setFillColor(Color(76, 46, 99));
    startButton.setOutlineColor(Color::White);
    startButton.setOutlineThickness(2.f);
    Text startButtonText(uiFont, "START", 30);
    startButtonText.setFillColor(Color::White);
    centerText(startButtonText, {640.f, 378.f});

    RectangleShape closeButton({320.f, 72.f});
    closeButton.setPosition({480.f, 442.f});
    closeButton.setFillColor(Color(76, 46, 99));
    closeButton.setOutlineColor(Color::White);
    closeButton.setOutlineThickness(2.f);
    Text closeButtonText(uiFont, "CLOSE", 30);
    closeButtonText.setFillColor(Color::White);
    centerText(closeButtonText, {640.f, 478.f});
    //akhir wali screen ke sara display
    RectangleShape outcomeOverlay({1280.f, 720.f});
    outcomeOverlay.setFillColor(Color(24, 12, 36, 185));
    RectangleShape outcomePanel({1000.f, 300.f});
    outcomePanel.setPosition({140.f, 210.f});
    outcomePanel.setFillColor(Color(35, 18, 55, 240));
    outcomePanel.setOutlineColor(Color(255, 190, 120));
    outcomePanel.setOutlineThickness(3.f);
    RectangleShape outcomePanelShadow({1000.f, 300.f});
    outcomePanelShadow.setPosition({150.f, 220.f});
    outcomePanelShadow.setFillColor(Color(0, 0, 0, 180));
    //msg is empty bcz we dont know he won or lost
    Text outcomeTitle(uiFont, "", 64);
    outcomeTitle.setFillColor(Color(255, 190, 120));
    outcomeTitle.setOutlineColor(Color(25, 12, 35));
    outcomeTitle.setOutlineThickness(2.f);
    Text outcomeHint(uiFont, "R - restart    M - menu", 24);
    outcomeHint.setFillColor(Color(255, 190, 120));
    centerText(outcomeHint, {640.f, 430.f});

    //background;
    backgroundTexture.setRepeated(true);
    Sprite background(backgroundTexture);
    background.setTextureRect(IntRect({0, 0}, {1600, 900}));
    background.setScale({0.8f, 0.8f});

    // Create character;
    Sprite player(idleTexture);
    player.setScale({0.8f, 0.8f});

    // Create ground;
    RectangleShape ground({13000.f, 160.f});
    ground.setFillColor(Color::Black);
    ground.setPosition({0.f, 560.f});
    //obSTALCES
    constexpr size_t obstacleCount = 11;
    const float obstaclePositions[obstacleCount] = {
        900.f, 1500.f, 2200.f, 2900.f, 3600.f, 4300.f,
        5000.f, 5700.f, 6100.f, 7200.f, 7900.f
    };
    const bool obstacleIsSlide[obstacleCount] = {
        false, false, true, true, false, true,
        false, false, true, false, true
    };

    vector<RectangleShape> obstacles;
    obstacles.reserve(obstacleCount);
    for (size_t i = 0; i < obstacleCount; ++i)
    {
        bool isSlide = obstacleIsSlide[i];
        RectangleShape obstacle(isSlide ? Vector2f{60.f, 35.f}
                                        : Vector2f{30.f, 30.f});
        obstacle.setFillColor(Color::Black);
        obstacle.setPosition({obstaclePositions[i], isSlide ? 465.f : 530.f});
        obstacles.push_back(obstacle);
    }
    vector<FloatRect> obstacleHitboxes;
    obstacleHitboxes.reserve(obstacles.size());
     //climb full setting
    constexpr float climbObstacleHeight = 128.f;
    constexpr float climbApproachDistance = 50.f;
    RectangleShape climb1({60.f, climbObstacleHeight});
    RectangleShape climb2({60.f, climbObstacleHeight});

    climb1.setFillColor(Color::Black);
    climb2.setFillColor(Color::Black);

    climb1.setPosition({6500.f, 560.f - climbObstacleHeight});
    climb2.setPosition({8500.f, 560.f - climbObstacleHeight});

    RectangleShape exitMarker({8.f, 60.f});
    exitMarker.setFillColor(Color::Black);
    exitMarker.setOutlineColor(Color::White);
    exitMarker.setOutlineThickness(1.f);
    exitMarker.setPosition({levelExitX, 500.f});

    RectangleShape timerBarBackground({300.f, 18.f});
    timerBarBackground.setPosition({490.f, 20.f});
    timerBarBackground.setFillColor(Color::Black);
    timerBarBackground.setOutlineColor(Color::White);
    timerBarBackground.setOutlineThickness(1.f);

    RectangleShape timerBar({300.f, 18.f});
    timerBar.setPosition({490.f, 20.f});
    timerBar.setFillColor(Color(255, 190, 120));
    
    // Animation settings: start with idle;
    int columns = 4;
    int rows = 18;
    int totalFrames = 70;

    int frame = 0;
    float timer = 0.f;
    float frameTime = 0.067f;

    bool playing = true;
    bool loop = true;

    // Movement states;
    bool actionPlaying = false;
    bool normalAnimation = true;
    bool running = false;
    bool touchingJumpSlideObstacle = false;
    float droneWarningRemaining = 0.f;
    float droneFallingBehindRemaining = 0.f;
    bool playerCaptured = false;
    bool playerEscaped = false;
    float playerX = 300.f;
    float playerGroundY = 560.f;
    int climbingObstacleIndex = 0;
    int standingWallIndex = 0;
    bool fallingFromWall = false;

    // Previous keyboard states;
    bool rightWasPressed = false;
    bool spaceWasPressed = false;
    bool upWasPressed = false;
    bool downWasPressed = false;
    //level ki timing
    Clock clock;
    Clock levelClock;
    float backgroundOffset = 0.f;
    bool levelFinished = false;
    float levelFinishTime = 0.f;
    bool gameStarted = false;
    bool mouseWasPressed = false;
     
    auto resetGame = [&]()
    {
        columns = 4;
        rows = 18;
        totalFrames = 70;
        frame = 0;
        timer = 0.f;
        frameTime = 0.067f;
        playing = true;
        loop = true;

        actionPlaying = false;
        normalAnimation = true;
        running = false;
        touchingJumpSlideObstacle = false;
        droneWarningRemaining = 0.f;
        droneFallingBehindRemaining = 0.f;
        playerCaptured = false;
        playerEscaped = false;

        playerX = 300.f;
        playerGroundY = 560.f;
        climbingObstacleIndex = 0;
        standingWallIndex = 0;
        fallingFromWall = false;

        rightWasPressed = Keyboard::isKeyPressed(Keyboard::Key::Right);
        spaceWasPressed = Keyboard::isKeyPressed(Keyboard::Key::Space);
        upWasPressed = Keyboard::isKeyPressed(Keyboard::Key::Up);
        downWasPressed = Keyboard::isKeyPressed(Keyboard::Key::Down);

        backgroundOffset = 0.f;
        ground.setPosition({0.f, 560.f});
        for (size_t i = 0; i < obstacles.size(); ++i)
        {
            obstacles[i].setPosition({
                obstaclePositions[i], obstacleIsSlide[i] ? 465.f : 530.f
            });
        }
        climb1.setPosition({6500.f, 560.f - climbObstacleHeight});
        climb2.setPosition({8500.f, 560.f - climbObstacleHeight});
        exitMarker.setPosition({levelExitX, 500.f});
        timerBar.setSize({300.f, 18.f});

        background.setTextureRect(IntRect({0, 0}, {1600, 900}));
        player.setTexture(idleTexture);
        Vector2u idleSize = idleTexture.getSize();
        int idleFrameWidth = idleSize.x / 4;
        int idleFrameHeight = idleSize.y / 18;
        player.setTextureRect(IntRect(
            {0, 0}, {idleFrameWidth, idleFrameHeight}));
        player.setOrigin({idleFrameWidth / 2.f, idleFrameHeight * 0.9f});
        player.setPosition({playerX, playerGroundY});

        droneText.setString("Drone is approaching");
        levelFinished = false;
        levelFinishTime = 0.f;
        clock.restart();
        levelClock.restart();
    };

    