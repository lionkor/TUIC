/* This example showcases how to use a panel that uses JTUI_DETAIL_G8_C4 detail mode. This detail mode has 8 bits per glyph and 4 bits per color. This allows for up to 256 unique glyphs and for a palette of 16 colors. */

#include <gl/glew.h>
#include <GLFW/glfw3.h>

#include <TUIC/tuic.h>
#include <TUIC/backends/opengl33.h>

#include <stdio.h>


const size_t kTilesWide = 100;
const size_t kTilesTall = 80;
const size_t kTileSizeMultiplier = 1;
const size_t kTilePixelWidth = 8;
const size_t kTilePixelHeight = 8;
const char* kWindowTitle = "Example 1: G8_C4 Detail";
const char* kSheetImageFile = "cp_8x8_rgba_bg_alpha.png";
const size_t kSheetTextureFormat = 4;
const size_t kSheetChannels = 4;
const size_t kSheetBlendMode = TUI_BLEND_BG_ALPHA;
const size_t kAtlasMode = TUI_ATLAS_GRID;
const size_t kTuiDetail = TUI_DETAIL_G8_C4_FULL;


void TuiMessageCallback(int error_code, const char* msg)
{
    if (error_code == TUI_ERROR_BACKEND_SPECIFIC)
    {
        printf("%s : %s backend message: %s\n", tuiErrorCodeToString(error_code), tuiErrorCodeGetDescription(error_code), msg);
    }
    else
    {
        printf("%s : %s location: %s\n", tuiErrorCodeToString(error_code), tuiErrorCodeGetDescription(error_code), msg);
    }
}

int main()
{
    size_t window_width = kTilesWide * kTilePixelWidth * kTileSizeMultiplier;
    size_t window_height = kTilesTall * kTilePixelHeight * kTileSizeMultiplier;

    /* GLFW and GLEW Initialization */
        if (!glfwInit())
    {
        printf("GLFW failed to initialize.\n");
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    GLFWwindow* window = glfwCreateWindow(window_width, window_height, kWindowTitle, NULL, NULL);
    if (window == NULL)
    {
        printf("Failed to create GLFW window.\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(0); // Change to 1 to cap framerate with your monitor's vsync.

    /* TUI Object Declaration */
    TuiInstance instance;
    TuiGlyphAtlas atlas;
    TuiPalette palette;
    TuiPanel panel;
    TuiBatch batch;

    /* Tell TUIC to send all debug messages to our message callback (the function we implemented above main). */
    tuiSetDebugErrorCallback(TuiMessageCallback);

    /* Create the TUIC instance. */
    instance = tuiOpengl33InstanceCreate(window_width, window_height, ((void*)glfwGetProcAddress));

    /* Load the atlas image */
    TuiImage image = tuiImageLoad(kSheetImageFile, kSheetChannels);
    if (image == NULL)
    {
        printf("Issue loading image file. Make sure that it was moved from the content folder to a location the executable can read. You need to do this manually.\n");
        glfwTerminate();
        return -1;
    }

    /* Create the Glyph Atlas */
    atlas = tuiGlyphAtlasCreateCodepageGrid(instance, image, kSheetBlendMode);
    tuiImageDestroy(image);

    /* Create the palette */
    palette = tuiPaletteCreateXterm(instance, 16);

    /* Create the panel */
    panel = tuiPanelCreate(instance, window_width, window_height);

    /* Create the batch */
    batch = tuiBatchCreate(kTuiDetail, kTilesWide, kTilesTall);

    //Fill the batch with random data.
    time_t t;
    srand((unsigned)time(&t));
    for (int x = 0; x < kTilesWide; x++)
    {
        for (int y = 0; y < kTilesTall; y++)
        {
            tuiBatchSetTile_G8_C4_FULL(batch, x, y, rand() % 256, rand() % 256);
        }
    }

    //Render the batch to the panel
    tuiPanelDrawBatch(panel, atlas, palette, batch);

    //Render panel to a png image (Basic-Panel.png)
    TuiImage panel_image = tuiPanelGetImage(panel, NULL);
    tuiImageSave(panel_image, "Basic-Panel.png");
    tuiImageDestroy(panel_image);

    //fps tracking setup
    double last_time = 0;
    int nbFrame = 0;

    //Render loop
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents(); //handle input events and call callback functions

        /* Print fps to console */
        double currentTime = glfwGetTime();
        double delta = currentTime - last_time;
        nbFrame++;
        if (delta >= 1.0) { // If last fps print was more than 1 sec ago

            double fps = nbFrame / delta;
            printf("%f FPS\n", fps);
            nbFrame = 0;
            last_time = currentTime;
        }

        //Try commenting out the clear function, the tile setting loop, and/or the draw batch functions to see impact on performance.
        tuiBatchClear(batch); 
        for (int x = 0; x < kTilesWide; x++)
        {
            for (int y = 0; y < kTilesTall; y++)
            {
                int color = tuiClassicColorCombine(x % 16, 15 - (y % 16));
                tuiBatchSetTile_G8_C4_FULL(batch, x, y, x + y, color);
            }
        }

        tuiPanelDrawBatch(panel, atlas, palette, batch);
        tuiPanelRender(panel);
        //tuiPanelRenderTransformed(panel, window_width/2, window_width, window_height/2, window_height); //Use this function instead to render the panel within the given rect sides.

        glfwSwapBuffers(window); //swap the window buffers
    }

    /* Destroy all TUIC objects */
    tuiPanelDestroy(panel);
    tuiBatchDestroy(batch);
    tuiPaletteDestroy(palette);
    tuiGlyphAtlasDestroy(atlas);
    tuiInstanceDestroy(instance); //The instance must always be created first and destroyed last.

    glfwTerminate();
    return 0;
}