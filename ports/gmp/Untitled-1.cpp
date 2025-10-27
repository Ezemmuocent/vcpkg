
struct AppConfig {
    std::string appTitle;
    std::string churchName;
    std::string motto;

    AppConfig(const std::string& title = "CMakeChurchApp",
        const std::string& church = "Mountain of Fire and Miracles Ministries Region 41 Olokoro",
        const std::string& mottoText = "Where your hands are sent to war and your fingers to fight battle")
        : appName(name), churchName(church), motto(mottoText) {
    }
};
