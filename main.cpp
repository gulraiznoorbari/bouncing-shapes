#include <iostream>
#include <fstream>
#include <vector>
#include <memory>

#include "imgui.h"
#include "imgui-SFML.h"
#include <SFML/Graphics.hpp>

class MyShape
{
    std::string m_type = "";
    std::string m_name = "";
    float m_xPosition = 0.0f;
    float m_yPosition = 0.0f;
    float m_xSpeed = 0.0f;
    float m_ySpeed = 0.0f;
    float m_width = 0.0f;
    float m_height = 0.0f;
    float m_radius = 0.0f;
    int m_red = 255;
    int m_green = 255;
    int m_blue = 255;
    std::shared_ptr<sf::Shape> shape;
    std::shared_ptr<sf::Font> m_fontPtr;
    std::shared_ptr<sf::Text> text = std::make_shared<sf::Text>();

public:
    MyShape(const std::string objectType, const std::string name, float xPosition, float yPosition, float xSpeed, float ySpeed, int r, int g, int b, float radius)
        : m_type(objectType)
        , m_name(name)
        , m_xPosition(xPosition)
        , m_yPosition(yPosition)
        , m_xSpeed(xSpeed)
        , m_ySpeed(ySpeed)
        , m_red(r)
        , m_green(g)
        , m_blue(b)
        , m_radius(radius)
    {
        if (m_type == "Circle")
        {
            shape = std::make_shared<sf::CircleShape>(m_radius);
            shape->setFillColor(sf::Color(m_red, m_green, m_blue));
            shape->setPosition(m_xPosition, m_yPosition);
        }
    };

    MyShape(const std::string objectType, const std::string name, float xPosition, float yPosition, float xSpeed, float ySpeed, int r, int g, int b, float width, float height)
        : m_type(objectType)
        , m_name(name)
        , m_xPosition(xPosition)
        , m_yPosition(yPosition)
        , m_xSpeed(xSpeed)
        , m_ySpeed(ySpeed)
        , m_red(r)
        , m_green(g)
        , m_blue(b)
        , m_width(width)
        , m_height(height)
    {
        if (m_type == "Rectangle")
        {
            shape = std::make_shared<sf::RectangleShape>(sf::Vector2f(m_width, m_height));
            shape->setFillColor(sf::Color(m_red, m_green, m_blue));
            shape->setPosition(m_xPosition, m_yPosition);
        }
    };

    const std::shared_ptr<sf::Shape> getShape()
    {
        return shape;
    };

    const std::shared_ptr<sf::Text> getText()
    {
        return text;
    };

    void setName(const std::string& newName) 
    {
        m_name = newName; 
    }
    
    const std::string& getName() const 
    {
        return m_name; 
    }

    float getXSpeed() const 
    { 
        return m_xSpeed; 
    }
    
    float getYSpeed() const 
    { 
        return m_ySpeed; 
    }

    void setSpeed(float x, float y) 
    { 
        m_xSpeed = x; 
        m_ySpeed = y; 
    }

    void setText(std::shared_ptr<sf::Font> fontPtr, std::string textValue, int size, const sf::Color& color)
    {
        m_fontPtr = fontPtr;
        text->setFont(*m_fontPtr);
        text->setString(textValue);
        text->setCharacterSize(size);
        text->setFillColor(color);

        // Center the text on the shape
        sf::FloatRect textBounds = text->getLocalBounds();
        sf::FloatRect shapeBounds = shape->getGlobalBounds();
        text->setOrigin(textBounds.width / 2.f, textBounds.height / 2.f);
        text->setPosition(shapeBounds.left + shapeBounds.width / 2.f, shapeBounds.top + shapeBounds.height / 2.f);
    };

    void updatePosition(sf::Vector2u windowSize)
    {
        // This function handles position for both the shape and text (inside the shape)
        int windowWidth = windowSize.x;
        int windowHeight = windowSize.y;

        sf::FloatRect bounds = shape->getGlobalBounds();

        // Check horizontal bounds
        if (bounds.left <= 0 && m_xSpeed < 0)
        {
            m_xSpeed *= -1.0f;
        }
        else if (bounds.left + bounds.width >= windowWidth && m_xSpeed > 0)
        {
            m_xSpeed *= -1.0f;
        }

        // Check vertical bounds
        if (bounds.top <= 0 && m_ySpeed < 0)
        {
            m_ySpeed *= -1.0f;
        }
        else if (bounds.top + bounds.height >= windowHeight && m_ySpeed > 0)
        {
            m_ySpeed *= -1.0f;
        }

        m_xPosition += m_xSpeed;
        m_yPosition += m_ySpeed;

        shape->setPosition(m_xPosition, m_yPosition);
        if (text && text->getFont())
        {
            sf::FloatRect textBounds = text->getLocalBounds();
            text->setOrigin(textBounds.width / 2.0f, textBounds.height / 2.0f);
            text->setPosition(m_xPosition + bounds.width / 2.0f, m_yPosition + bounds.height / 2.0f);
        }
    };
};

void loadDataFromFile(const std::string filename, sf::RenderWindow& window, std::vector<std::shared_ptr<MyShape>>& shapes)
{
    std::ifstream inputFile(filename);

    float width, height, radius, xPosition, yPosition, xSpeed, ySpeed;
    int red, green, blue, fontSize;
    unsigned int windowWidth, windowHeight;
    std::string objectType, name, fontPath;
    sf::Color fontColor;
    bool fontLoaded = false;
    std::shared_ptr<sf::Font> fontPtr;

    while (inputFile >> objectType)
    {
        if (objectType == "Window")
        {
            inputFile >> windowWidth >> windowHeight;
            window.create(sf::VideoMode(windowWidth, windowHeight), "Moving Shapes");
        }
        else if (objectType == "Font") 
        {
            inputFile >> fontPath >> fontSize >> red >> green >> blue;
            fontColor = sf::Color(red, green, blue);

            fontPtr = std::make_shared<sf::Font>();
            if (!fontPtr->loadFromFile(fontPath))
            {
                std::cerr << "Could not load font: " << fontPath << "\n";
                exit(-1);
            }
            fontLoaded = true;
        }
        else if (objectType == "Circle")
        {
            inputFile >> name >> xPosition >> yPosition >> xSpeed >> ySpeed >> red >> green >> blue >> radius;
            std::shared_ptr<MyShape> circle = std::make_shared<MyShape>(objectType, name, xPosition, yPosition, xSpeed, ySpeed, red, green, blue, radius);
            if (fontLoaded)
            {
                circle->setText(fontPtr, name, fontSize, fontColor);
            }
            shapes.push_back(circle);
        }
        else if (objectType == "Rectangle")
        {
            inputFile >> name >> xPosition >> yPosition >> xSpeed >> ySpeed >> red >> green >> blue >> width >> height;
            std::shared_ptr<MyShape> rectangle = std::make_shared<MyShape>(objectType, name, xPosition, yPosition, xSpeed, ySpeed, red, green, blue, width, height);
            if (fontLoaded)
            {
                rectangle->setText(fontPtr, name, fontSize, fontColor);
            }
            shapes.push_back(rectangle);
        }
    }

    inputFile.close();
};

int main(int argc, int *argv[])
{
    std::vector<std::shared_ptr<MyShape>> myShapes;
    sf::RenderWindow window;

    loadDataFromFile("data/config.txt", window, myShapes);
    window.setFramerateLimit(60);

    ImGui::SFML::Init(window);
    sf::Clock deltaTime;

    ImGui::GetStyle().ScaleAllSizes(2.0f);
    ImGui::GetIO().FontGlobalScale = 2.0f;

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
        }

        ImGui::SFML::Update(window, deltaTime.restart());
        
        static int selectedShapeIndex = 0;
        static std::vector<bool> shapeVisibility;
        if (shapeVisibility.size() != myShapes.size())
        {
            shapeVisibility.resize(myShapes.size(), true);
        }

        ImGui::Begin("Shape Controls");
        if (!myShapes.empty())
        {
            std::vector<const char*> shapeNames;
            for (const auto& shape : myShapes)
            {
                shapeNames.push_back(shape->getName().c_str());
            }
            ImGui::Combo("Shape", &selectedShapeIndex, shapeNames.data(), (int)shapeNames.size());

            auto& shape = myShapes[selectedShapeIndex];
            auto sfShape = shape->getShape();

            bool visible = shapeVisibility[selectedShapeIndex];
            if (ImGui::Checkbox("Draw Shape", &visible))
            {
                shapeVisibility[selectedShapeIndex] = visible;
            }

            float shapeScale = sfShape->getScale().x;
            ImGui::SliderFloat("Scale", &shapeScale, 0.1f, 5.0f);
            sfShape->setScale(shapeScale, shapeScale);

            float speedX = shape->getXSpeed();
            float speedY = shape->getYSpeed();
            if (ImGui::InputFloat2("Speed", &speedX))
            {
                shape->setSpeed(speedX, speedY);
            }

            sf::Color currentColor = sfShape->getFillColor();
            float color[3] = {
                currentColor.r / 255.0f,
                currentColor.g / 255.0f,
                currentColor.b / 255.0f
            };
            if (ImGui::ColorEdit3("Color", color))
            {
                sfShape->setFillColor(sf::Color(
                    static_cast<sf::Uint8>(color[0] * 255),
                    static_cast<sf::Uint8>(color[1] * 255),
                    static_cast<sf::Uint8>(color[2] * 255)
                ));
            }

            static char nameBuffer[64];
            strncpy_s(nameBuffer, shape->getName().c_str(), _TRUNCATE);
            if (ImGui::InputText("Name", nameBuffer, IM_ARRAYSIZE(nameBuffer)))
            {
                shape->setName(std::string(nameBuffer));
                shape->getText()->setString(nameBuffer);
            }
        }
        else
        {
            ImGui::Text("No Shapes Loaded.");
        }
        ImGui::End();

        window.clear(sf::Color::Black);
        for (size_t i = 0; i < myShapes.size(); ++i)
        {
            auto& shape = myShapes[i];
            shape->updatePosition(window.getSize());
            if (shapeVisibility[i])
            {
                if (shapeVisibility[selectedShapeIndex])
                {
                    window.draw(*shape->getShape());
                }
                if (shape->getText() && shape->getText()->getFont()) 
                {
                    window.draw(*shape->getText());
                }
            }
        }
        
        ImGui::SFML::Render(window);
        window.display();
    }

    return 0;
}


/*
Config.txt Data Pattern:
(Window width height)
(Font fontPath size red green blue)
(Circle Name xPos yPos xSpeed ySpeed red green blue radius)
(Rectangle Name xPos yPos xSpeed ySpeed red green blue width height)
*/


//int main(int argc, int* argv[])
//{
//    const int wWidth = 1280;
//    const int wHeight = 720;
//
//    sf::RenderWindow window(sf::VideoMode(wWidth, wHeight), "SFML Works!");
//    window.setFramerateLimit(60);
//
//    ImGui::SFML::Init(window);
//    sf::Clock deltaTime;
//
//    ImGui::GetStyle().ScaleAllSizes(2.0f);
//    ImGui::GetIO().FontGlobalScale = 2.0f;
//
//    float circleRGB[3] = { 0.0f, 1.0f, 1.0f };
//    float circleRadius = 50.0f;
//    int circleSegments = 32;
//    float circleSpeedX = 1.0f;
//    float circleSpeedY = 1.5f;
//    bool drawCircle = true;
//    bool drawText = true;
//
//    sf::CircleShape circle(circleRadius, circleSegments);
//    circle.setPosition(10.0f, 10.0f);
//
//    sf::Font myFont;
//    if (!myFont.loadFromFile("assets/fonts/RobotoSlab.ttf"))
//    {
//        std::cerr << "Could not load the font!\n";
//        exit(-1);
//    }
//
//    sf::Text text("Sample Text", myFont, 24);
//    text.setPosition(0, wHeight - (float)text.getCharacterSize());
//
//    char displayString[255] = "Sample Text";
//
//    while (window.isOpen())
//    {
//        sf::Event event;
//        while (window.pollEvent(event))
//        {
//            ImGui::SFML::ProcessEvent(event);
//
//            if (event.type == sf::Event::Closed)
//            {
//                window.close();
//            }
//
//            if (event.type == sf::Event::KeyPressed)
//            {
//                std::cout << "Key Pressed with Code: " << event.key.code << "\n";
//                if (event.key.code == sf::Keyboard::X)
//                {
//                    circleSpeedX *= -1.0f;
//                }
//            }
//        }
//
//        ImGui::SFML::Update(window, deltaTime.restart());
//
//        ImGui::Begin("Window Title");
//        ImGui::Text("Window Text!");
//        ImGui::Checkbox("Draw Circle", &drawCircle);
//        ImGui::SameLine();
//        ImGui::Checkbox("Draw Text", &drawText);
//        ImGui::SliderFloat("Radius", &circleRadius, 0.0f, 300.0f);
//        ImGui::SliderInt("Sides", &circleSegments, 3, 64);
//        ImGui::ColorEdit3("Color Circle", circleRGB);
//        ImGui::InputText("Text", displayString, 255);
//        if (ImGui::Button("Set Text"))
//        {
//            text.setString(displayString);
//        }
//        ImGui::SameLine();
//        if (ImGui::Button("Reset Circle"))
//        {
//            circle.setPosition(0.0f, 0.0f);
//        }
//        ImGui::End();
//
//        circle.setPointCount(circleSegments);
//        circle.setRadius(circleRadius);
//
//        circle.setFillColor(sf::Color(sf::Uint8(circleRGB[0] * 255), sf::Uint8(circleRGB[1] * 255), sf::Uint8(circleRGB[2] * 255)));
//        circle.setPosition(circle.getPosition().x + circleSpeedX, circle.getPosition().y + circleSpeedY);
//
//        window.clear();
//
//        if (drawCircle)
//        {
//            window.draw(circle);
//        }
//        if (drawText)
//        {
//            window.draw(text);
//        }
//        ImGui::SFML::Render(window);
//        window.display();
//    }
//
//    return 0;
//}