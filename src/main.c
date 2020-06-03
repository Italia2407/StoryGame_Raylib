#include "libco.h"
#include "raylib.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define SCREEN_WIDTH (1280)
#define SCREEN_HEIGHT (720)

#define TEXT_SPEED (0.005f)

// Change this depending on the path of your executable relative to the assets folder
#define ASSET_PATH "../assets/"

cothread_t main_cothread;
cothread_t story_cothread;

typedef enum CommandType
{
    CT_NONE = 0,
    CT_ECHO,
    CT_OPTION
} CommandType;

typedef struct Command
{
    CommandType type;

    union
    {
        struct
        {
        	// In
            const char* text;
            int text_len;

            // Working
            float timer;
            int text_i;
        } echo;

        struct
        {
        	// In
            const char* text;
            int text_len;

            const char* optA;
            int optA_len;

            const char* optB;
            int optB_len;

            // Working
            float timer;
            int text_i;
            int spaceA;
            int spaceB;
            bool text_anim_done;

            // Out
            bool result;
        } option;
    };
    
} Command;

Command cmd;

void echo(const char* text)
{
	cmd = (Command){
        .type = CT_ECHO,
        .echo.text = text,
        .echo.text_len = strlen(text)
    };
	
    co_switch(main_cothread);
}

bool option(const char* text, const char* optA, const char* optB)
{
	cmd = (Command){
        .type = CT_OPTION,

        .option.text = text,
        .option.text_len = strlen(text),

        .option.optA = optA,
        .option.optA_len = strlen(optA),

        .option.optB = optB,
        .option.optB_len = strlen(optB)
    };
	
    co_switch(main_cothread);
    return cmd.option.result;
}

void pause()
{
	cmd = (Command){0};
	co_switch(main_cothread);
}

void story()
{
	// TODO
	// Write a different story
	echo("Hi, my name is Sussie from the Southern Beans Tribe and I am the high lord of UwU.\n\n");
	echo("I have developed the UwU movement during the Second Bread War in order to mobilize and win all the BROD.\n\n");
	
	if (option("Will you join me in the cause of UwU or will you disgrace me by taking the path of -_- ?\n", "YES", "NO"))
	{
		echo("Amazin'");
		pause();
	}
	else
	{
		echo("SHAME!");
		pause();
	}
}

int main(void)
{
    main_cothread = co_active();
    story_cothread = co_create(1024*8, story, NULL);

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Story Game");
    SetTargetFPS(60);

    bool command_executed = true;
    char text[1024*8] = {0};
    int text_i = 0;

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (command_executed)
        {
            co_switch(story_cothread);
			command_executed = false;
        }

        switch (cmd.type)
        {
            case CT_NONE:
            {
                
            } break;
            case CT_ECHO:
            {
				cmd.echo.timer += GetFrameTime();
                if (cmd.echo.timer >= TEXT_SPEED)
                {
					cmd.echo.timer = 0.0f;
                    text[text_i++] = cmd.echo.text[cmd.echo.text_i++];
                    if (cmd.echo.text_i == cmd.echo.text_len)
						command_executed = true;
                }
            } break;
            case CT_OPTION:
            {
                cmd.option.text_anim_done = cmd.option.text_i == cmd.option.text_len;
	
				cmd.option.timer += GetFrameTime();
                if (cmd.option.timer >= TEXT_SPEED && !cmd.option.text_anim_done)
                {
					cmd.option.timer = 0.0f;
                    text[text_i++] = cmd.option.text[cmd.option.text_i++];
	
					cmd.option.text_anim_done = cmd.option.text_i == cmd.option.text_len;
                    if (cmd.option.text_anim_done)
                    {
                    	cmd.option.result = true;
                        const char* option_text = TextFormat("\n>%s   %s\n\n", cmd.option.optA, cmd.option.optB);
                        cmd.option.spaceA = text_i + 1;
                        cmd.option.spaceB = text_i + 1 + cmd.option.optA_len + 3;
                        memcpy(&text[text_i], option_text, strlen(option_text) + 1);
                        text_i += strlen(option_text);
                    }
                }
                
                if (cmd.option.text_anim_done)
				{
                	if (IsKeyPressed(KEY_LEFT))
					{
						cmd.option.result = true;
						text[cmd.option.spaceA] = '>';
						text[cmd.option.spaceB] = ' ';
					}
					if (IsKeyPressed(KEY_RIGHT))
					{
						cmd.option.result = false;
						text[cmd.option.spaceA] = ' ';
						text[cmd.option.spaceB] = '>';
					}
					if (IsKeyPressed(KEY_ENTER))
					{
						command_executed = true;
					}
				}

            } break;
            default:
            {
                
            } break;
        }

        const int padd = 30;
        const Rectangle text_area = {padd, padd, GetScreenWidth() - padd*2, GetScreenHeight() - padd*2};
        DrawTextRec(GetFontDefault(), text, text_area, 20, 2, true, BLACK);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}