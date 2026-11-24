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

    while (window.isOpen())
    {
        // Window events
        while (const optional event = window.pollEvent())
        {
            if (event.value().is<Event::Closed>())
                window.close();
        }

        if (!window.isOpen())
            break;

        bool mousePressed = Mouse::isButtonPressed(Mouse::Button::Left);
        bool mouseClicked = mousePressed && !mouseWasPressed;
        mouseWasPressed = mousePressed;

        if (!gameStarted)
        {
            Vector2i mousePosition = Mouse::getPosition(window);
            bool overStart =
                mousePosition.x >= 480 && mousePosition.x <= 800 &&
                mousePosition.y >= 342 && mousePosition.y <= 414;
            bool overClose =
                mousePosition.x >= 480 && mousePosition.x <= 800 &&
                mousePosition.y >= 442 && mousePosition.y <= 514;

            startButton.setFillColor(overStart
                ? Color(105, 65, 135) : Color(76, 46, 99));
            closeButton.setFillColor(overClose
                ? Color(105, 65, 135) : Color(76, 46, 99));

            if (mouseClicked && overStart)
            {
                resetGame();
                gameStarted = true;
            }
            else if (mouseClicked && overClose)
            {
                window.close();
            }

            if (!window.isOpen())
                break;

            window.clear();
            window.draw(menuBackground);
            window.draw(menuTitle);
            window.draw(menuSubtitle);
            window.draw(startButton);
            window.draw(startButtonText);
            window.draw(closeButton);
            window.draw(closeButtonText);
            window.display();
            continue;
        }

        if (levelFinished && Keyboard::isKeyPressed(Keyboard::Key::R))
        {
            resetGame();
            continue;
        }
        if (levelFinished && Keyboard::isKeyPressed(Keyboard::Key::M))
        {
            resetGame();
            gameStarted = false;
            continue;
        }

        // Check keyboard
        bool right = Keyboard::isKeyPressed(Keyboard::Key::Right);
        bool space = Keyboard::isKeyPressed(Keyboard::Key::Space);
        bool up = Keyboard::isKeyPressed(Keyboard::Key::Up);
        bool down = Keyboard::isKeyPressed(Keyboard::Key::Down);

        // CLIMB FIX: Check the wall approach window without allowing a wall already behind the player.
        float playerFront = playerX + player.getGlobalBounds().size.x * 0.075f;
        FloatRect climb1ApproachBounds = climb1.getGlobalBounds();
        FloatRect climb2ApproachBounds = climb2.getGlobalBounds();
        float climb1Distance = climb1ApproachBounds.position.x - playerFront;
        float climb2Distance = climb2ApproachBounds.position.x - playerFront;
        bool climb1InRange =
            climb1Distance >= 0.f && climb1Distance <= climbApproachDistance;
        bool climb2InRange =
            climb2Distance >= 0.f && climb2Distance <= climbApproachDistance;
        int nearbyClimbObstacle = 0;
        if (climb1InRange)
            nearbyClimbObstacle = 1;
        if (climb2InRange &&
            (!climb1InRange || climb2Distance < climb1Distance))
            nearbyClimbObstacle = 2;

        // Jumping
        if (!levelFinished && !actionPlaying && space && !spaceWasPressed)
        {
            player.setTexture(jumpTexture);
            columns = 4;
            rows = 4;
            totalFrames = 14;
            frameTime = 0.067f;
            loop = false;

            actionPlaying = true;
            normalAnimation = false;
            frame = 0;
            timer = 0.f;
            playing = true;
        }

        else if (!levelFinished && !actionPlaying && standingWallIndex == 0 &&
                 !fallingFromWall && playerGroundY == 560.f && up && !upWasPressed &&
                 nearbyClimbObstacle != 0)
        {
            climbingObstacleIndex = nearbyClimbObstacle;
            player.setTexture(climbTexture);
            columns = 4;
            rows = 7;
            totalFrames = 28;
            frameTime = 0.04f;
            loop = false;

            actionPlaying = true;
            normalAnimation = false;
            frame = 0;
            timer = 0.f;
            playing = true;
        }

        // Sliding
        else if (!levelFinished && !actionPlaying && down && !downWasPressed)
        {
            player.setTexture(slideTexture);
            columns = 4;
            rows = 12;
            totalFrames = 24;
            frameTime = 0.067f;
            loop = false;

            actionPlaying = true;
            normalAnimation = false;
            frame = 0;
            timer = 0.f;
            playing = true;
        }

        // Remember previous key states
        rightWasPressed = right;
        spaceWasPressed = space;
        upWasPressed = up;
        downWasPressed = down;

        // Return to running or idle
        if (!levelFinished && !actionPlaying)
        {
            if (right && (!normalAnimation || !running))
            {
                player.setTexture(runTexture);
                columns = 4;
                rows = 6;
                totalFrames = 22;
                frameTime = 0.05f;
                loop = true;

                frame = 0;
                timer = 0.f;
                playing = true;

                running = true;
                normalAnimation = true;
            }
            else if (!right && (!normalAnimation || running))
            {
                player.setTexture(idleTexture);
                columns = 4;
                rows = 18;
                totalFrames = 70;
                frameTime = 0.067f;
                loop = true;

                frame = 0;
                timer = 0.f;
                playing = true;

                running = false;
                normalAnimation = true;
            }
        }

        bool climbing = actionPlaying && &player.getTexture() == &climbTexture;
        bool jumping = actionPlaying && &player.getTexture() == &jumpTexture;
        float climbLift = climbing ? climbObstacleHeight * frame / 27.f : 0.f;

        // Animation timing
        float dt = clock.restart().asSeconds();
        if (!levelFinished && droneWarningRemaining > 0.f)
        {
            droneWarningRemaining -= dt;
            if (droneWarningRemaining <= 0.f)
            {
                droneWarningRemaining = 0.f;
                droneFallingBehindRemaining = 1.f;
                droneText.setString("Drone is falling behind");
            }
        }
        else if (!levelFinished && droneFallingBehindRemaining > 0.f)
        {
            droneFallingBehindRemaining -= dt;
            if (droneFallingBehindRemaining < 0.f)
                droneFallingBehindRemaining = 0.f;
        }

       if (!levelFinished && right && !climbing && playerX < 300.f)
        {
            playerX += levelWorldSpeed * dt;
            if (playerX > 300.f)
                playerX = 300.f;
        }
        else if (!levelFinished &&
                 (right ||
                  (climbing &&
                   ((climbingObstacleIndex == 1 && climb1Distance > 0.f) ||
                    (climbingObstacleIndex == 2 && climb2Distance > 0.f)))))
        {
        float worldMove = levelWorldSpeed * dt;
        float baseHitboxBottom =
            player.getGlobalBounds().position.y +
            player.getGlobalBounds().size.y * 0.8f;
        float playerLeft = playerX - player.getGlobalBounds().size.x * 0.075f;
        float climb1Top = climb1.getGlobalBounds().position.y;
        float climb2Top = climb2.getGlobalBounds().position.y;
        bool canPassClimb1 =
            (climbing && climbingObstacleIndex == 1 &&
             baseHitboxBottom - climbLift <= climb1Top) ||
            standingWallIndex == 1;
        bool canPassClimb2 =
            (climbing && climbingObstacleIndex == 2 &&
             baseHitboxBottom - climbLift <= climb2Top) ||
            standingWallIndex == 2;

        if (!canPassClimb1 && climb1Distance >= 0.f && climb1Distance < worldMove)
            worldMove = climb1Distance > 0.f ? climb1Distance : 0.f;
        if (!canPassClimb2 && climb2Distance >= 0.f && climb2Distance < worldMove)
            worldMove = climb2Distance > 0.f ? climb2Distance : 0.f;

         // Background movement
         backgroundOffset += worldMove * (100.f / levelWorldSpeed);

        background.setTextureRect(
        IntRect({int(backgroundOffset), 0}, {1600, 900}));

      
        // move ground;
        ground.setPosition({
        ground.getPosition().x - worldMove, 560.f
        });

        // Move jump and slide obstacles.
        for (size_t i = 0; i < obstacles.size(); ++i)
        {
            float obstacleY = obstacleIsSlide[i] ? 465.f : 530.f;
            obstacles[i].setPosition({
                obstacles[i].getPosition().x - worldMove, obstacleY
            });
        } 

        climb1.setPosition({
        climb1.getPosition().x - worldMove, 560.f - climbObstacleHeight
        });

        climb2.setPosition({
        climb2.getPosition().x - worldMove, 560.f - climbObstacleHeight
        });

        exitMarker.setPosition({
        exitMarker.getPosition().x - worldMove, 500.f
        });

        if (standingWallIndex == 1 &&
            climb1.getPosition().x + climb1.getSize().x <= playerLeft)
        {
            standingWallIndex = 0;
            fallingFromWall = true;
        }
        else if (standingWallIndex == 2 &&
                 climb2.getPosition().x + climb2.getSize().x <= playerLeft)
        {
            standingWallIndex = 0;
            fallingFromWall = true;
        }
    }

        if (!levelFinished && fallingFromWall)
        {
            playerGroundY += 900.f * dt;
            if (playerGroundY >= 560.f)
            {
                playerGroundY = 560.f;
                fallingFromWall = false;
            }
        }

        if (!levelFinished && playing)
        {
            timer += dt;

            if (timer >= frameTime)
            {
                timer -= frameTime;
                if (climbing)
                {
                    if (frame < totalFrames - 1)
                    {
                        frame++;
                    }
                    else
                    {
                        FloatRect selectedWall = climbingObstacleIndex == 1
                            ? climb1.getGlobalBounds() : climb2.getGlobalBounds();
                        float playerLeft =
                            playerX - player.getGlobalBounds().size.x * 0.075f;
                        bool wallCleared =
                            selectedWall.position.x + selectedWall.size.x <= playerLeft;
                        if (!right || wallCleared)
                        {
                            playing = false;
                            actionPlaying = false;
                            playerGroundY = 560.f - climbObstacleHeight;
                            if (wallCleared)
                                fallingFromWall = true;
                            else
                                standingWallIndex = climbingObstacleIndex;
                            climbingObstacleIndex = 0;
                        }
                    }
                }
                else
                {
                    frame++;
                    if (frame >= totalFrames)
                    {
                        if (loop)
                            frame = 0;
                        else
                        {
                            frame = totalFrames - 1;
                            playing = false;
                            actionPlaying = false;
                        }
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

        player.setTextureRect(IntRect({x, y},
                                      {frameWidth, frameHeight}));


        player.setOrigin({frameWidth / 2.f, frameHeight * 0.9f});
        player.setPosition({playerX, climbing ? 560.f : playerGroundY});

        float levelElapsed = levelClock.getElapsedTime().asSeconds();
        if (!levelFinished)
        {
            if (playerX >= exitMarker.getPosition().x && levelElapsed <= levelDuration)
            {
                levelFinished = true;
                playerEscaped = true;
                levelFinishTime = levelElapsed;
                cout << "He escaped!" << endl;
            }
            else if (levelElapsed >= levelDuration)
            {
                levelFinished = true;
                playerCaptured = true;
                levelFinishTime = levelElapsed;
                cout << "He was captured." << endl;
            }
        }

        float displayedElapsed = levelFinished ? levelFinishTime : levelElapsed;
        float timeLeft = levelDuration - displayedElapsed;
        if (timeLeft < 0.f)
            timeLeft = 0.f;
        timerBar.setSize({300.f * timeLeft / levelDuration, 18.f});

        
        FloatRect playerBounds = player.getGlobalBounds();
        bool sliding = actionPlaying && &player.getTexture() == &slideTexture;
        climbLift = climbing ? climbObstacleHeight * frame / 27.f : 0.f;

        static const float slideH[24] = {82,82,76,64,50,38,28,26,30,28,32,34,
                                         38,48,54,58,64,68,72,74,80,80,80,82};
        static const float jumpRise[14] = {0,4,8,13,18,22,24,24,22,18,13,8,4,0};

        FloatRect playerHitbox;
        if (sliding)
        {
            if (frame < 0)
                frame = 0;
            else if (frame > 23)
                frame = 23;

            float w = 50.f;
            float h = slideH[frame];
            playerHitbox = FloatRect({playerX - w / 2, playerGroundY - h}, {w, h});
        }
        else
        {
            playerHitbox = FloatRect(
                { playerBounds.position.x + playerBounds.size.x * 0.425f,
                  playerBounds.position.y + playerBounds.size.y * 0.30f },
                { playerBounds.size.x * 0.15f,
                  playerBounds.size.y * 0.50f });

            if (jumping)
            {
                if (frame < 0)
                    frame = 0;
                else if (frame > 13)
                    frame = 13;

                playerHitbox.position.y -= jumpRise[frame];
            }
            else if (climbing)
            {
                playerHitbox.position.y -= climbLift;
            }
        }

        obstacleHitboxes.clear();
        for (size_t i = 0; i < obstacles.size(); ++i)
        {
            FloatRect obstacleBounds = obstacles[i].getGlobalBounds();
            float inset = obstacleIsSlide[i] ? 0.025f : 0.05f;
            float hitboxScale = obstacleIsSlide[i] ? 0.95f : 0.90f;
            float verticalInset = obstacleIsSlide[i] ? 0.025f : 0.10f;
            FloatRect obstacleHitbox(
                {
                    obstacleBounds.position.x + obstacleBounds.size.x * inset,
                    obstacleBounds.position.y + obstacleBounds.size.y * verticalInset
                },
                {
                    obstacleBounds.size.x * hitboxScale,
                    obstacleBounds.size.y * hitboxScale
                }
            );
            obstacleHitboxes.push_back(obstacleHitbox);
        }

        bool obstacleCollision = false;
        for (size_t i = 0; i < obstacleHitboxes.size(); ++i)
        {
            if (playerHitbox.findIntersection(obstacleHitboxes[i]))
            {
                obstacleCollision = true;
                break;
            }
        }

        bool newObstacleCollision =
            obstacleCollision && !touchingJumpSlideObstacle;
        touchingJumpSlideObstacle = obstacleCollision;

        if (!levelFinished && newObstacleCollision)
        {
            if (droneWarningRemaining > 0.f)
            {
                playerCaptured = true;
                levelFinished = true;
                levelFinishTime = levelElapsed;
                droneText.setString("You were captured!");
                cout << "He was captured." << endl;
            }
            else
            {
                droneWarningRemaining = 4.f;
                droneFallingBehindRemaining = 0.f;
                droneText.setString("Drone is approaching");
            }
        }

        // Draw everything
        gameScene.clear();
        gameScene.draw(background);
        gameScene.draw(ground);
        for (const RectangleShape& obstacle : obstacles)
            gameScene.draw(obstacle);
        gameScene.draw(climb1);
        gameScene.draw(climb2);
        gameScene.draw(exitMarker);
        gameScene.draw(player);
        gameScene.draw(timerBarBackground);
        gameScene.draw(timerBar);
        if (droneWarningRemaining > 0.f ||
            droneFallingBehindRemaining > 0.f || playerCaptured)
        {
            gameScene.draw(droneTextBackground);
            gameScene.draw(droneText);
        }
        gameScene.display();

        window.clear(Color(20, 10, 30));
        if (levelFinished)
        {
            Sprite gameFrame(gameScene.getTexture());
            window.draw(gameFrame);
            window.draw(outcomeOverlay);
            window.draw(outcomePanelShadow);
            window.draw(outcomePanel);
            outcomeTitle.setString(
                playerEscaped ? "YOU ESCAPED!" : "PLAYER WAS CAPTURED");
            centerText(outcomeTitle, {640.f, 340.f});
            window.draw(outcomeTitle);
            window.draw(outcomeHint);
        }
        else
        {
            Sprite gameFrame(gameScene.getTexture());
            window.draw(gameFrame);
        }
        window.display();
    }
    gameMusic.stop();
    return 0;
}





