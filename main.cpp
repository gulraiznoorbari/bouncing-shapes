#include <iostream>
#include <fstream>

#include "imgui.h"
#include "imgui-SFML.h"
#include <SFML/Graphics.hpp>

int main(int argc, int *argv[])
{
    const int wWidth = 1280;
    const int wHeight = 720;

    sf::RenderWindow window(sf::VideoMode(wWidth, wHeight), "SFML Works!");
    window.setFramerateLimit(60);

    ImGui::SFML::Init(window);
    sf::Clock deltaTime;

    ImGui::GetStyle().ScaleAllSizes(2.0f);
    ImGui::GetIO().FontGlobalScale = 2.0f;

    float circleRGB[3] = { 0.0f, 1.0f, 1.0f };
    float circleRadius = 50.0f;
    int circleSegments = 32;
    float circleSpeedX = 1.0f;
    float circleSpeedY = 1.5f;
    bool drawCircle = true;
    bool drawText = true;

    sf::CircleShape circle(circleRadius, circleSegments);
    circle.setPosition(10.0f, 10.0f);

    sf::Font myFont;
    if (!myFont.loadFromFile("assets/fonts/RobotoSlab.ttf"))
    {
        std::cerr << "Could not load the font!\n";
        exit(-1);
    }

    sf::Text text("Sample Text", myFont, 24);
    text.setPosition(0, wHeight - (float)text.getCharacterSize());

    char displayString[255] = "Sample Text";

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            ImGui::SFML::ProcessEvent(event);

            if (event.type == sf::Event::Closed)
            {
                window.close();
            }

            if (event.type == sf::Event::KeyPressed)
            {
                std::cout << "Key Pressed with Code: " << event.key.code << "\n";
                if (event.key.code == sf::Keyboard::X)
                {
                    circleSpeedX *= -1.0f;
                }
            }
        }

        ImGui::SFML::Update(window, deltaTime.restart());

        ImGui::Begin("Window Title");
        ImGui::Text("Window Text!");
        ImGui::Checkbox("Draw Circle", &drawCircle);
        ImGui::SameLine();
        ImGui::Checkbox("Draw Text", &drawText);
        ImGui::SliderFloat("Radius", &circleRadius, 0.0f, 300.0f);
        ImGui::SliderInt("Sides", &circleSegments, 3, 64);
        ImGui::ColorEdit3("Color Circle", circleRGB);
        ImGui::InputText("Text", displayString, 255);
        if (ImGui::Button("Set Text"))
        {
            text.setString(displayString);
        }
        ImGui::SameLine();
        if (ImGui::Button("Reset Circle"))
        {
            circle.setPosition(0.0f, 0.0f);
        }
        ImGui::End();

        circle.setPointCount(circleSegments);
        circle.setRadius(circleRadius);

        circle.setFillColor(sf::Color(sf::Uint8(circleRGB[0] * 255), sf::Uint8(circleRGB[1] * 255), sf::Uint8(circleRGB[2] * 255)));
        circle.setPosition(circle.getPosition().x + circleSpeedX, circle.getPosition().y + circleSpeedY);

        window.clear();

        if (drawCircle)
        {
            window.draw(circle);
        }
        if (drawText)
        {
            window.draw(text);
        }
        ImGui::SFML::Render(window);
        window.display();
    }

    return 0;
}

//#include <SFML/Graphics.hpp>
//
//int main()
//{
//    sf::RenderWindow window(sf::VideoMode(200, 200), "SFML works!");
//    sf::CircleShape shape(100.f);
//    shape.setFillColor(sf::Color::Green);
//
//    while (window.isOpen())
//    {
//        sf::Event event;
//        while (window.pollEvent(event))
//        {
//            if (event.type == sf::Event::Closed)
//                window.close();
//        }
//
//        window.clear();
//        window.draw(shape);
//        window.display();
//    }
//
//    return 0;
//}