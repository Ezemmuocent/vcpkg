#include <SFML/Config.hpp>   // for SFML version macros
#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>

#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>

#include "AppConfig.h"
#include "LoginManager.h"
#include "Dashboard.h"
#include "MemberManager.h"
#include "DonationManager.h"
#include "TitheManager.h"
#include "OfferingManager.h"
#include "AttendanceManager.h"
#include "SermonManager.h"
#include "SocialMediaManager.h"
#include "LiveStreamManager.h"
#include "ProfileManager.h"
#include "AdminPanel.h"
#include "PdfGenerator.h"
#include "Utils.h"

using json = nlohmann::json;

int main() {
    // [OK] Cross-compatible window creation
#if (SFML_VERSION_MAJOR >= 3)
    // SFML 3.x
    sf::RenderWindow window(sf::VideoMode({ 1280u, 800u }), "CMakeChurchApp", sf::Style::Default);
#else
    // SFML 2.x
    sf::RenderWindow window(sf::VideoMode(1280, 800), "CMakeChurchApp", sf::Style::Default);
#endif

    window.setFramerateLimit(60);

    // [OK] Check ImGui-SFML initialization
    if (!ImGui::SFML::Init(window)) {
        std::cerr << "[ERROR] Failed to initialize ImGui-SFML!" << std::endl;
        return -1;
    }

    // Verify ImGui context exists
    if (ImGui::GetCurrentContext() == nullptr) {
        std::cerr << "[ERROR] ImGui context not created!" << std::endl;
        return -1;
    } else {
        std::cout << "[OK] ImGui context created successfully." << std::endl;
    }

    // load config
    json cfgJson;
    std::ifstream cfgFile("data/config.json");
    if (cfgFile.is_open()) {
        cfgFile >> cfgJson;
    }

    AppConfig config;
    config.appName = "MFM SOUTH EAST REGION 41 OLOKORO";
    config.churchName = "Mountain of Fire and Miracles Ministries Region 41 Olokoro";
    config.motto = "Where your hands are sent to war and your fingers to fight battle.";

    std::cout << "[OK] Starting " << config.appName << "..." << std::endl;

    // managers
    LoginManager login;
    if (!login.LoadAccounts("data/users.json")) {
        login.AddAccount("Ezemmuocent", "Davi2368", UserRole::Admin);
        login.AddAccount("user", "user123", UserRole::User);
    }

    DonationManager donations; donations.Load("data/donations.json");
    TitheManager tithes;       tithes.Load("data/tithes.json");
    OfferingManager offerings; offerings.Load("data/offerings.json");
    MemberManager members;     members.Load("data/members.json");
    AttendanceManager attendance; attendance.Load("data/attendance.json");
    SermonManager sermons;     sermons.Load("data/sermons.json");
    SocialMediaManager social; social.LoadFromConfig(cfgJson);
    LiveStreamManager live;    live.LoadFromConfig(cfgJson);
    ProfileManager profile;
    AdminPanel adminPanel(login, social, live, donations, tithes, offerings);

    Dashboard dashboard;
    dashboard.Init(&donations, &tithes, &offerings, &attendance, &members);
    dashboard.SetTitle("Church Dashboard");

    // UI state
    bool loggedIn = false;
    static char username[64] = "";
    static char password[64] = "";

    sf::Clock deltaClock;

    while (window.isOpen()) {
        // [OK] Cross-compatible event loop
#if (SFML_VERSION_MAJOR >= 3)
        while (auto event = window.pollEvent()) {
            ImGui::SFML::ProcessEvent(window, *event);
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        }
#else
        sf::Event event;
        while (window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(event);
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }
#endif

        ImGui::SFML::Update(window, deltaClock.restart());
        ImGui::Begin(config.appName.c_str());

        if (!loggedIn) {
            ImGui::Text("Login");
            ImGui::InputText("Username", username, sizeof(username));
            ImGui::InputText("Password", password, sizeof(password), ImGuiInputTextFlags_Password);

            if (ImGui::Button("Login")) {
                if (login.Login(username, password)) {
                    loggedIn = true;
                    username[0] = password[0] = 0;
                }
                else {
                    ImGui::OpenPopup("Login Failed");
                }
            }

            if (ImGui::BeginPopupModal("Login Failed", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
                ImGui::Text("Invalid credentials.");
                if (ImGui::Button("OK")) ImGui::CloseCurrentPopup();
                ImGui::EndPopup();
            }
        }
        else {
            // top controls
            ImGui::Text("User: %s", login.GetCurrentUser().c_str());
            ImGui::SameLine();
            if (ImGui::Button("Logout")) {
                login.Logout();
                loggedIn = false;
            }

            // main tabs
            if (ImGui::BeginTabBar("MainTabs")) {
                if (ImGui::BeginTabItem("Dashboard")) { dashboard.Render(); ImGui::EndTabItem(); }
                if (ImGui::BeginTabItem("Members")) { members.Render(); ImGui::EndTabItem(); }
                if (ImGui::BeginTabItem("Donations")) { donations.Render(); ImGui::EndTabItem(); }
                if (ImGui::BeginTabItem("Tithes")) { tithes.Render(); ImGui::EndTabItem(); }
                if (ImGui::BeginTabItem("Offerings")) { offerings.Render(); ImGui::EndTabItem(); }
                if (ImGui::BeginTabItem("Attendance")) { attendance.Render(); ImGui::EndTabItem(); }
                if (ImGui::BeginTabItem("Sermons")) { sermons.Render(); ImGui::EndTabItem(); }
                if (ImGui::BeginTabItem("Media Center")) {
                    social.Render();
                    ImGui::Separator();
                    live.Render();
                    ImGui::EndTabItem();
                }
                if (login.GetCurrentRole() == UserRole::Admin) {
                    adminPanel.Render(cfgJson);
                }
                ImGui::EndTabBar();
            }
        }

        ImGui::End();
        window.clear(sf::Color(40, 40, 48));
        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();
    return 0;
}
