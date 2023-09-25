#include "headers.h"

static char lengthField[128] = "14";
int const defaultLength = 14;
int projectLength = defaultLength;
int daysPassed = 0;
string saveError = "";
string loadError = "";
string configFile = "config.txt";
string backgroundImage = "background.jpg";
bool imageIsLoaded = false;
bool reloadImage = false;
bool incorrectLength = false;
bool saveFailed = false;
bool loadFailed = false;
tm getCurrentDate();
tm today = getCurrentDate();
tm startDate = today;

int getDaysPassed() {
    system_clock::time_point start = system_clock::from_time_t(mktime(&startDate));
    system_clock::time_point end = system_clock::from_time_t(mktime(&today));

    duration<int, ratio<86400>> days = duration_cast<duration<int, ratio<86400>>>(end - start);

    return days.count() + 1;
};

tm getCurrentDate() {
    auto now = system_clock::now();
    time_t time = system_clock::to_time_t(now);
    tm date;
    localtime_s(&date, &time);
    return date;
};

int extractValue(const string& value) {
    smatch match;
    int result;
    try {
        regex_search(value, match, regex("(,?(\\d+))*\\.?(\\d+)"));
        string replaced = regex_replace(match.str(), regex(","), "");
        result = static_cast<int>(round(stod(replaced)));
    }
    catch (const exception& e) {
        cerr << "Failed to extract the length " << e.what() << endl;
        result = 0;
    };
    return result;
};

bool loadData() {
    string errorMessage = "";
    ifstream file(configFile);

    if (!file.is_open()) {
        loadFailed = true;
        loadError = "Failed to open " + configFile;
        return false;
    };

    try {
        string line;
        while (getline(file, line)) {
            istringstream iss(line);
            string key, value;
            getline(iss, key, '=');
            getline(iss, value);

            if (key == "startDate") {
                if (regex_match(value, regex("\\d{2}\\.\\d{2}\\.\\d{4}"))) {
                    istringstream iss(value);
                    iss >> get_time(&startDate, "%d.%m.%Y");
                    if (iss.fail()) errorMessage += "Unknown date \'" + value + "\'\n";
                }
                else {
                    errorMessage += "Incorrect date \'" + value + "\'\n";
                }
            }
            else if (key == "length") {
                projectLength = extractValue(value);
                if (projectLength == 0) {
                    projectLength = defaultLength;
                    errorMessage += "Incorrect project length \'" + value + "\'\n";
                };
                strcpy_s(lengthField, to_string(projectLength).c_str());
            }
            else errorMessage += "Wrong line \'" + key + "\'\n";
        };
        file.close();
        if (errorMessage.empty()) {
            loadFailed = false;
            loadError = "Success";
            return true;
        }
        else {
            loadFailed = true;
            loadError = "Failed to read " + configFile + ": \n" + errorMessage;
            return false;
        };
    }
    catch (const exception& e) {
        file.close();
        loadFailed = true;
        loadError = errorMessage.empty() ? e.what() : (string(e.what()) + ": \n" + errorMessage);
        return false;
    };
}

void renderImGui(sf::RenderWindow& window, sf::Clock& clock) {
    float windowWidth = (float)window.getSize().x;

    ImGui::SFML::Update(window, clock.restart());

    ImGui::SetNextWindowPos(ImVec2(windowWidth * 0.5f, 0.0f), 1, ImVec2(0.5f, 0.0f));
    ImGui::Begin("##Controll_Pannel", nullptr, flags);

    float symbolWidth = ImGui::CalcTextSize("9").x;
    ImVec2 buttonSize(symbolWidth * 9, 0.0f);

    ImGui::AlignTextToFramePadding();
    ImGui::Text("Project start date:");

    ImGui::SameLine();
    ImGui::SetNextItemWidth(symbolWidth * 15);
    if (ImGui::DateChooser("##Date_Picker", startDate, "%d.%m.%Y")) {
        daysPassed = getDaysPassed();
    };

    ImGui::SameLine();
    ImGui::Text("Project length:");

    ImGui::SameLine();
    ImGui::SetNextItemWidth(symbolWidth * 9);
    if (ImGui::InputText("##Input_Field", lengthField, sizeof(lengthField))) {
        int temp = extractValue(lengthField);
        strcpy_s(lengthField, to_string(temp).c_str());
        if (temp > 3000 || temp < 1) {
            incorrectLength = true;
        }
        else {
            projectLength = temp;
            incorrectLength = false;
        };
    };

    ImGui::SameLine();
    ImGui::SetNextItemWidth(symbolWidth * 9);

    if (ImGui::Button("Save##01", buttonSize)) {
        char dateToSave[32];
        try {
            strftime(dateToSave, sizeof(dateToSave), "%d.%m.%Y", &startDate);
            ofstream file(configFile);
            if (file.is_open()) {
                file << "startDate=" << dateToSave << '\n';
                file << "length=" << projectLength << '\n';
                file.close();
                saveFailed = false;
                saveError = "Success";
            }
            else {
                throw runtime_error("Saving failed. Cannot open " + configFile);
            };
        }
        catch (const exception& e) {
            saveFailed = true;
            saveError = e.what();
        };
    };

    if (!imageIsLoaded) {
        float itemSpacing = ImGui::GetStyle().ItemSpacing.x;
        float centerPosX = ImGui::GetContentRegionAvail().x * 0.5f + itemSpacing;

        ImGui::Separator();
        string msg = "Cannot load the image. Try again?";
        ImGui::SetCursorPosX(centerPosX - ImGui::CalcTextSize(msg.c_str()).x * 0.5f);
        ImGui::Text(msg.c_str());

        ImGui::SetCursorPosX(centerPosX - buttonSize.x - itemSpacing * 0.5f);
        if (ImGui::Button("Reload##02", buttonSize)) {
            reloadImage = true;
        };

        ImGui::SetCursorPosX(centerPosX + buttonSize.x - itemSpacing * 0.5f);
        ImGui::SameLine();
        if (ImGui::Button("Close##03", buttonSize)) {
            window.close();
        };
    };

    if (loadFailed) {
        float itemSpacing = ImGui::GetStyle().ItemSpacing.x;
        float centerPosX = ImGui::GetContentRegionAvail().x * 0.5f + itemSpacing;

        ImGui::Separator();
        ImGui::SetCursorPosX(centerPosX - ImGui::CalcTextSize(loadError.c_str()).x * 0.5f);
        ImGui::Text(loadError.c_str());

        ImGui::SetCursorPosX(centerPosX - buttonSize.x - itemSpacing * 0.5f);
        if (ImGui::Button("Reload##04", buttonSize)) {
            loadData();
        };

        ImGui::SetCursorPosX(centerPosX + buttonSize.x - itemSpacing * 0.5f);
        ImGui::SameLine();
        if (ImGui::Button("Hide##05", buttonSize)) {
            loadFailed = false;
        };
    };

    if (saveFailed) {
        float centerPosX = ImGui::GetContentRegionAvail().x * 0.5f + ImGui::GetStyle().ItemSpacing.x;
        ImGui::Separator();

        ImGui::SetCursorPosX(centerPosX - ImGui::CalcTextSize(saveError.c_str()).x * 0.5f);
        ImGui::Text(saveError.c_str());
        string msg = "Info will not be saved. Fix the issue and try to save again";
        ImGui::SetCursorPosX(centerPosX - ImGui::CalcTextSize(msg.c_str()).x * 0.5f);
        ImGui::Text(msg.c_str());

        ImGui::SetCursorPosX(centerPosX - buttonSize.x * 0.5f);

        if (ImGui::Button("Hide##06", buttonSize)) {
            saveFailed = false;
        };
    };

    if (incorrectLength) {
        float itemSpacing = ImGui::GetStyle().ItemSpacing.x;
        float centerPosX = ImGui::GetContentRegionAvail().x * 0.5f;
        string errorText(lengthField + string(u8" (° _°) Are you sure this is your project length?"));

        ImGui::Separator();
        ImGui::SetCursorPosX(centerPosX + ImGui::GetStyle().ItemSpacing.x - ImGui::CalcTextSize(errorText.c_str()).x * 0.5f);
        ImGui::Text(errorText.c_str());

        ImGui::SetCursorPosX(centerPosX - buttonSize.x * 0.5f + itemSpacing );
        
        if (ImGui::Button("OK##06", buttonSize)) {
            projectLength = extractValue(lengthField);
            incorrectLength = false;
        };
    };

    //ImGui::ShowDemoWindow();
    ImGui::End();
    ImGui::SFML::Render(window);
};

void setIcons(sf::RenderWindow& window) {
    HICON hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_WINDOWICON));  // Title bar icon
    HICON hIconEXE = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_FILEICON)); // Exe icon
    HWND hWnd = window.getSystemHandle();
    SendMessage(hWnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);   // Setting up the icon for the title bar
    SendMessage(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon); // Setting up the icon for the title bar
    SetClassLong(GetConsoleWindow(), GCLP_HICON, reinterpret_cast<LONG_PTR>(hIconEXE));   // Setting up the icon for the exe file
    SetClassLong(GetConsoleWindow(), GCLP_HICONSM, reinterpret_cast<LONG_PTR>(hIconEXE)); // Setting up the icon for the exe file
};

int getNearestMultiple(int value) {
    const int multiple = (int)round(value * 0.025f);
    if (multiple < 2.1f) return 1;

    vector<int> a = {0, 5, 10, 25, 50, 100, 250, 500 };
    int nearest = 5;

    for (int i = 0; i < 7; i++) {
        if (multiple >= a[i] && multiple < a[i + 1]) {
            if (multiple - a[i] <= a[i + 1] - multiple) {
                return a[i];
            }
            else {
                return a[i + 1];
            };
        };
    };
    return a[7];
};

bool setImguiFont()
{
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImFontAtlas* atlas = io.Fonts;
    ImFontConfig cfg;
    ImVector<ImWchar> ranges;
    ImFontGlyphRangesBuilder builder;
    builder.AddRanges(io.Fonts->GetGlyphRangesCyrillic());
    builder.AddText(u8"╯°□︵┻━");
    builder.BuildRanges(&ranges);
    cfg.FontBuilderFlags |= ImGuiFreeTypeBuilderFlags::ImGuiFreeTypeBuilderFlags_MonoHinting;
    //cfg.FontBuilderFlags |= ImGuiFreeTypeBuilderFlags::ImGuiFreeTypeBuilderFlags_ForceAutoHint;
    cfg.FontDataOwnedByAtlas = false;
    io.Fonts->AddFontFromMemoryTTF((void*)RobotoMArial_data, RobotoMArial_size, 16.0f, &cfg, ranges.Data);
    for (int n = 0; n < atlas->ConfigData.Size; n++) ((ImFontConfig*)&atlas->ConfigData[n])->RasterizerMultiply = 1.0f;
    atlas->FontBuilderIO = ImGuiFreeType::GetBuilderForFreeType();
    atlas->Build();
    return true;
};

class RenderCallbacks : public sf::WindowCallbacks
{
public:
    RenderCallbacks(sf::RenderWindow& mainWindow)
        : window(mainWindow)
    {
        if (imageTexture.loadFromFile(backgroundImage)) {
            backgroundSprite.setTexture(imageTexture);
            imageIsLoaded = true;
            textureSize.x = imageTexture.getSize().x;
            textureSize.y = imageTexture.getSize().y;
        }
        else {
            imageIsLoaded = false;

            turnTable.setString(L"\u0028\u256F\u00B0\u25A1\u00B0\u0029\u256F\uFE35\u253B\u2501\u253B"); //const wchar_t imageErrorText[] = { 0x0028, 0x256F, 0x00B0, 0x25A1, 0x00B0, 0x0029, 0x256F, 0xFE35, 0x253B, 0x2501, 0x253B, 0 };
            turnTable.setFont(font);
            turnTable.setFillColor(sf::Color::Black);
            imageError.setString("Cannot load image \"" + backgroundImage + "\"\n          Is it in the current folder?");
            imageError.setFont(font);
            imageError.setFillColor(sf::Color::Black);
        };

        font.loadFromMemory(RobotoMArial_data, RobotoMArial_size);

        line.setFillColor(sf::Color::Green);

        todayText.setFont(font);
        todayText.setFillColor(sf::Color::Green);
        dayNumber.setString("0");
        dayNumber.setFont(font);
        dayNumber.setFillColor(sf::Color::Blue);

        renderTexture.create(3840, 2160);
    };

    void asyncRender(const sf::Event* optionalResizedEvent) override
    {
        sf::Vector2i sizeNow(window.getSize());
        renderTexture.clear(sf::Color{ 240, 240, 240 });

        if (imageIsLoaded) {
            if (sizeNow.x != windowSize.x || sizeNow.y != windowSize.y) {
                windowSize = sizeNow;
                scale = min((float)windowSize.x / textureSize.x, (float)windowSize.y / textureSize.y);
                image.x = textureSize.x * scale;
                image.y = textureSize.y * scale;
                offset.x = (windowSize.x - image.x) / 2.f;
                offset.y = (windowSize.y - image.y) / 2.f;
                fontSize = (int)(image.y * 0.026f);
                dayNumberY = offset.y + image.y * 0.93f;

                window.setView(sf::View(sf::FloatRect(0.f, 0.f, (float)windowSize.x, (float)windowSize.y)));
                backgroundSprite.setScale(scale, scale);
                backgroundSprite.setPosition(offset.x, offset.y);

                todayText.setString("Day " + to_string(daysPassed));
                todayText.setCharacterSize(fontSize);
                sf::FloatRect todayTextBounds = todayText.getLocalBounds();
                todayText.setOrigin((todayTextBounds.left + todayTextBounds.width) / 2, todayTextBounds.top + todayTextBounds.height);
                line.setSize(sf::Vector2f(2.f, image.y - todayTextBounds.height));
            };

            float incrementX = image.x / projectLength;
            float lineX = offset.x + daysPassed * incrementX - incrementX * 0.5f;
            int skipDays = getNearestMultiple(projectLength);
            int fontSizeByWidth = (int)(image.x * 1.3f / (projectLength * to_string(projectLength).length() / skipDays));
            todayText.setString("Day " + to_string(daysPassed));

            float leftSide = todayText.getOrigin().x + offset.x;
            float rightSide = offset.x + image.x - todayText.getOrigin().x;
            if (lineX < leftSide) {
                todayText.setPosition(leftSide, offset.y + image.y);
            }
            else if (lineX > rightSide) {
                todayText.setPosition(rightSide, offset.y + image.y);
            }
            else {
                todayText.setPosition(lineX, offset.y + image.y);
            };

            line.setPosition(lineX, offset.y);
            dayNumber.setCharacterSize(fontSizeByWidth < fontSize ? fontSizeByWidth : fontSize);

            renderTexture.draw(backgroundSprite);
            renderTexture.draw(line);
            renderTexture.draw(todayText);

            for (int i = 1; i < projectLength; i++) {
                bool canBeDrawn = i % skipDays == 0;
                if (canBeDrawn) {
                    dayNumber.setString(to_string(i));
                    sf::FloatRect DNBounds = dayNumber.getLocalBounds();
                    float dayNumberX = offset.x + i * incrementX - incrementX * 0.5f;
                    dayNumber.setOrigin(DNBounds.left + DNBounds.width / 2, 0);
                    dayNumber.setPosition(dayNumberX, dayNumberY);
                    renderTexture.draw(dayNumber);
                };
            };
        }
        else {
            if (sizeNow.x != windowSize.x || sizeNow.y != windowSize.y) {
                windowSize = sizeNow;
                window.setView(sf::View(sf::FloatRect(0.f, 0.f, (float)windowSize.x, (float)windowSize.y)));
                fontSize2 = (int)(windowSize.y * 0.05f);
                errorTextPos.x = windowSize.x / 2.f;
                errorTextPos.y = windowSize.y / 2.f - fontSize2;

                turnTable.setCharacterSize(fontSize2);
                sf::FloatRect turnTableBounds = turnTable.getLocalBounds();
                turnTable.setOrigin((turnTableBounds.left + turnTableBounds.width) / 2.f, 0.f);
                turnTable.setPosition(errorTextPos.x, errorTextPos.y);

                imageError.setCharacterSize((int)(fontSize2 * 0.75f));
                sf::FloatRect imageErrorBounds = imageError.getLocalBounds();
                imageError.setOrigin((imageErrorBounds.left + imageErrorBounds.width) / 2.f, -turnTableBounds.height * 1.2f);
                imageError.setPosition(errorTextPos.x, errorTextPos.y);
            };

            renderTexture.draw(turnTable);
            renderTexture.draw(imageError);

            if (reloadImage) {
                if (imageTexture.loadFromFile(backgroundImage)) {
                    backgroundSprite.setTexture(imageTexture);
                    textureSize.x = imageTexture.getSize().x;
                    textureSize.y = imageTexture.getSize().y;
                    imageIsLoaded = true;
                    reloadImage = false;
                    windowSize = sf::Vector2i(0, 0);
                }
                else reloadImage = false;
            };
        };
        renderTexture.display();
        renderSprite.setTexture(renderTexture.getTexture());

        window.draw(renderSprite);
        renderImGui(window, clock);
        window.display();
    };

private:
    sf::RenderWindow& window;
    sf::Sprite renderSprite;
    sf::RenderTexture renderTexture;
    sf::Texture imageTexture;
    sf::Clock clock;
    sf::Font font;
    sf::RectangleShape line;
    sf::Sprite backgroundSprite;
    sf::Text todayText;
    sf::Text dayNumber;
    sf::Text turnTable;
    sf::Text imageError;
    sf::Vector2i textureSize;
    sf::Vector2i windowSize;
    sf::Vector2f image;
    sf::Vector2f offset;
    sf::Vector2f errorTextPos;
    float scale = 1;
    float dayNumberY = 0.0f;
    int fontSize = (int)(image.y * 0.026f);
    int fontSize2 = fontSize;
    int sideOffset = (int)(image.x * 0.0165f);
};

int main() {
    sf::RenderWindow window(sf::VideoMode(1280, 760), "TimeLine Tracker");
    sf::Event event;
    RenderCallbacks windowCallbacks(window);
    window.setWindowCallbacks(&windowCallbacks);
    window.setFramerateLimit(150);
    window.setVerticalSyncEnabled(true);
    setIcons(window);

    ImGui::SFML::Init(window, false);
    setImguiFont();
    setImguiStyle();
    ImGui::SFML::UpdateFontTexture();

    loadData();
    daysPassed = getDaysPassed();

    while (window.isOpen()) {
        while (window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(event);
            if (event.type == sf::Event::Closed) {
                window.close();
            };
        };
        windowCallbacks.asyncRender(nullptr);
    };

    ImGui::SFML::Shutdown();
    return 0;
};