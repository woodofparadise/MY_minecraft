#include "game.h"

using namespace std;

Game::Game(bool& gameState, int seed)
{
    // 初始化世界种子
    this->seed = seed;

    // 设置长、宽、窗口名称
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "MY_MINECRAFT", NULL, NULL);
    if(window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        gameState = false;
        return ;
    }
    glfwMakeContextCurrent(window); // 将我们窗口的上下文设置为当前线程的主上下文
    glfwSetWindowUserPointer(window, this); // 将游戏类的指针关联到游戏窗口上，方便窗口输入相关的静态函数访问

    set_wondow_properties();

    // 给GLAD传入用于加载系统相关OpenGL函数指针地址的函数
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Fail to initialized GLAD" << std::endl;
        gameState = false;
        return ;
    }

    // 初始化着色器
    selectionShader.init_shader("./shaders/selectionShader.vs", "./shaders/selectionShader.fs");
    blockShader.init_shader("./shaders/blockShader.vs", "./shaders/blockShader.fs");
    HUDShader.init_shader("./shaders/HUDShader.vs", "./shaders/HUDShader.fs");
    textShader.init_shader("./shaders/textShader.vs", "./shaders/textShader.fs");

    // 初始化文字渲染器（使用系统字体）
    if (!textRenderer.init("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 24))
    {
        std::cout << "Warning: Failed to initialize text renderer" << std::endl;
    }

    // 初始化人物和地形
    blockShader.use();

    player.upload_data("./Textures/steve.png");
    terrain.init_terrain(this->seed, player.position, "./Textures/DefaultPack.png");
    terrain.bind_block_texture(blockShader);
    player.bind_player_texture(blockShader);
    player.set_position(glm::vec3(0.5f, terrain.get_height(player.position), 0.5f));
    blockShader.set_int("blockTexture", 1);
    blockShader.set_int("playerTexture", 2);

    HUDShader.use();
    HUDitems.resize(1);
    HUDitems[0].set_HUDitem(25.0f, 25.0f, glm::vec2(SCR_WIDTH/2.0f, SCR_HEIGHT/2.0f), "./Textures/cursor.png");
    HUDitems[0].bind_item_texture(HUDShader, 3);
    HUDShader.set_int("cursorTexture", 3);
    toolbar.set_toolbar();
    toolbar.bind_texture(HUDShader, 4);
    cout << "initialize success" << endl;
}

// 设置窗口属性，绑定发生窗口事件时调用的函数
void Game::set_wondow_properties()
{
    // 注册帧缓冲大小函数，告诉GLFW每当窗口调整大小的时候调用该函数
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // 注册鼠标滚轮回调函数
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetScrollCallback(window, scroll_callback);

    // 注册鼠标事件回调函数
    glfwSetCursorPosCallback(window, mouse_callback);

    // 注册鼠标点击事件回调函数
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    // 注册键盘按键事件回调函数
    glfwSetKeyCallback(window, key_callback);
}

// 游戏主循环
void Game::game_loop()
{
    // 渲染循环，在每次循环开始前检查一次GLFW是否被要求退出
    while(!glfwWindowShouldClose(window))
    {
        // 输入
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // 计算FPS
        frameCount++;
        fpsAccumulator += deltaTime;
        if (fpsAccumulator >= fpsUpdateInterval)
        {
            currentFPS = frameCount / fpsAccumulator;
            frameCount = 0;
            fpsAccumulator = 0.0f;
            displayVertices = terrain.drawnVertices;
            displayTriangles = terrain.drawnTriangles;
        }
        glfwPollEvents(); // 处理鼠标/键盘事件，更新摄像机方向
        process_input(window); // 在每一帧检测窗口是否返回
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // 设置清空屏幕所用的颜色
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 清除颜色缓冲和深度缓冲

        glm::mat4 view = player.camera.get_view_matrix();                // 观察矩阵
        glm::mat4 projection = player.camera.get_projection_matrix();    // 投影矩阵

        glm::mat4 vpMatrix = projection * view;                            // VP矩阵用于视锥体剔除

        blockShader.use();
        player.update_position(terrain, deltaTime);
        terrain.update_terrain(player.camera.cameraPos, &vpMatrix);       // 更新地形（带视锥剔除）
        blockShader.set_mat4("view", view);
        blockShader.set_mat4("projection", projection);
        terrain.draw_terrain(blockShader, vpMatrix, player.camera.cameraPos); // 绘制地形（带视锥剔除+透明排序）
        if(!player.cameraMode)
        {
            player.draw_player(blockShader);
        }

        // 检测选中的方块
        blockSelected = false;
        if (raycast_step(player.camera.cameraPos, player.camera.cameraFront, 4.0f, terrain, selectedBlock, lastHitBlock))
        {
            // 渲染选中效果
            render_selection_box(selectedBlock, selectionShader, projection, view);
            blockSelected = true;
        }

        // 渲染HUD元素
        for(int i = 0; i < HUDitems.size(); i++)
        {
            HUDitems[i].draw_item(HUDShader);
        }
        toolbar.draw_toolbar(HUDShader);

        // 渲染FPS和统计信息
        if (textRenderer.isInitialized)
        {
            std::stringstream ss;
            ss << "FPS: " << std::fixed << std::setprecision(1) << currentFPS;
            textRenderer.renderText(textShader, ss.str(), SCR_WIDTH - 120.0f, SCR_HEIGHT - 30.0f, 0.8f, glm::vec3(1.0f, 1.0f, 1.0f));

            std::stringstream ssVert;
            ssVert << "Verts: " << displayVertices;
            textRenderer.renderText(textShader, ssVert.str(), SCR_WIDTH - 130.0f, SCR_HEIGHT - 52.0f, 0.5f, glm::vec3(0.8f, 0.8f, 0.8f));

            std::stringstream ssTri;
            ssTri << "Tris:  " << displayTriangles;
            textRenderer.renderText(textShader, ssTri.str(), SCR_WIDTH - 130.0f, SCR_HEIGHT - 68.0f, 0.5f, glm::vec3(0.8f, 0.8f, 0.8f));
        }

        // 交换缓冲，重置光标到屏幕中心
        glfwSwapBuffers(window);
        glfwSetCursorPos(window, SCR_WIDTH/2, SCR_HEIGHT/2);
        // break;
    }
    return ;
}

// 释放资源
void Game::clear()
{
    player.clear();
    terrain.clear();
    for(int i = 0; i < HUDitems.size(); i++)
    {
        HUDitems[i].clear();
    }
    toolbar.clear();
    textRenderer.clear();
    glfwDestroyWindow(window);
    cout << "Quiting game..." << endl;
}

void Game::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void Game::mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    Game* game = static_cast<Game*>(glfwGetWindowUserPointer(window));
    if (!game)
    {
        cout << "handle mouse movement error" << endl;
        return ;
    }
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);
    // cout << xpos << " " << ypos << endl;

    // player.camera.processMouseMovement1(xpos, ypos);

    // if(firstMouse)
    // {
    //     firstMouse = false;
    //     lastX = xpos;
    //     lastY = ypos;
    // }
    float xoffset = xpos - game->lastX;
    float yoffset = game->lastY - ypos;

    game->lastX = xpos;
    game->lastY = ypos;

    game->player.camera.process_mouse_movement_2(xoffset, yoffset);
}

void Game::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    Game* game = static_cast<Game*>(glfwGetWindowUserPointer(window));
    if (!game)
    {
        cout << "handle scroll callback error" << endl;
        return ;
    }
    game->player.camera.process_mouse_scroll(static_cast<float>(yoffset));
}

void Game::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    Game* game = static_cast<Game*>(glfwGetWindowUserPointer(window));
    // 只响应按下事件（不响应释放和重复）
    if (action == GLFW_PRESS)
    {
        if (key >= GLFW_KEY_1 && key <= GLFW_KEY_9)
        {
            game->toolbar.selectedBlock = key - GLFW_KEY_0 - 1;
        }
        if(key == GLFW_KEY_R)
        {
            game->player.switch_camera_mode();
        }
        if(key == GLFW_KEY_SPACE)
        {
            game->player.jump();
        }
        if(key == GLFW_KEY_ESCAPE)
        {
            glfwSetWindowShouldClose(window, true);
        }
    }
}

void Game::process_input(GLFWwindow *window)
{
    Game* game = static_cast<Game*>(glfwGetWindowUserPointer(window));
    if (!game)
    {
        cout << "handle keyboard input error" << endl;
        return ;
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        game->player.move(FORWARD);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        game->player.move(BACKWARD);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        game->player.move(LEFT);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        game->player.move(RIGHT);
}

void Game::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    Game* game = static_cast<Game*>(glfwGetWindowUserPointer(window));
    if (!game)
    {
        cout << "handle mouse input error" << endl;
        return ;
    }
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS && game->blockSelected)
    {
        // 破坏方块
        game->terrain.destroy_block(game->selectedBlock);
    }
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && game->blockSelected && game->toolbar.toolbarBlock[game->toolbar.selectedBlock] != AIR && !is_overlap_with_player(game->player.position, game->lastHitBlock))
    {
        // 放置方块
        game->terrain.create_block(game->lastHitBlock, game->toolbar.toolbarBlock[game->toolbar.selectedBlock]);
    }
}
