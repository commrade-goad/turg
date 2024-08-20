// include some needed libs.
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "../raylib-5.0_linux_amd64/include/raylib.h"

// defining some constant -> window height, width, and player font size.
#define WIN_W 1280
#define WIN_H 720
#define PLAYER_FONT_SIZE 32

// use real board texture
#define USER_REAL_BOARD_TEXT

// create game state
enum GameState {
    GAME,
    MENU,
    WIN,
};

// creating new data type for the bead.
struct GameObject {
    int onBoard;
    int finished;
    int pos; // 14
};

struct Tooltip {
    Rectangle box;
    Rectangle text_pos;
    char text[2];
};

// function to jump to the next turn.
void incrementTurn(int *turn) {
    *turn = (*turn == 0) ? 1 : 0;
}

// generate random int what else.
int gen_random(int max) {
    long result = random() % (max + 1);
    return result;
}

// generate the random dice.
int setup_dice(char *str) {
    int move = gen_random(4);
    snprintf(str, 2, "%d",move); 
    return move;
}

void check_victory(int point[2], enum GameState *state, int turn) {
    if (point[turn] >=7) {
        *state = WIN;
    }
}

#ifdef CUSTOM_MODE
int check_move(const struct GameObject obj[7], size_t idx, int move) {
    int cur_pos = obj[idx].pos;
    size_t pos_buf[7] = {};
    size_t index = 0;

    for (int i = 0; i < 7; i++) {
        if (cur_pos < obj[i].pos) {
            pos_buf[index] = obj[i].pos;
            printf("pos_buf  : %d\n", obj[i].pos);
            index += 1;
        }
    }

    for (int i = 0; i < index + 1; i++) {
        if (cur_pos + move > pos_buf[i] && pos_buf[i] > 0) {
            printf("Not valid because :\n");
            printf(" index : %d\n", i);
            printf(" with : %d + %d > %zu\n", cur_pos, move, pos_buf[i]);
            return 1;
        }
    }
    return 0;
}
#endif

int main (void) {

    srandom(time(NULL));

    // init window.
    InitWindow(WIN_W, WIN_H, "TURG");
    SetExitKey('Q');
    SetTargetFPS(60);

    // load texture.
#ifdef USER_REAL_BOARD_TEXT
    Texture2D board = LoadTexture("assets/board-real.png");
#else
    Texture2D board = LoadTexture("assets/board.png");
#endif
    Texture2D whiteb = LoadTexture("assets/white.png");
    Texture2D blackb = LoadTexture("assets/black.png");

    // define board size.
    int board_w = 20 * 8;
    int board_h = 20 * 3;

    // define bead size.
    int bead_w = 20;
    int bead_h = 20;

    // define player.
    struct GameObject player[2][7] = {};

    // define turn.
    int turn = 0;

    //define scale.
    float board_scale = 4.0f;
    float bead_scale = 2.0f;

    // define rect for src and dest.
    Rectangle source_board = {0, 0, board_w, board_h};
    Rectangle destination_board = {(float)WIN_W / 2 - (board_w * 2), (float)WIN_H / 2 - (board_h * 2), board_w * board_scale, board_h * board_scale};
    Vector2 origin = {0,0};
    Rectangle source_bead = {0, 0, bead_w, bead_h};

    // black bead aka player 1.
    Rectangle destination_bbead = {destination_board.x + (board_w * board_scale) - (bead_w * 7) * bead_scale, destination_board.y - board_h, bead_w * bead_scale, bead_h * bead_scale};
    Rectangle destination_bbead_arr[7] = {};
    for (int j = 0;j < 7; j++) {
        destination_bbead_arr[j] = destination_bbead;
        if (j > 0) {
            destination_bbead_arr[j].x += bead_w * bead_scale;
            destination_bbead = destination_bbead_arr[j];
        }
    }

    // white bead aka the player 2.
    Rectangle destination_wbead = {destination_board.x, (destination_board.y + board_h * board_scale) + 24, bead_w * bead_scale, bead_h * bead_scale};
    Rectangle destination_wbead_arr[7] = {};
    for (int j = 0;j < 7; j++) {
        destination_wbead_arr[j] = destination_wbead;
        if (j > 0) {
            destination_wbead_arr[j].x += bead_w * bead_scale;
            destination_wbead = destination_wbead_arr[j];
        }
    }

    // getting the dice.
    char str[2];
    int move = setup_dice(str);

    // counter for how many beads is out / on board.
    int cameout[2] = {0, 0};

    // point system.
    int point[2] = {0, 0};
    int winner_idx = -1;

    // handle extra turn
    int extra_turn = 0;

    // game state.
    enum GameState state = MENU;

    // dvd box for menu.
    Rectangle dvd = {0, 0, 200, 200};
    Vector2 pwr = {400, 400};

    // game loop.
    while (!WindowShouldClose()) {

        // game logic.

        // menu state
        if (state == MENU) {
            if (IsKeyPressed(KEY_P)) {
                state = GAME;
            }

            BeginDrawing();
            ClearBackground(GetColor(0xccccdeff));

            // handle dvd
            DrawRectanglePro(dvd, origin, 0.0f, GetColor(0xeeeeefff));
            float frame = GetFrameTime();
            dvd.x += pwr.x * frame;
            dvd.y += pwr.y * frame;

            if (dvd.x + dvd.width >= WIN_W || dvd.x < 0) pwr.x *= -1;
            if (dvd.y + dvd.height >= WIN_H || dvd.y < 0) pwr.y *= -1;

            char *menu_buf = "TURG";
            char *menu_opt1_buf = "Press `p` to play.";
            char *menu_opt2_buf = "Press `q` to quit.";
            DrawRectangle(0, 0, WIN_W, WIN_H, GetColor(0x00000098));
            DrawText(menu_buf, ((float)WIN_W / 2) - ((float)MeasureText(menu_buf, 48)/2), ((float)WIN_H / 2) - ((float)48 / 2), 48, WHITE);
            DrawText(menu_opt1_buf, ((float)WIN_W / 2) - ((float)MeasureText(menu_opt1_buf, 20)/2), ((float)WIN_H / 2) + 48, 20, WHITE);
            DrawText(menu_opt2_buf, ((float)WIN_W / 2) - ((float)MeasureText(menu_opt2_buf, 20)/2), ((float)WIN_H / 2) + 48 + 25, 20, WHITE);

            EndDrawing();

            continue;
        }

        for (int i = 0; i < 7; i++) {
            if (player[0][i].pos == 0) player[0][i].onBoard = 0;
            if (player[1][i].pos == 0) player[1][i].onBoard = 0;
        }

        // Handle input.
        if (IsKeyPressed(' ')) {
            incrementTurn(&turn);
            move = setup_dice(str);
        }
        if (IsKeyPressed(KEY_I)) {
            for (int i = 0; i < 7; i++) {
                printf("IDX : %d\n", i);
                printf("  onBoard : %d\n", player[turn][i].onBoard);
                printf("  pos : %d\n", player[turn][i].pos);
                printf("  finished : %d\n", player[turn][i].finished);
                printf("==========================\n");
            }
        }

        // create a bool to check if the current dice pos is already used.
        int ignore_key = 0;
        if (IsKeyPressed(KEY_N) && move > 0) {
            for (int i = 0; i < 7; i++) {
                if (player[turn][i].pos == move) {
                    ignore_key = 1;
                    break;
                }
            }
            if (ignore_key == 0) {
                for (int i = 0; i < 7; i++) {
                    if (player[turn][i].onBoard == 0 && player[turn][i].finished == 0 && player[turn][i].pos == 0) {
#ifdef CUSTOM_MODE
                        if (check_move(player[turn], i, move) == 1) {
                            ignore_key = 1;
                            break;
                        }
#endif
                        player[turn][i].onBoard = 1;
                        player[turn][i].finished= 0;
                        player[turn][i].pos = move;
                        cameout[turn] += 1;
                        if (move != 4) incrementTurn(&turn);
                        move = setup_dice(str);
                        break;
                    }
                }
            } else {
                ignore_key = 1;
            }
        }
        ignore_key = 0;

        // reset key.
        if (IsKeyPressed(KEY_R)) {
            for (int j = 0; j < 2; j++) {
                for (int i = 0; i < 7; i++) {
                    player[j][i].onBoard = 0;
                    player[j][i].finished= 0;
                    player[j][i].pos = 0;
                }
                cameout[j] = 0;
            }
            point[0] = 0;
            point[1] = 0;
            turn = 0;
            move = setup_dice(str);
            state = GAME;
        }

        // handle index as the keys.
        for (int i  = 0; i < 7; i++) {
            if (IsKeyPressed(i + 1 + '0') && player[turn][i].onBoard && !player[turn][i].finished) {
                for (int k = 0; k < 7; k++) {
                    if (player[turn][i].pos + move == player[turn][k].pos) {
                        ignore_key = 1;
                        break;
                    }
#ifdef CUSTOM_MODE
                    if (check_move(player[turn], i, move) == 1) {
                        ignore_key = 1;
                        break;
                    }
#endif
                }
                if (ignore_key == 0 && player[turn][i].pos < 15 && player[turn][i].pos + move <= 15) {
                    if (player[turn][i].pos + move == 8) extra_turn = 1;
                    if (player[turn][i].pos + move == 4) extra_turn = 1;
                    player[turn][i].pos += move;
                    int invert = 0;
                    if (turn == 0) invert = 1;
                    for (int k = 0; k < 7; k++) {
                        // check for the special tile
                        if (player[turn][i].pos == 8 && player[invert][k].pos == 8) {
                            player[turn][i].pos -= move;
                            break;
                        }
                        // capturing the other player piece
                        if (player[invert][k].pos == player[turn][i].pos && player[turn][i].pos >= 5 && player[turn][i].pos <= 12) {
                            player[invert][k].pos = 0;
                            player[invert][k].onBoard = 0;
                            cameout[invert] -= 1;
                            break;
                        }
                    }

                    // finish checker at this point bcs the only way to win is from tapping this button.
                    for (int i = 0; i < 7; i++) {
                        if (player[turn][i].pos == 15) {
                            point[turn] += 1;
                            player[turn][i].pos = 0;
                            player[turn][i].finished = 1;
                            break;
                        }
                    }
                    winner_idx = turn;
                    check_victory(point, &state, turn);

                    // add extra turn.
                    if (!extra_turn) {
                        incrementTurn(&turn);
                    } else {
                        extra_turn = 0;
                    }
                    move = setup_dice(str);
                }
                ignore_key = 0;
            }
        }

        // start drawing.
        BeginDrawing();

        // clear the bg.
        ClearBackground(GetColor(0xccccdeff));
        DrawRectangle(0, 0, WIN_W, WIN_H, GetColor(0x00000088));

        // draw the dice nums, text and some tooltip at the bottom.
        Color tmp_color_arr[2] = {BLACK, RED};
        int index_to_use = 0;
        if (move <= 0 ) {
            index_to_use = 1;
            DrawText("Press `space` to skip turn.", ((float)WIN_W / 2) - ((float)MeasureText("Press `space` to skip turn.", 24) / 2), (48 * 2) + 24, 24, BLACK);
        }
        DrawText(str, ((float)WIN_W / 2) - ((float)MeasureText(str, 48) / 2), 48, 48, tmp_color_arr[index_to_use]);
        DrawText("Dice", ((float)WIN_W / 2) - ((float)MeasureText("Dice", 24) / 2), 48 * 2, 24, BLACK);
        DrawText("Select the desired index to move bead.", 0, WIN_H - 20, 20, BLACK);
        DrawText("Press `n` to enter with new bead.", 0, WIN_H - 20 * 2, 20, BLACK);
        DrawText("The Royal Game of Ur", 10, 10, 36, BLACK);

        // draw the board.
        DrawTexturePro(board, source_board, destination_board, origin, 0.0f, WHITE);
        
        // draw the player tooltip
        char buf_poin[2];
        snprintf(buf_poin, 2, "%d", point[turn]);
        if (turn == 0) {
            DrawText(buf_poin, WIN_W - MeasureText(buf_poin, 48) - 10, 10, 48, BLACK);
            DrawText("Player 1", destination_board.x, destination_board.y - board_h, PLAYER_FONT_SIZE, BLACK);
            for (int i = cameout[0]; i < 7; i++) {
                DrawTexturePro(blackb, source_bead, destination_bbead_arr[i], origin, 0.0f, WHITE);
            }
        } else {
            DrawText(buf_poin, WIN_W - MeasureText(buf_poin, 48) - 10, 10, 48, WHITE);
            DrawText("Player 2", destination_board.x + (board_w * board_scale) - (18 * 8), (destination_board.y + board_h * board_scale) + 32, PLAYER_FONT_SIZE, WHITE);
            for (int i = 0; i < 7 - cameout[1]; i++) {
                DrawTexturePro(whiteb, source_bead, destination_wbead_arr[i], origin, 0.0f, WHITE);
            }
        }
        
        // tooltip array
        struct Tooltip tooltip_pos[7] = {};
        // drawing the bead at the table / board.
        Texture2D color_arr[2] = {blackb, whiteb};
        for (int j = 0; j < 2; j++) {
            for (int i = 0; i < 7; i++) {
                if (player[j][i].onBoard && !player[j][i].finished) {
                    int pos = player[j][i].pos;
                    if (pos == 0) {}
                    Rectangle bead_dest = {};
                    if (pos >= 1 && pos <= 4) {
                        int grid = 3 + pos;
                        bead_dest.x = (destination_board.x + bead_w) + (20 * board_scale * grid);
                        bead_dest.y = (destination_board.y + (board_h * board_scale) - bead_w * bead_scale) - bead_h;
                        bead_dest.width = bead_w * bead_scale;
                        bead_dest.height = bead_h * bead_scale;
                        if (j == 0) {
                            bead_dest.y = destination_board.y + bead_h;
                        }
                    }
                    if (pos <= 12 && pos > 4) {
                        int calc_grid = 0;
                        if (pos > 5) {
                            for (int k = 5; k < pos; k++) {
                                calc_grid += 1;
                            }
                        }
                        bead_dest.x = (destination_board.x + bead_w) + (20 * board_scale * (7 - calc_grid));
                        bead_dest.y = (destination_board.y + (20 * board_scale * 2) - bead_w * bead_scale) - bead_h;
                        bead_dest.width = bead_w * bead_scale;
                        bead_dest.height = bead_h * bead_scale;
                    }
                    if (pos == 13) {
                        bead_dest.x = (destination_board.x + bead_w);
                        bead_dest.y = (destination_board.y + (board_h * board_scale) - bead_w * bead_scale) - bead_h;
                        bead_dest.width = bead_w * bead_scale;
                        bead_dest.height = bead_h * bead_scale;
                        if (j == 0) {
                            bead_dest.y = destination_board.y + bead_h;
                        }
                    }
                    if (pos == 14) {
                        bead_dest.x = (destination_board.x + bead_w) + (20 * board_scale);
                        bead_dest.y = (destination_board.y + (board_h * board_scale) - bead_w * bead_scale) - bead_h;
                        bead_dest.width = bead_w * bead_scale;
                        bead_dest.height = bead_h * bead_scale;
                        if (j == 0) {
                            bead_dest.y = destination_board.y + bead_h;
                        }
                    }
                    DrawTexturePro(color_arr[j], source_bead, bead_dest, origin, 0.0f, WHITE);
                    if (j == turn) {
                        bead_dest.y -= (bead_h * bead_scale) + 15;
                        tooltip_pos[i].box = bead_dest;
                        tooltip_pos[i].text_pos = bead_dest;
                        char tmp_buf[3];
                        snprintf(tmp_buf, 2, "%d",i + 1); 
                        strncpy(tooltip_pos[i].text, tmp_buf, 2);
                        tooltip_pos[i].text_pos.x = bead_dest.x + ((float)bead_dest.width / 2) - ((float)MeasureText(tmp_buf, 24)/2);
                        tooltip_pos[i].text_pos.y = bead_dest.y + ((float)bead_dest.height/4);
                    }
                }
            }
        }
        // draw the tooltip.
        Color team_color[2] = {GetColor(0x333333dd), GetColor(0xffffffaa)};
        Color team_color_text[2] = {WHITE, BLACK};
        for (int i = 0; i < 7; i++) {
            DrawRectanglePro(tooltip_pos[i].box, origin, 0.0f, team_color[turn]);
            DrawText(tooltip_pos[i].text, tooltip_pos[i].text_pos.x, tooltip_pos[i].text_pos.y, 24, team_color_text[turn]);
        }

        if (state == WIN) {
            char win_buf[32] = {};
            char *res_buf = "Press `r` to reset the game.";
            snprintf(win_buf, 32, "PLAYER %d WIN", winner_idx+1);
            DrawRectangle(0, 0, WIN_W, WIN_H, GetColor(0x000000cc));
            DrawText(win_buf, ((float)WIN_W / 2) - ((float)MeasureText(win_buf, 48)/2), ((float)WIN_H / 2) - ((float)48 / 2), 48, WHITE);
            DrawText(res_buf, ((float)WIN_W / 2) - ((float)MeasureText(res_buf, 24)/2), ((float)WIN_H / 2) - ((float)48 / 2) + 48, 24, GRAY);
        }

        EndDrawing();

    }
    CloseWindow();
}
