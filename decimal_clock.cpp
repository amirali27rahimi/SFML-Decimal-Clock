#include <SFML/Graphics.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/System/Clock.hpp>
#include <iostream>
#include <string>
#include <cmath>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <fstream>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

int main() {
    
    // --- 1. Window and Font Setup ---
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    sf::RenderWindow window(sf::VideoMode(600, 600), "SFML Decimal Clock", sf::Style::Close, settings);
    window.setFramerateLimit(60);

    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        std::cerr << "Error: Font 'arial.ttf' not found. Exiting.\n";
        return 1;
    }

    // --- 2. Icon Setup (Assuming icon.png) ---
    sf::Image icon;
    if (icon.loadFromFile("icon.png")) { 
        window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
    } else {
        std::cerr << "Warning: Icon 'icon.png' not loaded. Proceeding without custom icon.\n";
    }
    
    // --- 3. Clock Graphics Setup (Adjusted for 600x600) ---
    sf::Vector2f center(300.f, 300.f);
    float radius = 250.f; 
    float thickness = 30.f; 

    // Text Elements (موقعیت‌ها تنظیم شده‌اند)
    sf::Text normalTimeText("Loading...", font, 24);
    normalTimeText.setOrigin(normalTimeText.getLocalBounds().width / 2.f, 0.f); 
    normalTimeText.setPosition(center.x, center.y - 150.f); 

    sf::Text decimalTimeText("Loading...", font, 30);
    decimalTimeText.setOrigin(decimalTimeText.getLocalBounds().width / 2.f, decimalTimeText.getLocalBounds().height / 2.f);
    decimalTimeText.setPosition(center.x, center.y);

    // Main Loop
    while (window.isOpen()) {
        // Event Handling
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed || 
               (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)) {
                window.close();
            }
        }

        // Time Acquisition 
        auto now = std::chrono::system_clock::now();
        auto duration = now.time_since_epoch();
        std::time_t tt = std::chrono::duration_cast<std::chrono::seconds>(duration).count();

        struct tm ltm;
        if (localtime_r(&tt, &ltm) == nullptr) {
            std::cerr << "Error getting local time\n";
            continue;
        }

        int hour = ltm.tm_hour;
        int minute = ltm.tm_min;
        int second = ltm.tm_sec;

        // Standard 24h Time Display
        std::ostringstream ossNormal;
        ossNormal << std::setfill('0') << std::setw(2) << hour << ":"
                  << std::setw(2) << minute << ":" << std::setw(2) << second;
        normalTimeText.setString("24h Time: " + ossNormal.str());
        normalTimeText.setOrigin(normalTimeText.getLocalBounds().width / 2.f, 0.f); 

        // Decimal Time Calculation
        double secondsInDay = hour * 3600.0 + minute * 60.0 + second;
        double fractionOfDay = secondsInDay / 86400.0; 
        double decimalSeconds = fractionOfDay * 100000.0; 

        int decHours = static_cast<int>(decimalSeconds / 10000.0);
        int decMinutes = static_cast<int>((static_cast<int>(decimalSeconds) % 10000) / 100);
        int decSeconds = static_cast<int>(decimalSeconds) % 100;

        std::ostringstream ossDecimal;
        ossDecimal << std::setfill('0') << std::setw(2) << decHours << ":"
                   << std::setw(2) << decMinutes << ":" << std::setw(2) << decSeconds;
        decimalTimeText.setString("Decimal: " + ossDecimal.str());
        decimalTimeText.setOrigin(decimalTimeText.getLocalBounds().width / 2.f, decimalTimeText.getLocalBounds().height / 2.f); 

        // Drawing Progress Arcs
        sf::VertexArray arc(sf::TriangleStrip);
        sf::VertexArray remainingArc(sf::TriangleStrip);

        int points = 180;
        float startAngle = -M_PI / 2.0f;
        float usedAngle = static_cast<float>(2.0 * M_PI * fractionOfDay);

        // Used Section (Blue - A bit translucent for glass effect)
        sf::Color usedColor(50, 180, 255, 180); 
        for (int i = 0; i <= points; ++i) {
            float angle = startAngle + (usedAngle * i / points);
            sf::Vector2f outer(center.x + std::cos(angle) * radius, center.y + std::sin(angle) * radius);
            sf::Vector2f inner(center.x + std::cos(angle) * (radius - thickness), center.y + std::sin(angle) * (radius - thickness));
            arc.append(sf::Vertex(outer, usedColor));
            arc.append(sf::Vertex(inner, usedColor));
        }

        // Remaining Section (Frosted Glass Effect - White/Light Gray with High Opacity)
        sf::Color glassColor(255, 255, 255, 100);
        for (int i = 0; i <= points; ++i) {
            float angle = startAngle + usedAngle + ((2.0f * M_PI - usedAngle) * i / points);
            sf::Vector2f outer(center.x + std::cos(angle) * radius, center.y + std::sin(angle) * radius);
            sf::Vector2f inner(center.x + std::cos(angle) * (radius - thickness), center.y + std::sin(angle) * (radius - thickness));
            remainingArc.append(sf::Vertex(outer, glassColor));
            remainingArc.append(sf::Vertex(inner, glassColor));
        }

        // Final Render
        window.clear(sf::Color(20, 20, 40)); 
        
        // Drawing order is important for layering
        window.draw(arc);
        window.draw(remainingArc);
        window.draw(normalTimeText);
        window.draw(decimalTimeText);
        window.display();
    }

    return 0;
}
