#include "imgui.h"
#include "imgui-SFML.h"

#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>

#include <iostream>
#include <fstream>
#include <vector>

typedef std::vector<std::vector<sf::Vertex>> MatrixOfVertecies;

MatrixOfVertecies redMap;
MatrixOfVertecies greenMap;
MatrixOfVertecies blueMap;

enum ETool
{
    PAINT_ROAD,
    SPAWN_POINT,
    FINISH_POINT
};

static struct Tool {
    sf::Color currentColor = sf::Color::Red;
    ETool etool = PAINT_ROAD;
} s_tool;

static struct ExportSettings {
    float scale = 1.f;
    char exportName[128] = "";
} s_expSettings;

sf::RectangleShape spawnPoint, finishPoint;

sf::RenderWindow *window;


MatrixOfVertecies &GetColorMap(const sf::Color color)
{
    if (color == sf::Color::Red)
        return redMap;
    else if (color == sf::Color::Green)
        return greenMap;
    else// if (color == sf::Color::Blue)
        return blueMap;
}

void ConvertSFML2Unity(std::ofstream &fileName, MatrixOfVertecies &map)
{
    fileName << "new Vector3[] { ";

    for (int i(0); i < map.size(); ++i)
    {
        for (int j(0); j < map[i].size(); ++j)
        {
            // map[i][j].position.x = 1 - map[i][j].position.x;
            map[i][j].position.y = 1 - map[i][j].position.y;

            map[i][j] = sf::Vertex(map[i][j].position * s_expSettings.scale, map[i][j].color);

            map[i][j].position.x *= 1.f / window->getSize().x;
            map[i][j].position.y *= 1.f / window->getSize().y;
        }
    }

    for (int i(0); i < map.size(); ++i)
    {
        for (int j(0); j < map[i].size() - 1; ++j)
        {
            if (map[i].size() < 2)
                break;

            fileName << "new Vector3(" << map[i][j].position.x << "f, " << map[i][j].position.y << "f, 0f), ";
            fileName << "new Vector3(" << map[i][j].position.x << "f, " << map[i][j].position.y << "f, 1f), ";
            fileName << "new Vector3(" << map[i][j + 1].position.x << "f, " << map[i][j + 1].position.y << "f, 1f), ";

            fileName << "new Vector3(" << map[i][j + 1].position.x << "f, " << map[i][j + 1].position.y << "f, 0f), ";
            fileName << "new Vector3(" << map[i][j].position.x << "f, " << map[i][j].position.y << "f, 0f), ";
            fileName << "new Vector3(" << map[i][j + 1].position.x << "f, " << map[i][j + 1].position.y << "f, 1f)";
            
            if (i != map.size() - 1 || j != map[i].size() - 2)
                fileName << ",";
        }
    }

    for (int i(0); i < map.size(); ++i)
    {
        for (int j(0); j < map[i].size(); ++j)
        {
            map[i][j].position.x *= window->getSize().x;
            map[i][j].position.y *= window->getSize().y;

            map[i][j] = sf::Vertex(map[i][j].position * (1.f / s_expSettings.scale), map[i][j].color);

            // map[i][j].position.x = 1 - map[i][j].position.x;
            map[i][j].position.y = 1 - map[i][j].position.y;
        }
    }

    fileName << " },\n";
}

void ExportMap()
{
    // m_maps.Add(new Level(
    //     new Vector3()[] { new Vector3(0f, 0f, 0f) },
    //     new Vector3()[] { new Vector3(0f, 0f, 0f) },
    //     new Vector3()[] { new Vector3(0f, 0f, 0f) },
    //     new Vector3(0f, 0f, 0f),
    //     new Vector3(0f, 0f, 0f)));

    std::ofstream fileName(s_expSettings.exportName);
    fileName << "m_maps.Add(new Level(\n";

    ConvertSFML2Unity(fileName, redMap);
    ConvertSFML2Unity(fileName, greenMap);
    ConvertSFML2Unity(fileName, blueMap);

    sf::Vector2f pos;
    pos = spawnPoint.getPosition() + spawnPoint.getSize() * 0.5f;
    pos.y = 1 - pos.y;
    pos.x *= 1.f / window->getSize().x * s_expSettings.scale;
    pos.y *= 1.f / window->getSize().y * s_expSettings.scale;
    fileName << "new Vector3(" << pos.x << "f, " << pos.y << "f, " << 0.5 << "f),\n";

    pos = finishPoint.getPosition() + finishPoint.getSize() * 0.5f;
    pos.y = 1 - pos.y;
    pos.x *= 1.f / window->getSize().x * s_expSettings.scale;
    pos.y *= 1.f / window->getSize().y * s_expSettings.scale;
    fileName << "new Vector3(" << pos.x << "f, " << pos.y << "f, " << 0.5 << "f)));\n";
}

int main()
{
	window = new sf::RenderWindow(sf::VideoMode(800, 600), "Linux is great!", sf::Style::Close);

    redMap.push_back(std::vector<sf::Vertex>());
    greenMap.push_back(std::vector<sf::Vertex>());
    blueMap.push_back(std::vector<sf::Vertex>());

    spawnPoint.setSize(sf::Vector2f(10, 10));
    spawnPoint.setPosition(sf::Vector2f(-10, -10));
    spawnPoint.setOutlineColor(sf::Color::White);
    spawnPoint.setOutlineThickness(2);

    finishPoint.setSize(sf::Vector2f(10, 10));
    finishPoint.setPosition(sf::Vector2f(-10, -10));
    finishPoint.setOutlineColor(sf::Color::White);
    finishPoint.setOutlineThickness(2);

    window->setFramerateLimit(30);
    ImGui::SFML::Init(*window);

    sf::Event event;

    sf::Clock deltaClock;
    while (window->isOpen())
    {
        while (window->pollEvent(event))
        {
            ImGui::SFML::ProcessEvent(event);

            if (event.type == sf::Event::Closed)
            {
                window->close();
                continue;
            }

            // if click imgui
            ImGuiIO& io = ImGui::GetIO();
            if (io.WantCaptureMouse)
                continue;

            // click sfml
            if (event.type == sf::Event::MouseButtonReleased && 
                event.mouseButton.button == sf::Mouse::Left)
            {
                sf::Vector2i mousePos = sf::Mouse::getPosition(*window);

                if (s_tool.etool == PAINT_ROAD)
                {
                    MatrixOfVertecies &matrix = GetColorMap(s_tool.currentColor);

                    // index last peace of road
                    int lastVector = matrix.size() - 1;

                    sf::Vertex newPoint(sf::Vector2f(mousePos.x, mousePos.y), s_tool.currentColor);

                    matrix[lastVector].push_back(newPoint);
                }
                else if (s_tool.etool == SPAWN_POINT)
                {
                    spawnPoint.setPosition(sf::Vector2f(mousePos.x, mousePos.y) - spawnPoint.getSize() * 0.5f);
                    s_tool.etool = PAINT_ROAD;
                }
                else if (s_tool.etool == FINISH_POINT)
                {
                    finishPoint.setPosition(sf::Vector2f(mousePos.x, mousePos.y) - finishPoint.getSize() * 0.5f);
                    s_tool.etool = PAINT_ROAD;                    
                }
            }
        }

        ImGui::SFML::Update(*window, deltaClock.restart());

        ImGui::Begin("Tools");

        if (ImGui::Button("Red road"))
        {
            s_tool.currentColor = sf::Color::Red;
        }
        
        if (ImGui::Button("Green road"))
        {
            s_tool.currentColor = sf::Color::Green;
        }
        
        if (ImGui::Button("Blue road"))
        {
            s_tool.currentColor = sf::Color::Blue;
        }
        
        if (ImGui::Button("Cut current road"))
        {
            MatrixOfVertecies &matrix = GetColorMap(s_tool.currentColor);
            matrix.push_back(std::vector<sf::Vertex>());
        }

        if (ImGui::Button("Place spawn point"))
        {
            s_tool.etool = SPAWN_POINT;
        }

        if (ImGui::Button("Place finish point"))
        {
            s_tool.etool = FINISH_POINT;
        }

        if (ImGui::Button("Remove last point"))
        {
            MatrixOfVertecies &matrix = GetColorMap(s_tool.currentColor);

            // index last peace of road
            int lastVector = matrix.size() - 1;

            if (matrix[lastVector].size() > 0)
            {
                matrix[lastVector].pop_back();
            }
            else if (matrix.size() > 1)
            {
                matrix.pop_back();
                lastVector = matrix.size() - 1;
                matrix[lastVector].pop_back();
            }
        }

        ImGui::End();

        ImGui::Begin("Export settings");
        
        ImGui::InputText("Filename export", s_expSettings.exportName, 128);
        ImGui::InputFloat("Scale export", &s_expSettings.scale);

        if (ImGui::Button("Export"))
        {
            ExportMap();
        }
        
        ImGui::End();

        window->clear();

        for (std::vector<sf::Vertex> vec : redMap)
        {
            window->draw(&vec[0], vec.size(), sf::LineStrip);
        }

        for (auto vec : greenMap)
        {
            window->draw(&vec[0], vec.size(), sf::LineStrip);
        }

        for (auto vec : blueMap)
        {
            window->draw(&vec[0], vec.size(), sf::LineStrip);
        }

        window->draw(spawnPoint);
        window->draw(finishPoint);

        ImGui::SFML::Render(*window);

        window->display();
    }

    ImGui::SFML::Shutdown();

    delete window;

    return 0;
}