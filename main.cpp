
#include "raylib.h"
#include <string>
#include <iostream>
#include <fstream>
#include <openssl/evp.h>
#include <sstream>
#include <iomanip>

int main()
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(800, 450, "SHA-256 hash");
    SetTargetFPS(60);

    std::string filePath;
    std::string hashText;
    std::string statusText;

    while (!WindowShouldClose()) {
        if(IsFileDropped()) {
            FilePathList files = LoadDroppedFiles();

            if(files.count > 0) {
                filePath = files.paths[0];
                hashText.clear();
                std::ifstream file(filePath, std::ios::binary);

                if (!file.is_open()) {
                    statusText = "Failed to open this file.\n";
                } else {
                    statusText = "File is opened: " + filePath + "\n";
                    EVP_MD_CTX* context = EVP_MD_CTX_new();

                    if (context == nullptr) {
                        statusText = "Failed to create the hash context.\n";
                    } else {
                        if (EVP_DigestInit_ex(context, EVP_sha256(), nullptr) != 1) {
                            statusText = "Failed to initialize the hash context.\n";
                        } else { 
                            char buffer[65536];
                            bool success = true;

                            while (file) {
                                file.read(buffer, sizeof(buffer));
                                std::streamsize bytesRead = file.gcount();

                                if (bytesRead > 0) {
                                    if (EVP_DigestUpdate(context, buffer,
                                    static_cast<std::size_t>(bytesRead)) != 1) {
                                        statusText = "Error during hash calculation.\n";
                                        success = false;
                                        break;
                                    }
                                }

                            }
                            
                            if (file.bad() || (file.fail() && !file.eof())) {
                                statusText = "Error reading the file.\n";
                                success = false;
                            }

                            if(success) {
                                unsigned char digest[EVP_MAX_MD_SIZE];
                                unsigned int digestLength = 0;

                                if (EVP_DigestFinal_ex(context, digest, &digestLength) != 1) {
                                    statusText = "Failed to finalize the hash calculation.\n";
                                } else {
                                    std::ostringstream hex;
                                    hex << std::hex << std::setfill('0');

                                    for (unsigned int i = 0; i < digestLength; ++i) {
                                        hex << std::setw(2)
                                            << static_cast<unsigned int >(digest[i]);

                                    }

                                    hashText = hex.str();
                                    statusText = "SHA-256 calculated successfully.";
                                    std::cout << "SHA-256 hash: " << hashText << '\n';
                                }
                            }
                        }

                        EVP_MD_CTX_free(context);
                    }
                }
            }

            UnloadDroppedFiles(files);
        }

        BeginDrawing();
        ClearBackground(Color{24, 26, 32, 255});
        DrawText("Drop a file here!", 30, 30, 28, RAYWHITE);
        DrawText(filePath.c_str(), 30, 100, 18, LIGHTGRAY);
        DrawText("SHA-256: ", 30, 160, 18, RAYWHITE);
        if (!hashText.empty()) {
           DrawText("SHA-256:", 30, 160, 18, RAYWHITE);
           DrawText(hashText.substr(0, 32).c_str(), 30, 195, 20, GREEN);
           DrawText(hashText.substr(32).c_str(), 30, 225, 20, GREEN);

           float buttonWidht = MeasureText("copy to clipboard", 20) + 30.0f;
           Rectangle copyButton = {30, 260, buttonWidht, 35};
           bool hovered = CheckCollisionPointRec(GetMousePosition(), copyButton);
           DrawRectangleRec(copyButton, hovered ? DARKGRAY : GRAY);
           DrawText("Copy to clipboard", 40, 270, 20, RAYWHITE);

           if (hovered && IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
               SetClipboardText(hashText.c_str());
               statusText = "Hash copied!";
           }

           const char* resetLabel = "Calculate new hash";   
           float resetWidth = MeasureText(resetLabel, 20) + 30.0f;

           Rectangle resetButton{
            copyButton.x + copyButton.width + 15,
            copyButton.y,
            resetWidth,
            35
           };

           bool resetHovered = CheckCollisionPointRec(GetMousePosition(), resetButton);
           DrawRectangleRec(resetButton, resetHovered ? DARKGRAY : GRAY);
           DrawText(resetLabel, static_cast<int>(resetButton.x + 15), static_cast<int>(resetButton.y + 8), 20, RAYWHITE);

           if (resetHovered && IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
               filePath.clear();
               hashText.clear();
               statusText = "Drop a file here!";
           }
        }
        DrawText(statusText.c_str(), 30, 300, 18, RAYWHITE);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
